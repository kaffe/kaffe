/*
 * jar.c
 * Handle JAR input files.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include "config.h"
#include "debug.h"
#include "config-std.h"
#include "config-io.h"
#include "config-mem.h"
#include "gtypes.h"
#include "jsyscall.h"
#include "inflate.h"
#include "jar.h"
#include "gc.h"
#include "stats.h"
#include "files.h"

static inline int
jar_read(jarFile* file, char *buf, off_t len)
{
#ifdef HAVE_MMAP
    if (file->data != (char*)-1) {
        if (file->offset + len > file->size)
	    len = file->size - file->offset;
	if (len <= 0)
	    return 0;
	memcpy(buf, file->data + file->offset, len);
	file->offset += len;
	return len;
    }
    else
#endif
    {
	int rc;
	ssize_t bread;
	rc = KREAD(file->fd, buf, len, &bread);
	if (rc) {
	    file->error = SYS_ERROR(rc);
	}
	return (rc == 0 ? bread : -1);
    }
}

static inline off_t
jar_lseek(jarFile* file, off_t offset, int whence)
{
#ifdef HAVE_MMAP
    if (file->data != (char*)-1) {
	off_t pos;

	switch (whence) {
	    case SEEK_CUR:
	        pos = file->offset + offset;
		break;
	    case SEEK_SET:
		pos = offset;
		break;
	    case SEEK_END:
	        pos = file->size + offset;
		break;
	    default:
		return (off_t)-1;
	}
	if (pos < 0)
	    return (off_t)-1;
	else if (pos > file->size)
	    return (off_t)-1;
	file->offset = pos;
	return pos;
    }
    else
#endif
    {
	off_t off;
	int rc = KLSEEK(file->fd, offset, whence, &off);
	return (rc == 0 ? off : -1);
    }
}

/*
 * Read in a central directory record.
 */
static
jarEntry*
readCentralDirRecord(jarFile* file)
{
	INITREADS();
	jarCentralDirectoryRecord head;
	jarEntry* ret;
	int len, extra;
	off_t pos;

	head.signature = READ32(file);
	head.versionMade = READ16(file);
	head.versionExtract = READ16(file);
	head.flags = READ16(file);
	head.compressionMethod = READ16(file);
	head.lastModifiedTime = READ16(file);
	head.lastModifiedDate = READ16(file);
	head.crc = READ32(file);
	head.compressedSize = READ32(file);
	head.uncompressedSize = READ32(file);
	head.fileNameLength = READ16(file);
	head.extraFieldLength = READ16(file);
	head.fileCommentLength = READ16(file);
	head.diskNumberStart = READ16(file);
	head.internalFileAttribute = READ16(file);
	head.externalFileAttribute = READ32(file);
	head.relativeLocalHeaderOffset = READ32(file);

	if (head.signature != CENTRALHEADERSIGNATURE) {
		file->error = "Bad central record signature";
		return (0);
	}

	len = sizeof(jarEntry) + (head.fileNameLength + 1);
	ret = KMALLOC(len);
	if (!ret) {
		file->error = "out of memory";
		return 0;
	}
	addToCounter(&jarmem, "vmmem-jar files", 1, GCSIZEOF(ret));
DBG(JARFILES,	
	dprintf("Entry at: %p/len=%d usize%d\n", ret, len, 
	    head.uncompressedSize);	
    )

	ret->next = 0;
	ret->fileName = (char*)((uintp)ret + sizeof(jarEntry));
	ret->compressionMethod  = head.compressionMethod;
        ret->compressedSize = head.compressedSize;
        ret->uncompressedSize = head.uncompressedSize;
	ret->dosTime = ((head.lastModifiedDate << 16)
			| head.lastModifiedTime);

	READBYTES(file, head.fileNameLength, ret->fileName);
	SKIPBYTES(file, head.extraFieldLength + head.fileCommentLength);

DBG(JARFILES,	
	dprintf("Central record filename: %s\n", ret->fileName);	)

	/* Compute file data location using local header info */
	pos = jar_lseek(file, (off_t)0, SEEK_CUR);
	(void)jar_lseek(file, (off_t)(head.relativeLocalHeaderOffset + 28), SEEK_SET);
	extra = READ16(file);
	ret->dataPos = head.relativeLocalHeaderOffset
		+ SIZEOFLOCALHEADER + head.fileNameLength + extra;

	/* Jump back to original central directory position */
	(void)jar_lseek(file, pos, SEEK_SET);
	return (ret);
}

/*
 * Find first central directory record and return nr of actual records.
 * NB. We assume there's not comments in JARs.
 */
