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

#define	CONSTANT_Unknown		0
#define	CONSTANT_Error			0
#define CONSTANT_ResolvedString		(16+CONSTANT_String)
#define CONSTANT_ResolvedClass		(16+CONSTANT_Class)

#define WORDS_TO_LONG(HI, LO) (((uint64)(HI) << 32)|(uint64)(uint32)(LO))

#define WORD2UTF(WORD) ((Utf8Const*) (WORD))

typedef jword ConstSlot;

typedef struct _constants {
	u4		size;
	u1*		tags;
	ConstSlot*	data;
} constants;

/*
 * Macros to take constant pools apart.  XX prefix with CONST_
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

struct classFile;
struct _errorInfo;
struct Hjava_lang_Class;

extern bool readConstantPool(struct Hjava_lang_Class*, struct classFile*, struct _errorInfo*);

#endif
