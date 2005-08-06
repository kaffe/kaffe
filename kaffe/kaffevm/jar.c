/*
 * jar.c
 * Handle JAR input files.
 *
 * Copyright (c) 2000, 2001, 2002 The University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.  
 * See the file "license.terms" for restrictions on redistribution 
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
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

/* Undefine this to make jar files mutable during the vm lifetime */
/* #define STATIC_JAR_FILES */

#if defined(KAFFEH)
#undef KAFFEVM_ABORT
#define KAFFEVM_ABORT() abort()
#undef  initStaticLock
#define initStaticLock(x)
#undef  staticLockIsInitialized
#define staticLockIsInitialized(x)	1
#undef  lockStaticMutex
#undef  unlockStaticMutex
#define unlockStaticMutex(x)
#define lockStaticMutex(x)
#undef  lockMutex
#undef  unlockMutex
#define lockMutex(x)
#define unlockMutex(x)
#undef destroyStaticLock
#define destroyStaticLock(x)
#endif

/*
 * Error messages.
 */

static const char * JAR_ERROR_BAD_CENTRAL_RECORD_SIGNATURE = "Bad central record signature";
static const char * JAR_ERROR_BAD_SIGNATURE                = "Bad signature";
static const char * JAR_ERROR_DECOMPRESSION_FAILED         = "Decompression failed";
static const char * JAR_ERROR_ENTRY_COUNT_MISMATCH         = "Entry count doesn't match directory size";
static const char * JAR_ERROR_IMPOSSIBLY_LARGE_DIRECTORY   = "Impossibly large directory size";
static const char * JAR_ERROR_IO                           = "I/O error";
static const char * JAR_ERROR_NO_END                       = "Failed to find end of JAR record";
static const char * JAR_ERROR_OUT_OF_MEMORY                = "Out of memory";
static const char * JAR_ERROR_TRUNCATED_FILE               = "Truncated file";
static const char * JAR_ERROR_UNSUPPORTED_COMPRESSION      = "Unsupported compression in JAR file";

/*
 * The jarCache keeps a list of all the jarFiles cached in the system.
 * However, since some JAR files are opened and closed frequently we don't
 * actively flush unused files from the system unless there are more than
 * JAR_FILE_CACHE_MAX jarFiles in the system.  This means we'll keep some
 * jarFiles in memory longer than we should but it helps to avoid the
 * constant opening/closing some java code does.
 */
struct _jarCache {
#if !defined(KAFFEH)
	iStaticLock	lock;
#endif
	jarFile *files;
#define JAR_FILE_CACHE_MAX 12
	unsigned int count;
};

static struct _jarCache jarCache;

/*
 * Hash a file name, the hash value is stored in what `hash' points to.
 */
static unsigned int hashName(const char *name)
{
	unsigned int hash = 0;

	assert(name != 0);
	
	for( hash = 0; *name; name++ )
		hash = (31 * hash) + (*name);

	return hash;
}

/*
 * Find a cached jarFile object.  If the file is found, it is returned and its
 * user count is incremented.
 *
 */
static jarFile *findCachedJarFile(char *name)
{
	jarFile *curr, **prev, *retval = NULL;

	assert(name != NULL);
	
	lockStaticMutex(&jarCache.lock);
	curr = jarCache.files;
	prev = &jarCache.files;
	while( curr && !retval )
	{
		assert(curr != NULL);
		assert(curr->fileName != NULL);
		
		if( !strcmp(curr->fileName, name) )
		{
			/* unlink it... */
			*prev = curr->next;
			/* and move it to the front */
			curr->next = jarCache.files;
			jarCache.files = curr;
			/* Return this node and increment the user count */
			retval = curr;
			retval->users++;

DBG(JARFILES,  dprintf("Found cached jar file %s, %d users\n", retval->fileName, retval->users); );

			assert(retval->users >= 1);
		}
		prev = &curr->next;
		curr = curr->next;
	}
	unlockStaticMutex(&jarCache.lock);
	return( retval );
}

/*
 * Free the contents of the entry table.
 */
static void collectEntryTable(jarFile *jf)
{
	assert(jf != 0);
	assert(jf->users == 0);
	
	if( jf->table )
	{
		addToCounter(&jarmem, "vmmem-jar files",
			     1, -(jlong)GCSIZEOF(jf->table));
		gc_free(jf->table);
		jf->table = NULL;
	}
}

