/*
 * native.h
 * Native method support.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	Kaffe.org contributors. See ChangeLog for details. All rights reserved.
 *   
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __native_h
#define __native_h

#include "stddef.h"
#include <jni.h>

struct _jmethodID;
struct Hjava_lang_Class;
struct Hjava_lang_ClassLoader;
struct _strconst;

#if !defined(__DEFINED_METHOD)
#define	__DEFINED_METHOD
typedef struct _jmethodID Method;
#endif
#if !defined(__DEFINED_UTF8CONST)
#define	__DEFINED_UTF8CONST
typedef struct _strconst Utf8Const;
#endif
#if !defined(__DEFINED_CLASS)
#define	__DEFINED_CLASS
typedef struct Hjava_lang_Class Hjava_lang_Class;
#endif

struct _dispatchTable;
struct _iLock;



typedef struct Hjava_lang_Object {
	struct _dispatchTable*  vtable;
	struct _iLock*		lock;
	/* This pointer contains the VM function which should be called
	 * to handle object finalization.
	 */
        void*       finalizer_call;
	/* Data follows on immediately */
} Hjava_lang_Object;

/* Turn a handle into the real thing */
#define	unhand(o)	(o)

/* Include array types */
#include "Arrays.h"

/* Get the strings */
#include <java_lang_String.h>

/* Some internal machine object conversions to "standard" types. */
typedef	Hjava_lang_Class	HClass;
typedef	Hjava_lang_Object	HObject;
typedef	Hjava_lang_String	HString;

extern char*	stringJava2CBuf(const HString*, char*, int);
extern char*	stringJava2C(const HString*);
extern HString*	stringC2Java(const char*);

extern int	utf8ConstEqualJavaString(const Utf8Const*, const HString*);

extern HObject*	AllocObject(const char*, struct Hjava_lang_ClassLoader*);
extern HObject*	AllocArray(jsize len, int type);
extern HObject*	AllocObjectArray(int, const char*, struct Hjava_lang_ClassLoader*);

/*
 * Define KMALLOC, KFREE, etc.
 */
#include "kaffe/jmalloc.h"

/* Pick up errorInfo, postOutOfMemory, throwError, and checkPtr */
#include "errors.h"

/*
 * Functions needed protect calls to non-reentrant functions when a
 * user-level threading package such as jthread is used.
 */
void enterUnsafeRegion(void);
void leaveUnsafeRegion(void);

#endif
