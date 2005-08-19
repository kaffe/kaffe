/*
 * file.c
 * File support routines.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004, 2005
 * 	Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

/*
 * Used in kaffeh and kaffevm
 *
 * XXX rename to classFileHandle.h ?
 */
#include "config.h"
#include "config-std.h"

#ifdef HAVE_STRING_H
#include <string.h>
#endif

#include "gtypes.h"
#include "errors.h"
#include "file.h"

/*
 * Init a a classFile struct to point to the given buffer.
 */
void 
classFileInit(classFile* cf,
	      unsigned char* mem,
	      const unsigned char* buf,
	      size_t len,
	      ClassFileType cft)
{
	assert (((buf == NULL) && (len == 0))
		 || ((buf != NULL) && (len != 0)));

	cf->mem = mem;
	cf->base = cf->cur = buf;
	cf->size = len;
	cf->type = cft;
}

/*
 * Check that the needed number of bytes are available.  If
 * not a ClassFormatError is posted in einfo.
 */
bool 
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

/* Read a single unsigned byte from cf */
void 
readu1(u1* c, classFile* cf)
{
	assert(c != NULL);
	assert(cf != NULL);
	assert(cf->type != CP_INVALID);

	*c = cf->cur[0];
	cf->cur += 1;
}

/* Read a pair of unsigned bytes from cf */
void 
readu2(u2* c, classFile* cf)
{
	assert(c != NULL);
	assert(cf != NULL);
	assert(cf->type != CP_INVALID);

	*c = (((u2) cf->cur[0]) << 8) | ((u2) cf->cur[1]);
	cf->cur += 2;
}

/* Read a four-byte unsigned word of unsigned bytes from cf */
void 
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
void
readm(void* dest, size_t len, size_t size, classFile* cf)
{
	assert(dest != NULL);
	assert(cf != NULL);
	assert(cf->type != CP_INVALID);

	memcpy(dest, cf->cur, len*size);
	cf->cur += len*size;
}

/* Skip over an arbitrary chunk of cf */
void
seekm(classFile* cf, size_t len)
{
	assert(cf != NULL);
	assert(cf->type != CP_INVALID);

	cf->cur += len;
}