/*
 * Free a jarFile structure and its child objects.
 */
static void collectJarFile(jarFile *jf)
{
	assert(jf != NULL);
	assert(jf->users == 0);
	assert(!(jf->flags & JFF_CACHED));
	
	collectEntryTable(jf);
	/* Make sure we free everything */
	if( jf->fd != -1 )
	{
		/* Close the file */
		KCLOSE(jf->fd);
		jf->fd = -1;
	}
#ifdef HAVE_MMAP
	if( jf->data != MAP_FAILED )
	{
#if !defined(NDEBUG)
		/* Only define rc for use in assert */
		int rc = 
#endif /* defined(NDEBUG)  */
		munmap(jf->data, jf->size);

		assert(rc == 0);
	}
#endif
	addToCounter(&jarmem, "vmmem-jar files", 1, -(jlong)GCSIZEOF(jf));
	destroyStaticLock(&jf->lock);
	gc_free(jf);
}

/*
 * Cache a jarFile object, if the passed in object matches one in the cache
 * it will be thrown away, and the cached one is returned, with its user
 * count incremented.
 */
static jarFile *cacheJarFile(jarFile *jf)
{
	jarFile *curr, **prev, **lru = NULL, *dead_jar = NULL, *retval = jf;
	int already_cached = 0;

	assert(jf != 0);
	assert(!(jf->flags & JFF_CACHED));
	
	lockStaticMutex(&jarCache.lock);
	/*
	 * Walk the cache, if the file we're trying to cache already matches
	 * one in the cache then we can just use it.  Otherwise, we need to
	 * make room in the cache and continue.
	 */
	curr = jarCache.files;
	prev = &jarCache.files;
	while( curr && !already_cached )
	{
		assert(curr != NULL);
		assert(curr->fileName != 0);
		
		/* Look for a matching JAR file */
		if( !strcmp(curr->fileName, jf->fileName) )
		{
			/* Names match, check the dates */
			if( curr->lastModified == jf->lastModified )
			{
				/*
				 * They're the same, unlink it from the cache
				 * so we can put it at the front later on.
				 */
				*prev = curr->next;
				retval = curr;
				retval->users++;

DBG(JARFILES,  dprintf("Found cached jar file %s, %d users\n", retval->fileName, retval->users); );

			}
			else
			{
				/*
				 * The modified time is different, purge our
				 * cached version and proclaim this one the
				 * canonical version.
				 *
				 * XXX Hmm...  Do we care how the two dates
				 * relate?  Since this is the one now being
				 * loaded from disk it "must" be the one
				 * the user wants.  Bleh, too bad the
				 * semantics don't seem to have been defined.
				 */
				*prev = curr->next;
				curr->flags &= ~JFF_CACHED;
				dead_jar = curr;

DBG(JARFILES,  dprintf("Cached jar file %s purged\n", curr->fileName); );

			}
			/*
			 * `jf' is redundant so the number of cached files
			 * isn't going to change.
			 */
			already_cached = 1;

			assert(retval->users >= 1);
		}
		else if( curr->users == 0 )
		{
			/*
			 * Record the least recently used file in case we need
			 * to eject someone.
			 */
			lru = prev;
		}
		prev = &curr->next;
		curr = curr->next;
	}
	if( !already_cached )
	{
		/*
		 * Cache the file if theres still room rather than ejecting
		 * the lru or if theres no room and no lru.
		 */
		if( (jarCache.count < JAR_FILE_CACHE_MAX) || !lru )
		{
			/* Adding a new cache node */
			jarCache.count++;
		}
		else
		{
			/*
			 * Theres an unused jarFile, unlink the least
			 * recently used one.
			 */
			dead_jar = *lru;
			*lru = dead_jar->next;
			dead_jar->flags &= ~JFF_CACHED;
		}
	}
	/* Put the file at the start of the cache */
	retval->next = jarCache.files;
	jarCache.files = retval;
	retval->flags |= JFF_CACHED;
	unlockStaticMutex(&jarCache.lock);
	/*
	 * Free the redundant/excess files outside of the lock.
	 *
	 * NOTE: The dead_jar test must come first since the file could
	 * already be cached and get superceded by a newer version of the
	 * file.
	 */
	if( dead_jar )
		collectJarFile(dead_jar);
	else if( already_cached )
	{
		assert(jf->users == 1);
		jf->users = 0;
		collectJarFile(jf);
	}

	assert(retval != 0);
	
	return( retval );
}

