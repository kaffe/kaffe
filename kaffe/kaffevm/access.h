/*
 * access.h
 * Access flags.
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	Kaffe.org contributors, see ChangeLogs for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __access_h
#define __access_h

#include "gtypes.h"

#define	ACC_PUBLIC		0x0001
#define	ACC_PRIVATE		0x0002
#define	ACC_PROTECTED		0x0004
#define	ACC_STATIC		0x0008
#define	ACC_FINAL		0x0010
#define	ACC_SYNCHRONISED	0x0020
#define	ACC_VOLATILE		0x0040
#define	ACC_TRANSIENT		0x0080
#define	ACC_NATIVE		0x0100
#define	ACC_INTERFACE		0x0200
#define	ACC_ABSTRACT		0x0400
#define	ACC_STRICT		0x0800

#define	ACC_MASK		0x07FF

/* Warning: strictfp aka ACC_STRICT is also 0x0800 and used for
   classes and methods */
#define	ACC_CONSTRUCTOR		0x0800

/* only for class */
#define	ACC_GCJ			0x1000
/* only for method: method was jitted */
#define	ACC_JITTED		0x1000
#define ACC_JNI                 0x2000
/* either jitted or native method was found */
#define	ACC_TRANSLATED		0x4000
#define	ACC_VERIFIED		0x8000

typedef enum {
	ACC_TYPE_CLASS,
	ACC_TYPE_INNER_CLASS,
	ACC_TYPE_METHOD,
	ACC_TYPE_INTERFACE_METHOD,
	ACC_TYPE_FIELD,
	ACC_TYPE_INTERFACE_FIELD
} access_type_t;

struct Hjava_lang_Class;

/*
 * Check the validity of the given flags.
 *
 * @param type Type of access.
 * @param access_flags The flag set to check.
 * @return NULL if the flags are valid, otherwise it returns a description
 * of the problem.
 */
const char *checkAccessFlags(access_type_t type, accessFlags access_flags);

/*
 * Check the accessibility of a slot from a specific context.
 *
 * context - The class that wishes to access another class' method/field.
 * target - The class being accessed.
 * target_flags - The access flags for the method/field being accessed in
 *   target.
 * returns - True if the slot is accessible.
 */
int checkAccess(struct Hjava_lang_Class *context,
		struct Hjava_lang_Class *target,
		accessFlags target_flags);
/*
 * Separate method for checking virtual method accessibility.  This is used to
 * check the accessibility when inheritance matters.  For example:
 *
 * package one;
 *
 * class A
 * {
 *   public void foo(B b) { b.foo(); }
 * }
 *
 * abstract class B
 * {
 *   protected abstract void foo();
 * }
 *
 * package two;
 *
 * class C
 *   extends C
 * {
 *   protected void foo() { }
 * }
 *
 * context - The class that wishes to access another class' method.
 * target - The original object class.  This must be specified since public
 *   methods contained within package private classes become public when
 *   inherited by public classes.
 * meth - The method to check for accessibility.
 * returns - True if the method is accessible.
 */
int checkMethodAccess(struct Hjava_lang_Class *context,
		      struct Hjava_lang_Class *target,
		      Method *meth);
int checkFieldAccess(struct Hjava_lang_Class *context,
		     struct Hjava_lang_Class *target,
		     Field *field);

#endif
