/*
 * file.h
 * File support routines.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef kaffe_kaffevm_file_h
#define kaffe_kaffevm_file_h

/*
 * Used in kaffeh and kaffevm
 *
 * XXX rename to classFileHandle.h ?
 */
#include "config.h"
#include "config-std.h"
#include "gtypes.h"
#include "errors.h"

typedef enum ClassFileType {
	CP_INVALID  = 0,
	CP_ZIPFILE  = 1,
	CP_DIR      = 2,
	CP_SOFILE   = 3,
	CP_BYTEARRAY= 4
} ClassFileType;
 
typedef struct classFile {
	const unsigned char* base;
	const unsigned char* cur;
	size_t	        size;
	ClassFileType	type;
} classFile;

/*
 * Init a a classFile struct to point to the given buffer.
 */
static inline void classFileInit(classFile* cf,
				 const unsigned char* buf,
				 size_t len,
				 ClassFileType cft) __UNUSED__;
/*
 * Check that the needed number of bytes are available.  If
 * not a ClassFormatError is posted in einfo.
 */
static inline bool checkBufSize(classFile* cf, u4 need,
				const char* cfname,
				errorInfo* einfo) __UNUSED__;
/* Read a single unsigned byte from cf */
static inline void readu1(u1* c, classFile* cf) __UNUSED__;
/* Read a pair of unsigned bytes from cf */
static inline void readu2(u2* c, classFile* cf) __UNUSED__;
/* Read a four-byte unsigned word from cf */
static inline void readu4(u4* c, classFile* cf) __UNUSED__;
/* Read an arbitrary chunk from cf */
static inline void readm(void* dest, size_t len,
			 size_t size, classFile* cf) __UNUSED__;
/* Skip over an arbitrary chunk of cf */
static inline void seekm(classFile*cf, size_t len) __UNUSED__;

static inline void 
classFileInit(classFile* cf, const unsigned char* buf, size_t len, ClassFileType cft)
{
	assert (((buf == NULL) && (len == 0))
		 || ((buf != NULL) && (len != 0)));

	cf->base = cf->cur = buf;
	cf->size = len;
	cf->type = cft;
}

static inline bool 
checkBufSize(classFile* cf, u4 need, const char* cfname, errorInfo* einfo)
{
	assert(cf != NULL);
	assert(cf->type != CP_INVALID);
	
	if ((unsigned)(cf->base + cf->size - cf->cur) < need)
	{
		if (cfname != NULL)
			postExceptionMessage(einfo,
					     JAVA_LANG(ClassFormatError),
					     "%s class file truncated",
					     cfname);
		else
			postExceptionMessage(einfo,
					     JAVA_LANG(ClassFormatError),
					     "Truncated class file");
			
		return false;
	}

	return true;
}

static inline void 
readu1(u1* c, classFile* cf)
{
	assert(c != NULL);
	assert(cf != NULL);
	assert(cf->type != CP_INVALID);

	*c = cf->cur[0];
	cf->cur += 1;
}

static inline void 
readu2(u2* c, classFile* cf)
{
	assert(c != NULL);
	assert(cf != NULL);
	assert(cf->type != CP_INVALID);

	*c = (((u2) cf->cur[0]) << 8) | ((u2) cf->cur[1]);
	cf->cur += 2;
}

static inline void 
readu4(u4* c, classFile* cf)
{
	assert(c != NULL);
	assert(cf != NULL);
	assert(cf->type != CP_INVALID);

	*c = (((u4) cf->cur[0]) << 24)
		| (((u4) cf->cur[1]) << 16)
		| (((u4) cf->cur[2]) << 8)
		| ((u4) cf->cur[3]);
	cf->cur += 4;
}

/**
 * Read len*size bytes out of the classfile, and into dest.
 */
static inline void
readm(void* dest, size_t len, size_t size, classFile* cf)
{
	assert(dest != NULL);
	assert(cf != NULL);
	assert(cf->type != CP_INVALID);

	memcpy(dest, cf->cur, len*size);
	cf->cur += len*size;
}

static inline void
seekm(classFile* cf, size_t len)
{
	assert(cf != NULL);
	assert(cf->type != CP_INVALID);

	cf->cur += len;
}

#endif /* kaffe_kaffevm_file_h */