/*
 * Remove a JAR file from the cache.
 */
static void removeJarFile(jarFile *jf)
{
	jarFile *curr, **prev;

	assert(jf != NULL);

	/* Make sure its actually in the cache. */
	if( jf->flags & JFF_CACHED )
	{
		lockStaticMutex(&jarCache.lock);
		{
			curr = jarCache.files;
			prev = &jarCache.files;
			/* Find `jf' on the list and... */
			while( curr != jf )
			{
				assert(curr != NULL);
				
				prev = &curr->next;
				curr = curr->next;
			}
			/* unlink it */
			*prev = curr->next;
			jf->next = NULL;
			jf->flags &= ~JFF_CACHED;
			jarCache.count--;
		}
		unlockStaticMutex(&jarCache.lock);
	}
}

void flushJarCache(void)
{
	jarFile **prev, *curr, *next;

	lockStaticMutex(&jarCache.lock);
	curr = jarCache.files;
	prev = &jarCache.files;
	while( curr )
	{
		next = curr->next;
		if( curr->users == 0 )
		{
			*prev = next;
			curr->flags &= ~JFF_CACHED;
			collectJarFile(curr);
		}
		else
		{
			prev = &curr->next;
		}
		curr = next;
	}
	unlockStaticMutex(&jarCache.lock);
}

/*
 * Convenient read function that operates on regular or mmap'ed files.
 * This also takes an `instantiation' function which is used to convert
 * any data into the proper byte order and alignment.
 */
static int jarRead(jarFile *jf, uint8 *buf, size_t len,
		   int (*ins_func)(uint8 *dest, uint8 *src))
{
	int retval = -1;

	assert(jf != 0);
	assert(buf != 0);
	
#ifdef HAVE_MMAP
	if( jf->data != MAP_FAILED )
	{
		if((jf->offset + len) > jf->size )
		{
			jf->error = JAR_ERROR_TRUNCATED_FILE;
		}
		else if( ins_func )
		{
			/*
			 * We optimize this to use the instantiation function
			 * to do the copying instead of doing a memcpy and then
			 * moving stuff around.
			 */
			jf->offset += ins_func(buf, jf->data + jf->offset);
			retval = len;
		}
		else
		{
			/* Just copy the bits */
			memcpy(buf, jf->data + jf->offset, len);
			jf->offset += len;
			retval = len;
		}
	}
	else
#endif
	{
		size_t bytes_left;
		ssize_t bytes_read;
		int rc = 0;

		bytes_left = len;
		/* XXX is this loop necessary? */
		while( bytes_left &&
		       !(rc = KREAD(jf->fd,
				    &buf[len - bytes_left],
				    bytes_left,
				    &bytes_read)) &&
		       bytes_read )
		{
			bytes_left -= bytes_read;
		}
		if( rc )
		{
			jf->error = SYS_ERROR(rc);
		}
		else if( bytes_left )
		{
			jf->error = JAR_ERROR_TRUNCATED_FILE;
		}
		else
		{
			/* Instantiate the memory */
			if( ins_func )
				ins_func(buf, buf);
			retval = len;
		}
	}
	return( retval );
}

/*
 * Instantiate a structure that was encoded in a flat file.  We use a define
 * to make sure the instantiation function is inlined.
 */
#ifdef HAVE_MMAP
#define jarInstantiate(jf, buf, ins_func) \
{ \
	if( jf->data != MAP_FAILED ) \
	{ \
		jf->offset += ins_func(buf, jf->data + jf->offset); \
	} \
	else \
	{ \
		ins_func(buf, buf); \
	} \
}
#else
#define jarInstantiate(jf, buf, ins_func) \
{ \
	ins_func(buf, buf); \
}
#endif

/*
 * Convenient seek function that operates on regular or mmap'ed files.
 */
static 
off_t 
jarSeek(jarFile *jf, off_t offset, int whence)
{
	off_t retval = (off_t)-1;

	assert(jf != 0);
	
#ifdef HAVE_MMAP
	if( jf->data != (uint8*)-1 )
	{
		off_t pos = 0;
		
		switch( whence )
		{
		case SEEK_CUR:
			pos = jf->offset + offset;
			break;
		case SEEK_SET:
			pos = offset;
			break;
		case SEEK_END:
			pos = jf->size + offset;
			break;
		default:
			KAFFEVM_ABORT();
			break;
		}
		if( (pos >= 0) && ((unsigned)pos < jf->size) )
		{
			jf->offset = pos;
			retval = pos;
		}
	}
	else
#endif
	{
		off_t off;
		int rc;
		
		rc = KLSEEK(jf->fd, offset, whence, &off);
		if( rc )
			jf->error = SYS_ERROR(rc);
		else
			retval = off;
	}
	return( retval );
}

