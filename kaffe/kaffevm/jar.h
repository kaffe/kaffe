/*
 * jar.h
 * Handle JAR input files.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __jar_h
#define __jar_h

/*
 * JAR files are like ZIP files.
 * Format:
 *   { [ jarLocalHeader + filename + extraField + fileData + jarDataDescriptor
 *	   ..... } + jarCentralDirectory ]
 */

/*
 * Central directory header.
 */
typedef struct _jarCentralDirectoryRecord {

	uint32		signature;
	uint16		versionMade;
	uint16		versionExtract;
	uint16		flags;
	uint16		compressionMethod;
	uint16		lastModifiedTime;
	uint16		lastModifiedDate;
	uint32		crc;
	uint32		compressedSize;
	uint32		uncompressedSize;
	uint16		fileNameLength;
	uint16		extraFieldLength;
	uint16		fileCommentLength;
	uint16		diskNumberStart;
	uint16		internalFileAttribute;
	uint32		externalFileAttribute;
	uint32		relativeLocalHeaderOffset;

} jarCentralDirectoryRecord;

/*
 * Central directory end record.
 */
typedef struct _jarCentralDirectoryEnd {

	uint32	signature;
	uint16	numberOfDisk;
	uint16	numberOfDiskWithDirectory;
	uint16	nrOfEntriesInThisDirectory;
	uint16	nrOfEntriesInDirectory;
	uint32	sizeOfDirectory;
	uint32	offsetOfDirectory;
	uint16	commentLength;

} jarCentralDirectoryEnd;

/*
 * JAR file record.
 */
typedef struct _jarEntry {

	struct _jarEntry*	next;
	char*			fileName;
	long			dataPos;
	uint16			compressionMethod;
	uint32			compressedSize;
	uint32			uncompressedSize;

} jarEntry;

typedef struct _jarFile {

	int			fp;
	int			count;
	jarEntry*		head;
	char*			error;

} jarFile;

/*
 * Various signatures.
 */
#define	LOCALHEADERSIGNATURE	0x04034b50
#define	CENTRALHEADERSIGNATURE	0x02014b50
#define	CENTRALENDSIGNATURE	0x06054b50

/*
 * Sizes of infile headers.
 */
#define	SIZEOFLOCALHEADER	30
#define	SIZEOFCENTRALEND	22

/*
 * Compression methods.
 */
#define COMPRESSION_STORED	0
#define COMPRESSION_SHRUNK	1
#define COMPRESSION_REDUCED1	2
#define COMPRESSION_REDUCED2	3
#define COMPRESSION_REDUCED3	4
#define COMPRESSION_REDUCED4	5
#define COMPRESSION_IMPLODED	6
#define COMPRESSION_TOKENIZED	7
#define COMPRESSION_DEFLATED	8

/*
 * Macros to read little-endian values.
 */
#define	INITREADS()		int rtmp; unsigned char rbuf[4]
#define	READ8(F)		(rtmp = read(F, rbuf, 1), 		    \
				 rtmp < 1 ? EOF : (unsigned)rbuf[0])

#define	READ16(F)		(rtmp = read(F, rbuf, 2), 		    \
				 rtmp < 2 ? EOF : (unsigned)rbuf[0] | 	    \
						 ((unsigned)rbuf[1]) << 8)

#define	READ32(F)		(rtmp = read(F, rbuf, 4), 		    \
				 rtmp < 4 ? EOF : (unsigned)rbuf[0] | 	    \
						 ((unsigned)rbuf[1]) << 8 |\
						 ((unsigned)rbuf[2]) << 16|\
						 ((unsigned)rbuf[3]) << 24)

#define	READBYTES(F,S,B)	read(F, B, S)
#define	SKIPBYTES(F,S)		lseek((F), (long)(S), SEEK_CUR)

/*
 * Interface.
 */
jarFile*	openJarFile(char*);
void		closeJarFile(jarFile*);
jarEntry*	lookupJarFile(jarFile*, char*);
uint8*		getDataJarFile(jarFile*, jarEntry*);

#endif
