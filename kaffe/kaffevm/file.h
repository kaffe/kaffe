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

#ifndef __file_h
#define __file_h

typedef struct _classFile {
	unsigned char*	base;
	unsigned char*	buf;
	int		size;
	int		type;
} classFile;

#define	readu1(c,f)	(*(c) = f->buf[0], f->buf += 1)
#define	readu2(c,f)	(*(c) = (f->buf[0] << 8) | f->buf[1], f->buf += 2)
#define	readu4(c,f)	(*(c) = (f->buf[0] << 24)|(f->buf[1] << 16)|\
				(f->buf[2] << 8)|f->buf[3], f->buf += 4)

#define	readm(b,l,s,f)	(memcpy(b, f->buf, (l)*(s)), f->buf += (l)*(s))
#define	seekm(f,l)	(f->buf += (l))

#endif