/* Macro used below to create a 32 bit value from a uint8 buffer */
#define copy32le(dest, buf, index) \
do { \
	register uint32 tmp; \
	\
	tmp = (((buf)[(index) + 3] << 24) | \
	       ((buf)[(index) + 2] << 16) | \
	       ((buf)[(index) + 1] << 8) | \
	       ((buf)[(index) + 0])); \
	dest = tmp; \
} while(0);

/* Macro used below to create a 16 bit value from a uint8 buffer */
#define copy16le(dest, buf, index) \
do { \
	register uint16 tmp; \
	\
	tmp = (((buf)[(index) + 1] << 8) | \
	       ((buf)[(index) + 0])); \
	dest = tmp; \
} while(0);

/*
 * Unfortunately, the data members of the zip headers aren't aligned properly
 * so we can't just blast the data into a structure.  So we have to use these
 * instantiation functions to copy the data and do any byte swapping.
 *
 * Note 1: These functions work from high to low memory so that we're
 * basically just shuffling the bits around and not using extra storage.
 *
 * Note 2: We're hoping that the compiler is smart enough to drop the unused
 * data members when inlining...
 */
static int
instantiateCentralDir(uint8 *dest, uint8 *buf)
{
	jarCentralDirectoryRecord *cdr = (jarCentralDirectoryRecord *)dest;

	assert(dest != 0);
	assert(buf != 0);
	
	copy32le(cdr->relativeLocalHeaderOffset, buf, 42);
	copy32le(cdr->externalFileAttribute, buf, 38); /* Not used */
	copy16le(cdr->internalFileAttribute, buf, 36); /* Not used */
	copy16le(cdr->diskNumberStart, buf, 34); /* Not used */
	copy16le(cdr->fileCommentLength, buf, 32);
	copy16le(cdr->extraFieldLength, buf, 30);
	copy16le(cdr->fileNameLength, buf, 28);
	copy32le(cdr->uncompressedSize, buf, 24);
	copy32le(cdr->compressedSize, buf, 20);
	copy32le(cdr->crc, buf, 16); /* Not used */
	copy16le(cdr->lastModifiedDate, buf, 14);
	copy16le(cdr->lastModifiedTime, buf, 12);
	copy32le(cdr->compressionMethod, buf, 10);
	copy16le(cdr->flags, buf, 8); /* Not used */
	copy16le(cdr->versionExtract, buf, 6); /* Not used */
	copy16le(cdr->versionMade, buf, 4); /* Not Used */
	return( FILE_SIZEOF_CENTRALDIR );
}

static int
instantiateLocalHeader(uint8 *dest, uint8 *buf)
{
	jarLocalHeader *lh = (jarLocalHeader *)dest;

	assert(dest != 0);
	assert(buf != 0);
	
	copy16le(lh->extraFieldLength, buf, 28);
	copy16le(lh->fileNameLength, buf, 26);
	copy32le(lh->uncompressedSize, buf, 22); /* Not used */
	copy32le(lh->compressedSize, buf, 18); /* Not used */
	copy32le(lh->crc, buf, 14); /* Not used */
	copy16le(lh->lastModifiedDate, buf, 12); /* Not used */
	copy16le(lh->lastModifiedTime, buf, 10); /* Not used */
	copy16le(lh->compressionMethod, buf, 8); /* Not used */
	copy16le(lh->flags, buf, 6); /* Not used */
	copy16le(lh->versionExtract, buf, 4); /* Not used */
	return( FILE_SIZEOF_LOCALHEADER );
}

