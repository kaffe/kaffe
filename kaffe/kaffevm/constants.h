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
#define	CONSTANT_Error			0
#define CONSTANT_ResolvedString		(16+CONSTANT_String)
#define CONSTANT_ResolvedClass		(16+CONSTANT_Class)

/* NB. This should match jvalue and eventually be replaced by it */
typedef struct _slots {
	union {
		jint		tint;
		jword		tword;
		jlong		tlong;
		jfloat		tfloat;
		jdouble		tdouble;
		void*		taddr;
		char*		tstr;
	} v;
} slots;

#define WORDS_TO_LONG(HI, LO) (((uint64)(HI) << 32)|(uint64)(uint32)(LO))

#define CLASS_CONSTANTS(CL) (&(CL)->constants)
#define CLASS_CONST_SIZE(CL) ((CL)->constants.size)
#define CLASS_CONST_TAG(CL, IDX) ((CL)->constants.tags[IDX])
#define CLASS_CONST_DATA(CL, IDX) ((CL)->constants.data[IDX])
#define CLASS_CONST_UTF8(CL, IDX) WORD2UTF(CLASS_CONST_DATA(CL, IDX))
#define CLASS_CONST_INT(CL, IDX) ((int32) CLASS_CONST_DATA(CL, IDX))
#if SIZEOF_VOIDP == 8
#define CLASS_CONST_LONG(CL, IDX) \
  ((uint64) CLASS_CONST_DATA(CL, IDX))
#else
#define CLASS_CONST_LONG(CL, IDX) \
  WORDS_TO_LONG ((CL)->constants.data[IDX], (CL)->constants.data[(IDX)+1])
#endif
/* The first uint16 of the INDEX'th constant pool entry. */
#define CLASS_CONST_USHORT1(CL, INDEX) ((CL)->constants.data[INDEX] & 0xFFFF)
/* The second uint16 of the INDEX'th constant pool entry. */
#define CLASS_CONST_USHORT2(CL, INDEX) \
  ((uint16)((CL)->constants.data[INDEX] >> 16))

#define WORD2UTF(WORD) ((Utf8Const*) (WORD))

typedef jword ConstSlot;

typedef struct _constants {
	u4		size;
	u1*		tags;
	ConstSlot*	data;
} constants;

/*
 * Macros to take constant pools apart.
 */
#define	METHODREF_CLASS(idx, pool)		((uint16)pool->data[idx])
#define	METHODREF_NAMEANDTYPE(idx, pool)	((uint16)(pool->data[idx]>>16))
#define	CLASS_NAME(idx, pool)			((uint16)pool->data[idx])
#define	FIELDREF_CLASS(idx, pool)		METHODREF_CLASS(idx, pool)
#define	FIELDREF_NAMEANDTYPE(idx, pool)		METHODREF_NAMEANDTYPE(idx, pool)
#define	NAMEANDTYPE_NAME(idx, pool)		METHODREF_CLASS(idx, pool)
#define	NAMEANDTYPE_SIGNATURE(idx, pool)	METHODREF_NAMEANDTYPE(idx, pool)
#define	STRING_NAME(idx, pool)			CLASS_NAME(idx, pool)
#define	CONST_TAG(idx, pool)			((idx) > 0 && (idx) < pool->size ?  pool->tags[idx] : CONSTANT_Unknown)

#define CLASS_CLASS(idx, pool)			((Hjava_lang_Class*)pool->data[idx])
#define CLASS_FIELD(idx, pool)			((Field*)pool->data[idx])

struct _classFile;
struct _errorInfo;
struct Hjava_lang_Class;

extern bool readConstantPool(struct Hjava_lang_Class*, struct _classFile*, struct _errorInfo*);

#endif
