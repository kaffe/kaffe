/*
 * cnstants.h
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

typedef uintp ConstSlot;

typedef struct _constants {
	u4		size;
	u1*		tags;
	ConstSlot*	data;
} constants;



/*
 * Macros to take constant pools apart.  XX prefix with CONST_
 */
#define CONST_TAG(idx, pool)                            (((idx) > 0 && (idx) < pool->size) ?  pool->tags[idx] : CONSTANT_Unknown)

#define CLASS_CLASS(idx, pool)                          ((Hjava_lang_Class*)pool->data[idx])

#define CLASS_NAME(idx, pool)                           ((uint16)pool->data[idx])
#define CONST_STRING_NAME(idx, pool)                    CLASS_NAME(idx, pool)

/* the actual character string pointed to by the data */
#define UNRESOLVED_CLASS_NAMED(idx, pool)               (WORD2UTF(pool->data[idx])->data)
#define RESOLVED_CLASS_NAMED(idx, pool)                 ((CLASS_CLASS(idx, pool))->name->data)

#define CLASS_NAMED(idx, pool)                          ((pool->tags[idx] == CONSTANT_ResolvedClass || pool->tags[idx] == CONSTANT_ResolvedString) ? RESOLVED_CLASS_NAMED(idx, pool) : UNRESOLVED_CLASS_NAMED(idx, pool))

#define CONST_STRING_NAMED(idx, pool)                   CLASS_NAMED(idx, pool)


#define FIELDREF_CLASS(idx, pool)                       ((uint16)pool->data[idx])
#define METHODREF_CLASS(idx, pool)                      FIELDREF_CLASS(idx, pool)
#define INTERFACEMETHODREF_CLASS(idx, pool)             FIELDREF_CLASS(idx, pool)

#define FIELDREF_NAMEANDTYPE(idx, pool)                 ((uint16)(pool->data[idx] >> 16))
#define METHODREF_NAMEANDTYPE(idx, pool)                FIELDREF_NAMEANDTYPE(idx, pool)
#define INTERFACEMETHODREF_NAMEANDTYPE(idx, pool)       FIELDREF_NAMEANDTYPE(idx, pool)


#define NAMEANDTYPE_NAME(idx, pool)                     ((uint16)pool->data[idx])
#define NAMEANDTYPE_SIGNATURE(idx, pool)                ((uint16)(pool->data[idx] >> 16))

/* the actual character string pointed to by the data */
#define NAMEANDTYPE_NAMED(idx, pool)                    (WORD2UTF(pool->data[NAMEANDTYPE_NAME(idx, pool)])->data)
#define NAMEANDTYPE_SIGD(idx, pool)                     (WORD2UTF(pool->data[NAMEANDTYPE_SIGNATURE(idx, pool)])->data)


#define FIELDREF_NAME(idx, pool)                        NAMEANDTYPE_NAME(FIELDREF_NAMEANDTYPE(idx, pool), pool)
#define METHODREF_NAME(idx, pool)                       FIELDREF_NAME(idx, pool)
#define INTERFACEMETHODREF_NAME(idx, pool)              FIELDREF_NAME(idx, pool)

/* the actual character string pointed to by the data */
#define FIELDREF_NAMED(idx, pool)                       (WORD2UTF(pool->data[FIELDREF_NAME(idx, pool)])->data)
#define METHODREF_NAMED(idx, pool)                      FIELDREF_NAMED(idx, pool)
#define INTERFACEMETHODREF_NAMED(idx, pool)             FIELDREF_NAMED(idx, pool)

#define FIELDREF_SIGNATURE(idx, pool)                   NAMEANDTYPE_SIGNATURE(FIELDREF_NAMEANDTYPE(idx, pool), pool)
#define METHODREF_SIGNATURE(idx, pool)                  FIELDREF_SIGNATURE(idx, pool)
#define INTERFACEMETHODREF_SIGNATURE(idx, pool)         FIELDREF_SIGNATURE(idx, pool)

/* aliases...make more sense in certain situations, like type checking */
#define FIELDREF_TYPE(idx, pool)                        FIELDREF_SIGNATURE(idx, pool)
#define METHODREF_TYPE(idx, pool)                       FIELDREF_TYPE(idx, pool)
#define INTERFACEMETHODREF_TYPE(idx, pool)              FIELDREF_TYPE(idx, pool)

/* the actual character string pointed to by the data */
#define FIELDREF_SIGD(idx, pool)                        (WORD2UTF(pool->data[FIELDREF_SIGNATURE(idx, pool)])->data)
#define METHODREF_SIGD(idx, pool)                       FIELDREF_SIGD(idx, pool)
#define INTERFACEMETHODREF_SIGD(idx, pool)              FIELDREF_SIGD(idx, pool)


#define CONST_UTF2CHAR(idx, pool)                       ((pool->data[idx] != 0) ? (((Utf8Const*)pool->data[idx])->data) : "")



struct classFile;
struct _errorInfo;
struct Hjava_lang_Class;

extern bool readConstantPool(struct Hjava_lang_Class*, struct classFile*, struct _errorInfo*);

#endif