static int
instantiateCentralDirEnd(uint8 *dest, uint8 *buf)
{
	jarCentralDirectoryEnd *cde = (jarCentralDirectoryEnd *)dest;

	assert(dest != 0);
	assert(buf != 0);
	
	copy16le(cde->commentLength, buf, 18); /* Not used */
	copy32le(cde->offsetOfDirectory, buf, 16);
	copy32le(cde->sizeOfDirectory, buf, 12); /* Not used */
	copy16le(cde->nrOfEntriesInDirectory, buf, 10);
	copy16le(cde->nrOfEntriesInThisDirectory, buf, 8); /* Not used */
	copy16le(cde->numberOfDiskWithDirectory, buf, 6); /* Not used */
	copy16le(cde->numberOfDisk, buf, 4); /* Not used */
	return( FILE_SIZEOF_CENTRALEND );
}

static inline int instantiateSignature(uint8 *dest, uint8 *buf)
{
	uint32 *sig = (uint32 *)dest;

	assert(dest != 0);
	assert(buf != 0);
	
	copy32le(sig[0], buf, 0);
	/*
	 * We're instantiating this structure in multiple stages so we don't
	 * move the file pointer.
	 */
	return( 0 );
}

/*
 * Read a JAR header from the file and check it's signature.
 */
static int readJarHeader(jarFile *jf, uint32 sig, void *buf, size_t len)
{
	int retval = 0;
	int ret;

	assert(jf != 0);
	assert((sig == CENTRAL_HEADER_SIGNATURE) ||
		(sig == LOCAL_HEADER_SIGNATURE) ||
		(sig == CENTRAL_END_SIGNATURE));
	assert(buf != 0);
	
	ret = jarRead(jf, buf, len, instantiateSignature);
	if (ret >= 0 && (unsigned)ret == len )
	{
		/* Check the signature */
		if( sig == ((uint32 *)buf)[0] )
		{
			retval = 1;
		}
		else
		{
			jf->error = JAR_ERROR_BAD_SIGNATURE;
		}
	}
	return( retval );
}

/*
 * Add a jarEntry to the given jarFile.
 */
static void addJarEntry(jarFile *jf, jarEntry *je)
{
	unsigned int hash;

	assert(jf != 0);
	assert(jf->table != 0);
	assert(je != 0);
	assert(je->fileName != 0);
	
	hash = hashName(je->fileName);
	hash = hash % jf->tableSize;
	je->next = jf->table[hash];
	jf->table[hash] = je;
}

/*
 * Create a jarEntry structure for the current central directory record in
 * the file.
 */
static int initJarEntry(jarFile *jf, jarEntry *je, char **name_strings)
{
	jarCentralDirectoryRecord cdr;
	int retval = 0;

	assert(jf != 0);
	assert(je != 0);
	assert(name_strings != 0);
	assert(*name_strings != 0);

	/* Read the header */
	if( readJarHeader(jf, CENTRAL_HEADER_SIGNATURE, &cdr,
			  FILE_SIZEOF_CENTRALDIR) )
	{
		int read_size;

		/* Instantiate the header */
		jarInstantiate(jf, (uint8 *)&cdr, instantiateCentralDir);
		je->next = NULL;
		/* Allocate space for our name */
		(*name_strings) -= cdr.fileNameLength + 1;
		je->fileName = *name_strings;
		/* Copy whatever we care about from the file record */
		je->dosTime = (cdr.lastModifiedDate << 16) |
			cdr.lastModifiedTime;
		je->localHeaderOffset = cdr.relativeLocalHeaderOffset;
		je->uncompressedSize = cdr.uncompressedSize;
		je->compressedSize = cdr.compressedSize;
		je->compressionMethod = cdr.compressionMethod;
		/* Read the file name */
		if( (read_size = jarRead(jf,
					 (uint8*) je->fileName,
					 cdr.fileNameLength,
					 NULL)) >= 0 )
		{
			/* Make sure its terminated */
			je->fileName[cdr.fileNameLength] = 0;
			/*
			 * Skip the extra field and comment, we should
			 * now be positioned at the next directory
			 * record, if there is one.
			 */
			if( jarSeek(jf,
				    (off_t)cdr.extraFieldLength +
				    cdr.fileCommentLength,
				    SEEK_CUR) > 0 )
			{
				retval = 1;

				assert(strlen(je->fileName) ==
				       cdr.fileNameLength);
			}
		}

	}
	else
	{
		jf->error = JAR_ERROR_BAD_CENTRAL_RECORD_SIGNATURE;
	}
	return( retval );
}

/*
 * Find the central directory end, find out the number of central directory
 * entries and seek to the start of them.
 */
