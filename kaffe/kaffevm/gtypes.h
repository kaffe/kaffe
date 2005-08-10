/*
 * gtypes.h
 * General types.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __gtype_h
#define __gtype_h

#include "config.h"
#include "config-int.h"
#include "config-std.h"
#include "jni.h"

typedef int8_t		int8;
typedef uint8_t		uint8;

typedef	int16_t		int16;
typedef	uint16_t	uint16;

typedef int32_t		int32;
typedef uint32_t	uint32;

typedef int64_t		int64;
typedef uint64_t	uint64;

#if SIZEOF_VOID_P == 4
typedef uint32			uintp;
typedef int32			intp;
#elif SIZEOF_VOID_P == 8
typedef uint64			uintp;
typedef int64			intp;
#else
#error "sizeof(void*) must be 4 or 8"
#endif

typedef	void*		jref;
typedef intp            jword;

#if !defined(HAVE_BOOL)
typedef enum _bool {
	false	= 0,
	true	= 1
} bool;
#endif

typedef uint8			u1;
typedef uint16			u2;
typedef uint32			u4;

typedef	u1		nativecode;
typedef u2		accessFlags;
typedef u2		constIndex;

typedef struct _jfieldID Field;

#if !defined(__DEFINED_METHOD)
#define __DEFINED_METHOD
typedef struct _jmethodID Method;
#endif
#if !defined(__DEFINED_UTF8CONST)
#define __DEFINED_UTF8CONST
typedef struct _strconst Utf8Const;
#endif

#define PTR_TYPE_SIZE	SIZEOF_VOID_P

struct _constants;
struct _methodTable;
struct _dispatchTable;
struct _jexception;

struct _strconst {
	int32		hash;		/* Hash code (== String.hashCode()) */
	int32		nrefs;		/* Number of references */
	int32		length;
	char	data[sizeof(int32)];	/* In UTF-8 format, with final '\0' */
};

#define	SHIFT_jchar		1
#define	SHIFT_jbyte		0
#define	SHIFT_jshort		1
#define	SHIFT_jint		2
#define	SHIFT_jlong		3
#define	SHIFT_jfloat		2
#define	SHIFT_jdouble		3
#if SIZEOF_VOID_P == 4
#define	SHIFT_jref		2
#elif SIZEOF_VOID_P == 8
#define	SHIFT_jref		3
#endif

#include "kaffe_jni.h"

/* The following two macros will exit or abort the JVM through the hooks
 * provided by JNI.  At this point, we only support one JVM with index 0.
 */
#define	KAFFEVM_EXIT(X)	Kaffe_JavaVMArgs.exit(X)
#define	KAFFEVM_ABORT()	Kaffe_JavaVMArgs.abort()

#endif
