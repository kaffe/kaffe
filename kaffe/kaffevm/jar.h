/*
 * jar.h
 * Handle JAR input files.
 *
 * Copyright (c) 2000, 2001, 2002 The University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.  
 * See the file "license.terms" for restrictions on redistribution 
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 */

#ifndef __jar_h
#define __jar_h

#include <sys/types.h>
#if !defined(KAFFEH)
#include "locks.h"
#endif

/*
 * JAR files are like ZIP files, they're basically a series of records
 * corresponding to the archived files.  The root record is the
 * jarCentralDirectoryEnd structure which is stored at the end of the file.
 * From there you can find the jarCentralDirectoryRecords which identify each
 * archived file.  Finally, the central directory records point to the local
 * headers which contain the actual compressed data.
 *
 *  { JAR local header +
 *    file name +
 *    extra fields +
 *    file data }* +
 *  { central directory record +
 *    file name +
 *    extra fields +
 *    file comment }* +
 *  central directory end
 */

/*
 * Central directory header, tells us about an archived file/dir and where
 * it is in the file.
 */
typedef struct _jarCentralDirectoryRecord {

	/* File offset | type | name */
#define	CENTRAL_HEADER_SIGNATURE	0x02014b50
	/* 00 */ uint32		signature; /* === CENTRAL_HEADER_SIGNATURE */
	/* 04 */ uint16		versionMade;
	/* 06 */ uint16		versionExtract;
	/* 08 */ uint16		flags;
	/* 10 */ uint16		compressionMethod;
	/* 12 */ uint16		lastModifiedTime;
	/* 14 */ uint16		lastModifiedDate;
	/* 16 */ uint32		crc;
	/* 20 */ uint32		compressedSize;
	/* 24 */ uint32		uncompressedSize;
	/* 28 */ uint16		fileNameLength;
	/* 30 */ uint16		extraFieldLength;
	/* 32 */ uint16		fileCommentLength;
	/* 34 */ uint16		diskNumberStart;
	/* 36 */ uint16		internalFileAttribute;
	/* 38 */ uint32		externalFileAttribute;
	/* 42 */ uint32		relativeLocalHeaderOffset;

} jarCentralDirectoryRecord;

#define FILE_SIZEOF_CENTRALDIR	46 /* Size of central dir in the file */

/*
 * Local header, header for the compressed data chunk of a file.
 */
typedef struct _jarLocalHeader {

	/* File offset | type | name */
#define	LOCAL_HEADER_SIGNATURE		0x04034b50
	/* 00 */ uint32		signature; /* === LOCAL_HEADER_SIGNATURE */
	/* 04 */ uint16		versionExtract;
	/* 06 */ uint16		flags;
	/* 08 */ uint16		compressionMethod;
	/* 10 */ uint16		lastModifiedTime;
	/* 12 */ uint16		lastModifiedDate;
	/* 14 */ uint32		crc;
	/* 18 */ uint32		compressedSize;
	/* 22 */ uint32		uncompressedSize;
	/* 26 */ uint16		fileNameLength;
	/* 28 */ uint16		extraFieldLength;

} jarLocalHeader;

#define	FILE_SIZEOF_LOCALHEADER	30 /* Size of local header in the file */

/*
 * Central directory end record, the root record that tells us where the
 * central directory records are located.
 */
typedef struct _jarCentralDirectoryEnd {

	/* File offset | type | name */
#define	CENTRAL_END_SIGNATURE		0x06054b50
	/* 00 */ uint32		signature; /* === CENTRAL_END_SIGNATURE */
	/* 04 */ uint16		numberOfDisk;
	/* 06 */ uint16		numberOfDiskWithDirectory;
	/* 08 */ uint16		nrOfEntriesInThisDirectory;
	/* 10 */ uint16		nrOfEntriesInDirectory;
	/* 12 */ uint32		sizeOfDirectory;
	/* 16 */ uint32		offsetOfDirectory;
	/* 18 */ uint16		commentLength;

} jarCentralDirectoryEnd;

#define	FILE_SIZEOF_CENTRALEND	22 /* Size of central end in the file */

/*
 * Internal structure for representing files in the JAR.  These are kept in
 * a hash table in the jarFile structure.
 *
 * `next' - Link to the next entry in the hash table.
 * `fileName' - The archived file name.
 * `dosTime' - Stored for the sake of the java code, we don't use it.
 * `uncompressedSize' - Cached value of the uncompressed size of the file.
 * `compressedSize' - Cached value of the compressed size of the file.
 * `compressionMethod' - The compression method used on the file.
 * `localHeaderOffset' - The location of the local header for the file and
 *   the file which follows.
 */
