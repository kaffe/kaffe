/*
 * constants.h
 * Manage constants.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __constant_h
#define __constant_h

#include "gtypes.h"

/*
 * Constant pool definitions.
 */
#define	CONSTANT_Class			7
#define	CONSTANT_Fieldref		9
#define	CONSTANT_Methodref		10
#define	CONSTANT_InterfaceMethodref	11
#define	CONSTANT_String			8
#define	CONSTANT_Integer		3
#define	CONSTANT_Float			4
#define	CONSTANT_Long			5
#define	CONSTANT_Double			6
#define	CONSTANT_NameAndType		12
#define	CONSTANT_Utf8			1
#define	CONSTANT_Unicode		2

#define	CONSTANT_LongC			129
#define	CONSTANT_DoubleC		130

#define	CONSTANT_Unknown		0
#define CONSTANT_ResolvedString		(16+CONSTANT_String)
#define CONSTANT_ResolvedClass		(16+CONSTANT_Class)

typedef struct _constants {
	u2	size;
	u1*	tags;
	jword*	data;
} constants;

#define	STRHASHSZ		128

struct Hjava_lang_Class;
struct _classFile;
struct _errorInfo;

bool readConstantPool(struct Hjava_lang_Class*, struct _classFile*, struct _errorInfo*);

/* Extract a character from a Java-style Utf8 string.
 * PTR points to the current character.
 * LIMIT points to the end of the Utf8 string.
 * PTR is incremented to point after the character thta gets returns.
 * On an error, -1 is returned. */
#define UTF8_GET(PTR, LIMIT) \
  ((PTR) >= (LIMIT) ? -1 \
   : *(PTR) < 128 ? *(PTR)++ \
   : (*(PTR)&0xE0) == 0xC0 && ((PTR)+=2)<=(LIMIT) && ((PTR)[-1]&0xC0) == 0x80 \
   ? (((PTR)[-2] & 0x1F) << 6) + ((PTR)[-1] & 0x3F) \
   : (*(PTR) & 0xF0) == 0xE0 && ((PTR) += 3) <= (LIMIT) \
   && ((PTR)[-2] & 0xC0) == 0x80 && ((PTR)[-1] & 0xC0) == 0x80 \
   ? (((PTR)[-3]&0x1F) << 12) + (((PTR)[-2]&0x3F) << 6) + ((PTR)[-1]&0x3F) \
   : ((PTR)++, -1))

extern Utf8Const* makeUtf8Const (char*, int);
extern int32 hashUtf8String(char*, int);

#endif