static int getCentralDirCount(jarFile *jf, unsigned int *out_dir_size)
{
	int pos, retval = -1;

	assert(jf != 0);
	assert(out_dir_size != 0);
	
	/* The central directory end is at the end of the file */
	if( (pos = jarSeek(jf, (off_t)-FILE_SIZEOF_CENTRALEND, SEEK_END)) > 0 )
	{
		jarCentralDirectoryEnd cde;

		if( readJarHeader(jf, CENTRAL_END_SIGNATURE,
				  &cde, FILE_SIZEOF_CENTRALEND) )
		{
			jarInstantiate(jf, (uint8 *)&cde,
				       instantiateCentralDirEnd);
			if( cde.nrOfEntriesInDirectory >
			    (cde.sizeOfDirectory / FILE_SIZEOF_CENTRALDIR) )
			{
				jf->error = JAR_ERROR_ENTRY_COUNT_MISMATCH;
			}
			else if( cde.sizeOfDirectory > (unsigned)pos )
			{
				jf->error = JAR_ERROR_IMPOSSIBLY_LARGE_DIRECTORY;
			}
			else if( jarSeek(jf,
					 (off_t)cde.offsetOfDirectory,
					 SEEK_SET) >= 0 )
			{
				*out_dir_size = cde.sizeOfDirectory;
				retval = cde.nrOfEntriesInDirectory;
			}
		}
		else
		{
			jf->error = JAR_ERROR_NO_END;
		}
	}
	return( retval );
}

/*
 * Read the central directory records from the file
 */
static int readJarEntries(jarFile *jf)
{
	unsigned int dir_size = 0;
	int retval = 0;

	assert(jf != 0);
	
	if( (jf->count = getCentralDirCount(jf, &dir_size)) >= 0 )
	{
		unsigned int table_size;

		/*
		 * Compute a sensible size for the hash table base on the
		 * number of entries in the jar.
		 */
		jf->tableSize = (jf->count + 3) / 4;
		/*
		 * We want a single block of memory for all jarEntry's and
		 * their names.
		 */
		/* Hash table buckets */
		table_size = sizeof(jarEntry *) * jf->tableSize;
		/* Add the total size of the central directory records */
		table_size += dir_size;
		/* ... however, we don't read the whole central dir struct */
		table_size -= jf->count * FILE_SIZEOF_CENTRALDIR;
		/* ... we use jarEntry's */
		table_size += jf->count * sizeof(jarEntry);
		/* ... with NULL terminated name strings. */
		table_size += jf->count * 1;
		if( (jf->table = gc_malloc(table_size, KGC_ALLOC_JAR)) )
		{
			char *name_strings;
			jarEntry *je;
			int lpc;

			/* jarEntry's are right after the table */
			je = (jarEntry *)(jf->table + jf->tableSize);
			/* names are at the end of the memory block */
			name_strings = ((char *)jf->table) + table_size;
			retval = 1;
			/*
			 * Read in the central directory records and add them
			 * to the hash table.
			 */
			for( lpc = 0; (lpc < jf->count) && retval; lpc++ )
			{
				if( initJarEntry(jf, je, &name_strings) )
				{
					addJarEntry(jf, je);
					je++;
				}
				else
				{
					retval = 0;
				}
			}
			
		}
		else
		{
			jf->error = JAR_ERROR_OUT_OF_MEMORY;
		}
	}
	else if( jf->error )
	{
	}
	else
	{
		retval = 1;
	}
	return( retval );
}

/*
 * Simple function to handle uncompressing the file data.
 */
static uint8 *inflateJarData(jarFile *jf, jarEntry *je,
			     jarLocalHeader *lh, uint8 *buf)
{
	uint8 *retval = NULL;

	assert(jf != NULL);
	assert(je != NULL);
	assert(lh != NULL);
	assert(buf != NULL);
	
	switch( je->compressionMethod )
	{
	case COMPRESSION_STORED:
		retval = buf;
		break;
	case COMPRESSION_DEFLATED:
		if( je->uncompressedSize == 0 )
		{
			/* XXX What to do? */
			retval = gc_malloc(8, KGC_ALLOC_JAR);
		}
		else if( (retval = gc_malloc(je->uncompressedSize,
					     KGC_ALLOC_JAR)) )
		{
			if( inflate_oneshot(buf,
					    (int)je->compressedSize,
					    retval,
					    (int)je->uncompressedSize) == 0 )
			{
				addToCounter(&jarmem, "vmmem-jar files",
					     1, GCSIZEOF(retval));

			}
			else
			{
				jf->error = JAR_ERROR_DECOMPRESSION_FAILED;
				gc_free(retval);
				retval = NULL;
			}
		}
		else
		{
			jf->error = JAR_ERROR_OUT_OF_MEMORY;
		}
		gc_free(buf);
		break;
	default:
		jf->error = JAR_ERROR_UNSUPPORTED_COMPRESSION;
		gc_free(buf);
		break;
	}
	return( retval );
}

