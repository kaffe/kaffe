/*
 * native.h
 * Native method support.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __native_h
#define __native_h

#undef	__NORETURN__
#if defined(__GNUC__)
#define	__NORETURN__	__attribute__((noreturn))
#else
#define	__NORETURN__
#endif

#include <jtypes.h>

struct _methods;
struct Hjava_lang_Class;
struct _strconst;

#if !defined(__DEFINED_METHOD)
#define	__DEFINED_METHOD
typedef struct _methods Method;
#endif
#if !defined(__DEFINED_UTF8CONST)
#define	__DEFINED_UTF8CONST
typedef struct _strconst Utf8Const;
#endif
#if !defined(__DEFINED_CLASS)
#define	__DEFINED_CLASS
typedef struct Hjava_lang_Class Hjava_lang_Class;
#endif

#include <java_lang_Object.h>

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

/*
 * Function used for Kaffe Native Interface (KNI).
 */
extern jvalue	do_execute_java_method(void*, const char*, const char*, Method*, int, ...);
extern jvalue	do_execute_java_class_method(const char*, const char*, const char*, ...);
extern HObject*	execute_java_constructor(const char*, HClass*, const char*, ...);

extern char*	stringJava2CBuf(const HString*, char*, int);
extern char*	stringJava2C(const HString*);
extern HString*	stringC2Java(const char*);

extern int	utf8ConstEqualJavaString(const Utf8Const*, const HString*);

extern void	SignalError(const char*, const char*) __NORETURN__;

extern HObject*	AllocObject(const char*);
extern HObject*	AllocArray(int, int);
extern HObject*	AllocObjectArray(int, const char*);

extern void	addNativeMethod(const char*, void*);

extern void	classname2pathname(const char*, char*);

/*
 * Define KMALLOC, KFREE, etc.
 */
#include "jmalloc.h"

#endif
