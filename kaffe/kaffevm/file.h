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
	unsigned char* mem;
	const unsigned char* base;
	const unsigned char* cur;
	size_t	        size;
	ClassFileType	type;
} classFile;

/*
 * Init a a classFile struct to point to the given buffer.
 */
extern void classFileInit(classFile* cf,
			  unsigned char* mem,
			  const unsigned char* buf,
			  size_t len,
			  ClassFileType cft);
/*
 * Check that the needed number of bytes are available.  If
 * not a ClassFormatError is posted in einfo.
 */
extern bool checkBufSize(classFile* cf, 
			 u4 need,
			 const char* cfname,
			 errorInfo* einfo);

/* Read a single unsigned byte from cf */
extern void readu1(u1* c, 
		   classFile* cf);

/* Read a pair of unsigned bytes from cf */
extern void readu2(u2* c, 
		   classFile* cf);

/* Read a four-byte unsigned word from cf */
extern void readu4(u4* c,
		   classFile* cf);

/* Read an arbitrary chunk from cf */
extern void readm(void* dest,
		  size_t len,
		  size_t size,
		  classFile* cf);

/* Skip over an arbitrary chunk of cf */
extern void seekm(classFile*cf,
		  size_t len);

#endif /* kaffe_kaffevm_file_h */