uint8 *getDataJarFile(jarFile *jf, jarEntry *je)
{
	uint8 *buf = NULL, *retval = NULL;
	jarLocalHeader lh;

	assert(jf != 0);
	assert(je != 0);

	lockStaticMutex(&jf->lock);
	/* Move to the local header in the file and read it. */
	if( !jf->error &&
	    (jarSeek(jf, (off_t)je->localHeaderOffset, SEEK_SET) >= 0) &&
	    readJarHeader(jf, LOCAL_HEADER_SIGNATURE, &lh,
			  FILE_SIZEOF_LOCALHEADER) )
	{
		jarInstantiate(jf, (uint8 *)&lh, instantiateLocalHeader);
		/* Skip the local file name and extra fields */
		jarSeek(jf,
			(off_t)lh.fileNameLength + lh.extraFieldLength,
			SEEK_CUR);
		/* Allocate some memory and read in the file data */
		if( (buf = (uint8 *)gc_malloc(je->compressedSize,
					      KGC_ALLOC_JAR)) )
		{
			if( jarRead(jf, buf, je->compressedSize, NULL) < 0 )
			{
				gc_free(buf);
				buf = NULL;
				jf->error = JAR_ERROR_IO;
			}
		}
		else
		{
			jf->error = JAR_ERROR_OUT_OF_MEMORY;
		}
	}

	unlockStaticMutex(&jf->lock);
	if( buf )
	{
	        /* Try to decompress it */
	        retval = inflateJarData(jf, je, &lh, buf);
	}
	return( retval );
}

/*
 * If a jarFile is cached it may be closed before it is used again so we might
 * have to reopen the file.
 */
static jarFile *delayedOpenJarFile(jarFile *jf)
{
	jarFile *retval = NULL;
	int fd, rc;

	assert(jf != NULL);

	/* Open the file and check for a different modified time */
	if( !(rc = KOPEN(jf->fileName, O_RDONLY|O_BINARY, 0, &fd)) )
	{
#if !defined(STATIC_JAR_FILES)
		struct stat jar_stat;

		if( !(rc = KFSTAT(fd, &jar_stat)) )
		{
			if( jar_stat.st_mtime == jf->lastModified )
			{
#endif /* !defined(STATIC_JAR_FILES) */
				
				/* Only set the fd in the structure here */
				lockStaticMutex(&jf->lock);
				if( jf->fd == -1 )
					jf->fd = fd; /* We're first */
				else
					KCLOSE(fd); /* Someone else set it */
				unlockStaticMutex(&jf->lock);
				retval = jf;
#if !defined(STATIC_JAR_FILES)
			}
			else
			{
				KCLOSE(fd);
				/* The file was changed, reopen it. */
				removeJarFile(jf);
				retval = openJarFile(jf->fileName);
			}
		}
		else
		{
			KCLOSE(fd);
		}
#endif /* !defined(STATIC_JAR_FILES) */
	}
	/*
	 * The file either disappeared, or a new one was made, get rid of the
	 * old one.
	 */
	if( retval != jf )
	{
		removeJarFile(jf);
		closeJarFile(jf);
	}
	return( retval );
}

