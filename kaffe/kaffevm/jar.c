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

#define DBG(s)

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "jar.h"

#if !defined(KAFFEH)
#include "gc.h"
#else
#define	gc_malloc_fixed(S)	calloc(S, 1)
#define	gc_free_fixed(M)	free(M)
#endif

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
	int len;

	head.signature = READ32(file->fp);
	head.versionMade = READ16(file->fp);
	head.versionExtract = READ16(file->fp);
	head.flags = READ16(file->fp);
	head.compressionMethod = READ16(file->fp);
	head.lastModifiedTime = READ16(file->fp);
	head.lastModifiedDate = READ16(file->fp);
	head.crc = READ32(file->fp);
	head.compressedSize = READ32(file->fp);
	head.uncompressedSize = READ32(file->fp);
	head.fileNameLength = READ16(file->fp);
	head.extraFieldLength = READ16(file->fp);
	head.fileCommentLength = READ16(file->fp);
	head.diskNumberStart = READ16(file->fp);
	head.internalFileAttribute = READ16(file->fp);
	head.externalFileAttribute = READ32(file->fp);
	head.relativeLocalHeaderOffset = READ32(file->fp);

	if (head.signature != CENTRALHEADERSIGNATURE) {
		file->error = "Bad central record signature";
		return (0);
	}

	len = sizeof(jarEntry) + (head.fileNameLength + 1);
	ret = gc_malloc_fixed(len);

	ret->next = 0;
	ret->fileName = (char*)((uintp)ret + sizeof(jarEntry));
	ret->compressionMethod  = head.compressionMethod;
        ret->compressedSize = head.compressedSize;
        ret->uncompressedSize = head.uncompressedSize;

	READBYTES(file->fp, head.fileNameLength, ret->fileName);
	SKIPBYTES(file->fp, head.extraFieldLength);
	SKIPBYTES(file->fp, head.fileCommentLength);

DBG(	printf("Central record filename: %s\n", ret->fileName);		)

	ret->dataPos = head.relativeLocalHeaderOffset + SIZEOFLOCALHEADER + head.fileNameLength + head.extraFieldLength;
	/* HACK HACK HACK */
	if (head.extraFieldLength != 0) {
		ret->dataPos += 4;
	}

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

	if (fseek(file->fp, -SIZEOFCENTRALEND, SEEK_END) != 0) {
		file->error = "Failed to seek into JAR file";
		return (0);
	}

	signature = READ32(file->fp);
	if (signature != CENTRALENDSIGNATURE) {
		file->error = "Failed to find end of JAR record";
		return (0);
	}

	ign = READ16(file->fp);	/* Nr of disk */
	ign = READ16(file->fp);	/* Nr of disk with central directory */
	ign = READ16(file->fp);	/* Nr of entries in central directory on this disk */
	sz = READ16(file->fp);	/* Nr of entries in central directory */
	ign = READ32(file->fp);	/* Size of central directory */
	off = READ32(file->fp);	/* Offset of central directory */

	fseek(file->fp, off, SEEK_SET);

	return (sz);
}

jarFile*
openJarFile(char* name)
{
	jarFile* file;
	jarEntry* curr;
	int i;

	file = gc_malloc_fixed(sizeof(jarFile));

	file->fp = fopen(name, "rb");
	if (file->fp == 0) {
		return (0);
	}

	i = findFirstCentralDirRecord(file);
	file->count = i;
	if (i > 0) {
		curr = readCentralDirRecord(file);
		file->head = curr;
		for (i--; i > 0; i--) {
			curr->next = readCentralDirRecord(file);
			curr = curr->next;
		}
	}

	return (file);
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

	if (fseek(file->fp, entry->dataPos, SEEK_SET) != 0) {
		file->error = "Failed to seek into JAR file";
		return (0);
	}
	buf = gc_malloc_fixed(entry->compressedSize);
	if (fread(buf, sizeof(uint8), entry->compressedSize, file->fp) != entry->compressedSize) {
		file->error = "Failed to read from JAR file";
		gc_free_fixed(buf);
		return (0);
	}

	/* Decompress data */
	switch (entry->compressionMethod) {
	case COMPRESSION_STORED:
		return (buf);

	case COMPRESSION_DEFLATED:
		nbuf = gc_malloc_fixed(entry->uncompressedSize);
		if (inflate_oneshot(buf, entry->compressedSize, nbuf, entry->uncompressedSize) == 0) {
			gc_free_fixed(buf);
			return (nbuf);
		}
		gc_free_fixed(buf);
		gc_free_fixed(nbuf);
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
		gc_free_fixed(buf);
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
		gc_free_fixed(curr);
	}

	fclose(file->fp);

	gc_free_fixed(file);
}
