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

/* Build an object handle */
#define	HandleTo(class)					\
	typedef struct H##class {			\
		Hjava_lang_Object base;			\
		struct Class##class data[1];		\
	} H##class

/* Turn a handle into the real thing */
#define	unhand(o)	((o)->data)

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
extern jvalue	do_execute_java_method(void*, char*, char*, Method*, int, ...);
extern jvalue	do_execute_java_class_method(char*, char*, char*, ...);
extern HObject*	execute_java_constructor(char*, HClass*, char*, ...);

extern char*	javaString2CString(HString*, char*, int);
extern char*	makeCString(HString*);
extern HString*	makeJavaString(char*, int);
extern int	equalUtf8JavaStrings(Utf8Const*, HString*);

extern void	SignalError(char*, char*) __NORETURN__;

extern HObject*	AllocObject(char*);
extern HObject*	AllocArray(int, int);
extern HObject*	AllocObjectArray(int, char*);

extern void	addNativeMethod(char*, void*);

extern void	classname2pathname(char*, char*);

/*
 * Redirect the malloc/free functions
 */
#include "jmalloc.h"

#endif