jarFile *openJarFile(char *name)
{
	jarFile *retval = NULL;

	assert(name != NULL);
	
	/* Look for it in the cache first */
	if( (retval = findCachedJarFile(name)) )
	{
		/* Check if we need to reopen the file */
		if( (retval->fd == -1)
#ifdef HAVE_MMAP
		    && (retval->data == MAP_FAILED)
#endif
		    )
		{
			retval = delayedOpenJarFile(retval);
		}
	}
	/*
	 * If a cached file wasn't found or its broken then try to make a new
	 * one.
	 */
	if( !retval &&
	    (retval = (jarFile *)gc_malloc(sizeof(jarFile) +
					   strlen(name) +
					   1,
					   KGC_ALLOC_JAR)) )
	{
		int rc, error = 0;

		retval->fileName = (char *)(retval + 1);
		strcpy(retval->fileName, name);
		retval->users = 1;
		retval->lastModified = 0;
		retval->count = 0;
		retval->error = NULL;
		retval->fd = -1;
		retval->table = NULL;
		retval->tableSize = 0;
		initStaticLock(&retval->lock);
#ifdef HAVE_MMAP
 		retval->data = MAP_FAILED;
#endif /* HAVE_MMAP */
		if( (rc = KOPEN(name, O_RDONLY|O_BINARY, 0, &retval->fd)) )
		{
			/* Error opening the file */
			error = 1;
		}
		else
		{
			struct stat jar_stat;
			
			if( KFSTAT(retval->fd, &jar_stat) == 0 )
 			{
				if( (jar_stat.st_mode & S_IFDIR) )
				{
					/* Its a directory! */
					error = 1;
				}
				else
				{
					retval->lastModified =
						jar_stat.st_mtime;
#ifdef HAVE_MMAP
					/*
					 * Setup the mmap members in the
					 * jarFile structure
					 */
					retval->size = jar_stat.st_size;
					retval->data = mmap(NULL,
							    retval->size,
							    PROT_READ,
							    MAP_SHARED,
							    retval->fd,
							    (off_t)0);
					if( retval->data == MAP_FAILED )
					{
						/*
						 * Unsuccessful mmap, fallback
						 * to the FD for now.
						 */
					}
					else
					{
						/*
						 * Successful mmap, close the
						 * FD
						 */
						KCLOSE(retval->fd);
						retval->fd = -1;
						retval->offset = 0;
					}
#endif
				}
			}
			else
			{
				error = 1;
			}
			/*
			 * If everythings a go, figure out how many directory
			 * entries there are and read them in.
			 */
			if( !error && !readJarEntries(retval) )
			{
				error = 1;
			}
		}
		if( !error )
		{
			addToCounter(&jarmem, "vmmem-jar files",
				     1, GCSIZEOF(retval));
			if( retval->table ) {
				addToCounter(&jarmem, "vmmem-jar files",
					     1, GCSIZEOF(retval->table));
			}
			/*
			 * No errors, so we cache the file.  If someone else
			 * beat us to the cache we'll use theirs instead.
			 */
			retval = cacheJarFile(retval);
		}
		else
		{
			/* Something went wrong, cleanup our mess */
			retval->users = 0;
			collectJarFile(retval);
			retval = NULL;
		}
	}

	return( retval );
}

void closeJarFile(jarFile *jf)
{
	if( jf )
	{

		lockStaticMutex(&jarCache.lock);
		jf->users--;
DBG(JARFILES,  dprintf("Closing jar file %s, users %d\n", jf->fileName, jf->users); );

		if( jf->users == 0 )
		{
			if( jarCache.count <= JAR_FILE_CACHE_MAX )
			{
				/*
				 * The cache isn't full so if its in there
				 * we can leave it.
				 */
#ifdef HAVE_MMAP
				if( jf->data != MAP_FAILED )
				{
					/*
					 * Unmap the object and force FD I/O
					 * from now on.
					 */
					munmap(jf->data, jf->size);
					jf->data = MAP_FAILED;
				}
				else
#endif
				{
					KCLOSE(jf->fd);
				}
				jf->fd = -1;
			}
			else
			{
				/* Too many files in the cache, remove it */
				removeJarFile(jf);
			}
			if( !(jf->flags & JFF_CACHED) )
			{
				/*
				 * Not cached anymore and this was the last
				 * reference so collect the file.
				 */
				collectJarFile(jf);
			}
		}
		unlockStaticMutex(&jarCache.lock);
	}
}

jarEntry *lookupJarFile(jarFile *jf, const char *entry_name)
{
	jarEntry *retval = NULL;

	assert(jf != NULL);
	assert(entry_name != NULL);
	
	/*
	 * No need to visit the kernel here since we're just walking the
	 * structure.
	 */
	if( jf->tableSize )
	{
		unsigned int hash;
		jarEntry *je;

		hash = hashName(entry_name);
		hash = hash % jf->tableSize;
		je = jf->table[hash];
		while( je && !retval )
		{
			if( !strcmp(je->fileName, entry_name) )
				retval = je;
			je = je->next;
		}
	}
	return( retval );
}

void KaffeVM_initJarCache()
{
  initStaticLock(&jarCache.lock);
}