static
int
findFirstCentralDirRecord(jarFile* file)
{
	INITREADS();
	uint32 signature;
	uint32 ign;
	uint16 sz;
	uint32 off;

	if (jar_lseek(file, -SIZEOFCENTRALEND, SEEK_END) == -1) {
		file->error = "Failed to seek into JAR file";
		return (0);
	}

	signature = READ32(file);
	if (signature != CENTRALENDSIGNATURE) {
		file->error = "Failed to find end of JAR record";
		return (0);
	}

	ign = READ16(file);	/* Nr of disk */
	ign = READ16(file);	/* Nr of disk with central directory */
	ign = READ16(file);	/* Nr of entries in central directory on this disk */
	sz = READ16(file);	/* Nr of entries in central directory */
	ign = READ32(file);	/* Size of central directory */
	off = READ32(file);	/* Offset of central directory */

	if (jar_lseek(file, (off_t)off, SEEK_SET) == -1) {
		file->error = "Failed to seek into central directory offset";
		return (0);
	}

	return (sz);
}

jarFile*
openJarFile(char* name)
{
#define read_checked(file, ent)					\
	{							\
		(ent) = readCentralDirRecord(file);		\
		if (!(ent)) {					\
			closeJarFile(file);			\
			return 0;				\
		}						\
		addToCounter(&jarmem, "vmmem-jar files", 1,	\
			     (jlong)GCSIZEOF((ent)));		\
	}
	jarFile* file;
	jarEntry* curr;
	int i;
	int rc;

	file = KMALLOC(sizeof(jarFile));
	if (!file) {
		return 0;
	}

	rc = KOPEN(name, O_RDONLY|O_BINARY, 0, &file->fd);
	if (rc) {
		KFREE(file);
		return (0);
	}
#ifdef HAVE_MMAP
	rc = KLSEEK(file->fd, 0, SEEK_END, &file->size);
	if (rc) {
		KCLOSE(file->fd);
		KFREE(file);
		return (0);
	}
	/* XXX unprotected -> make part of jsyscall interface! */
	file->data = mmap(NULL, file->size, PROT_READ, MAP_SHARED, file->fd, 0);
	if (file->data != (char*)-1) {
		KCLOSE(file->fd);
		file->offset = 0;
	}
#endif
	addToCounter(&jarmem, "vmmem-jar files", 1, GCSIZEOF(file));
	i = findFirstCentralDirRecord(file);
	file->count = i;
	if (i > 0) {
		read_checked(file, curr);
		file->head = curr;
		for (i--; i > 0; i--) {
			read_checked(file, curr->next);
			curr = curr->next;
		}
	}

	return (file);
#undef read_checked
}

jarEntry*
lookupJarFile(jarFile* file, char* entry)
{
	jarEntry* curr;

	curr = file->head;
	while (curr != 0) {
		if (strcmp(entry, curr->fileName) == 0) {
			return (curr);
		}
		curr = curr->next;
	}
	return (0);
}

uint8*
getDataJarFile(jarFile* file, jarEntry* entry)
{
	uint8* buf;
	uint8* nbuf;

	if (jar_lseek(file, (off_t)entry->dataPos, SEEK_SET) == -1) {
		file->error = "Failed to seek into JAR file";
		return (0);
	}
	if (entry->compressedSize == 0)
		return KMALLOC(4); // XXX Send something back
	buf = KMALLOC(entry->compressedSize);
	if (!buf) {
		file->error = "Out of memory";
		return (0);
	}
	if (jar_read(file, buf, entry->compressedSize) != entry->compressedSize) {
		KFREE(buf);
		return (0);
	}
	/* Decompress data */
	switch (entry->compressionMethod) {
	case COMPRESSION_STORED:
		return (buf);

	case COMPRESSION_DEFLATED:
		nbuf = KMALLOC(entry->uncompressedSize);
		if (nbuf && inflate_oneshot(buf, entry->compressedSize, nbuf, entry->uncompressedSize) == 0) {
			addToCounter(&jarmem, "vmmem-jar files", 1, GCSIZEOF(nbuf));
			KFREE(buf);
			return (nbuf);
		}
		file->error = "Decompression failed";
		KFREE(buf);
		KFREE(nbuf);
		return (0);

	/* These are not supported yet */
	case COMPRESSION_SHRUNK:
	case COMPRESSION_REDUCED1:
	case COMPRESSION_REDUCED2:
	case COMPRESSION_REDUCED3:
	case COMPRESSION_REDUCED4:
	case COMPRESSION_IMPLODED:
	case COMPRESSION_TOKENIZED:
	default:
		file->error = "Unsupported compression in JAR file";
		KFREE(buf);
		return (0);
	}
}

void
closeJarFile(jarFile* file)
{
	jarEntry* curr;
	jarEntry* next;

	for (curr = file->head; curr != 0; curr = next) {
		next = curr->next;
		addToCounter(&jarmem, "vmmem-jar files", 1, -(jlong)GCSIZEOF(curr));
		KFREE(curr);
	}

#ifdef HAVE_MMAP
	if (file->data != (char*)-1)
		munmap(file->data, file->size);
	else
#endif
	KCLOSE(file->fd);
	addToCounter(&jarmem, "vmmem-jar files", 1, -(jlong)GCSIZEOF(file));
	KFREE(file);
}

