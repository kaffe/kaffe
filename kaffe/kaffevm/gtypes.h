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
#include "config-std.h"
#include "jni.h"

#if defined(__STDC__)
#if !defined(HAVE_INT8)
typedef signed char		int8;
#endif
#elif defined(__CHAR_UNSIGNED__)
#error "no signed char type"
#else
#if !defined(HAVE_INT8)
typedef	char			int8;
#endif
#endif

#if !defined(HAVE_UINT8)
typedef	unsigned char		uint8;
#endif

#if SIZEOF_SHORT == 2
#if !defined(HAVE_INT16)
typedef	short			int16;
#endif
#if !defined(HAVE_UINT16)
typedef	unsigned short		uint16;
#endif
#else
#error "sizeof(short) must be 2"
#endif

/* If you change these definitions, also change strtod("-0.0") check
 * in configure.in
 */
#if SIZEOF_INT == 4
#if !defined(HAVE_INT32)
typedef	int			int32;
#endif
#if !defined(HAVE_UINT32)
typedef	unsigned int		uint32;
#endif
#elif SIZEOF_LONG == 4
#if !defined(HAVE_INT32)
typedef	long			int32;
#endif
#if !defined(HAVE_UINT32)
typedef	unsigned long		uint32;
#endif
#else
#error "sizeof(int) or sizeof(long) must be 4"
#endif

/* If you change these definitions, also change strtod("-0.0") check
 * in configure.in
 */
#if SIZEOF_LONG == 8
#if !defined(HAVE_INT64)
typedef	long			int64;
#endif
#if !defined(HAVE_UINT64)
typedef	unsigned long		uint64;
#endif
#elif SIZEOF_LONG_LONG == 8
#if !defined(HAVE_INT64)
typedef	long long		int64;
#endif
#if !defined(HAVE_UINT64)
typedef	unsigned long long	uint64;
#endif
#elif SIZEOF___INT64 == 8
#if !defined(HAVE_INT64)
typedef	__int64			int64;
#endif
#if !defined(HAVE_UINT64)
typedef	unsigned __int64	uint64;
#endif
#else
#error "sizeof(long) or sizeof(long long) or sizeof(__int64) must be 8"
#endif

#if SIZEOF_VOID_P == 4
typedef uint32			uintp;
#elif SIZEOF_VOID_P == 8
typedef uint64			uintp;
#else
#error "sizeof(void*) must be 4 or 8"
#endif

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

typedef struct _fields Field;

#if !defined(__DEFINED_METHOD)
#define __DEFINED_METHOD
typedef struct _methods Method;
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
#define	EXIT(X)	Kaffe_JavaVMArgs.exit(X)
#define	ABORT()	Kaffe_JavaVMArgs.abort()

#endif
