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

#define	readu1(c, f)	((*(c)) = getc(f))
#define	readu2(c, f)	((*(c)) = getc(f) << 8, (*c) |= getc(f))
#define	readu4(c, f)	((*(c)) = getc(f) << 24, (*c) |= getc(f) << 16, (*c) |= getc(f) << 8, (*c) |= getc(f))

#endif