typedef struct _jarEntry {

	struct _jarEntry*	next;
	char*		        fileName;
	uint32			dosTime;
	uint32			uncompressedSize;
	uint32			compressedSize;
	uint16			compressionMethod;
	unsigned long		localHeaderOffset;

} jarEntry;

/*
 * A jarFile structure is used to access the contents of a JAR file on disk.
 * The structure is created on the first open and then possibly cached in
 * memory for future use.
 *
 * `next' - Link to the next jarFile held in the cache.
 * `lock' - A lock for serializing access to the file descriptor.
 * `fileName' - The file name of the jar.
 * `flags' - Holds various flag bits for the jar.
 * `users' - How many times this file has been opened for use.  This is
 *   protected by the lock that also covers the internal cache.
 * `lastModified' - The last time the file was modified, this is only needed
 *   to figure out if we need to invalidate cached files.
 * `fd' - The file descriptor thats used to refer to the file.  It is -1 if
 *   the file is closed.
 * `count' - The number of directory entries in the jar.
 * `table' - The hash table array used to reference jarEntry's.  This is
 *   actually a large block of memory which also contains the jarEntry's and
 *   their file names immediately following the table array.
 * `tableSize' - The size of the `table' array.
 * `error' - Holds a string indicating why an operation failed.  XXX This isn't
 *   even remotely thread safe.
 * `data' - The location of the mmap'ed jar file.
 * `size' - The size of the `data' array.
 * `offset' - The current position in the `data' array.
 */

enum {
	/* Indicates whether or not the file is in the cache. */
	JFB_CACHED
};

enum {
	JFF_CACHED = (1L << JFB_CACHED)
};

typedef struct _jarFile {

	struct _jarFile*	next;
#if !defined(KAFFEH)
	iStaticLock		lock;
#endif
	char*			fileName;
	unsigned long		flags;
	unsigned int		users;
	time_t			lastModified;
	int			fd;
	int			count;
	jarEntry**		table;
	unsigned int		tableSize;
	const char*		error;

	/* Extra information needed when the file is mmap'ed */
#ifdef HAVE_MMAP
	uint8*			data;
	size_t			size;
	off_t			offset;
#endif

} jarFile;

/*
 * Compression methods possible for archived files.
 */
enum {
	COMPRESSION_STORED,	/* Not compressed, just directly stored */
	COMPRESSION_SHRUNK,	/* XXX Not supported by zlib */
	COMPRESSION_REDUCED1,	/* XXX Not supported by zlib */
	COMPRESSION_REDUCED2,	/* XXX Not supported by zlib */
	COMPRESSION_REDUCED3,	/* XXX Not supported by zlib */
	COMPRESSION_REDUCED4,	/* XXX Not supported by zlib */
	COMPRESSION_IMPLODED,	/* XXX Not supported by zlib */
	COMPRESSION_TOKENIZED,	/* XXX Not supported by zlib */
	COMPRESSION_DEFLATED	/* zlib compressed */
};

/*
 * Attempt to open the given JAR file.  If this file has already been opened
 * and is stored in the cache then its reference count is incremented and
 * that structure is returned.  Otherwise, the file is opened and the
 * directory tree is cached in the new jarFile structure.
 *
 * `filename' - The full path to JAR file on the file system.
 * returns - An initialized jarFile structure or NULL if something went
 *   wrong.
 */
jarFile *openJarFile(char *filename);
/*
 * Close a jarFile structure that was created with openJarFile.  Basically,
 * this will just decrement the jarFiles reference count and release it
 * if it reaches zero.
 *
 * `jf' - The jarFile structure as returned by openJarFile.
 */
void closeJarFile(jarFile *jf);
/*
 * Lookup a file stored in the given jarFile.
 *
 * `jf' - The jarFile in which to search for the given file.
 * `entry_name' - The name of the file to search for in the JAR.
 */
jarEntry *lookupJarFile(jarFile *jf, const char *entry_name);
/*
 * Read in and, if necessary, decompress the contents of a file stored in a
 * JAR file.
 *
 * `jf' - The JAR file that contains the file.
 * `je' - The file to load from the JAR file.  NOTE: This can be a copy of the
 *   jarEntry object returned from lookupJarFile.
 * returns - The contents of the file or NULL if there was an error.
 */
uint8 *getDataJarFile(jarFile *jf, jarEntry *je);
/*
 * Flush the internal cache of JAR files.
 */
void flushJarCache(void);

void KaffeVM_initJarCache(void);

#endif
