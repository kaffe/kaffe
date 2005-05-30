/*
 * reflect.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
#include "config.h"

#include "gtypes.h"
#include "access.h"
#include "baseClasses.h"
#include "classMethod.h"
#include "constants.h"
#include "exception.h"
#include "itypes.h"
#include "lookup.h"
#include "object.h"
#include "support.h"
#include "errors.h"
#include "stringSupport.h"

#include "java_lang_reflect_Constructor.h"
#include "java_lang_reflect_Method.h"
#include "java_lang_reflect_Field.h"

#include "defs.h"
#include "reflect.h"

static
HArrayOfObject*
makeParameters(Method* meth)
{
	int i;
	HArrayOfObject* array;
	errorInfo info;
	Hjava_lang_Class* clazz;

	array = (HArrayOfObject*)AllocObjectArray(METHOD_NARGS(meth),
	    "Ljava/lang/Class;", NULL);
	for (i = 0; i < METHOD_NARGS(meth); ++i) {
		clazz = getClassFromSignaturePart(METHOD_ARG_TYPE(meth, i),
					      meth->class->loader, &info);
		if (clazz == NULL) {
			throwError(&info);
		}
		unhand_array(array)->body[i] = &clazz->head;
	}

        return (array);
}

static
Hjava_lang_Class*
makeReturn(Method* meth)
{
	errorInfo info;
	Hjava_lang_Class* clazz;

	clazz = getClassFromSignaturePart(METHOD_RET_TYPE(meth), meth->class->loader, &info);
	if (clazz == 0) {
		throwError(&info);
	}
	return (clazz);
}

/*
 * create an array of types for the checked exceptions that this method
 * declared to throw.  These are stored in the declared_exception table
 * as indices into the constant pool.
 *
 * We do not bother to cache the resolved types here.
 */
static
HArrayOfObject*
makeExceptions(Method* meth)
{
	int nr;
	int i;
	HArrayOfObject* array;
	Hjava_lang_Class** ptr;

	if( meth->ndeclared_exceptions == -1 )
	{
		meth = meth->declared_exceptions_u.remote_exceptions;
	}
	nr = meth->ndeclared_exceptions;
	array = (HArrayOfObject*)AllocObjectArray(nr, "Ljava/lang/Class;", NULL);
	ptr = (Hjava_lang_Class**)&unhand_array(array)->body[0];
	for (i = 0; i < nr; i++) {
		errorInfo info;
		Hjava_lang_Class* clazz;
		clazz = getClass(meth->declared_exceptions[i], meth->class,
				&info);
		if (clazz == NULL) {
			throwError(&info);
		}
		*ptr++ = clazz;
	}
	return (array);
}

Hjava_lang_reflect_Constructor*
KaffeVM_makeReflectConstructor(struct Hjava_lang_Class* clazz, int slot)
{
	Hjava_lang_reflect_Constructor* meth;
	Method* mth;

	mth = CLASS_METHODS(clazz) + slot;
	meth = (Hjava_lang_reflect_Constructor*)
	    AllocObject("java/lang/reflect/Constructor", NULL);

	unhand(meth)->clazz = clazz;
	unhand(meth)->slot = slot;
	unhand(meth)->parameterTypes = makeParameters(mth);
	unhand(meth)->exceptionTypes = makeExceptions(mth);

	return (meth);
}

Hjava_lang_reflect_Method*
KaffeVM_makeReflectMethod(struct Hjava_lang_Class* clazz, int slot)
{
	Hjava_lang_reflect_Method* meth;
	Method* mth;

	mth = CLASS_METHODS(clazz) + slot;
	meth = (Hjava_lang_reflect_Method*)
	    AllocObject("java/lang/reflect/Method", NULL);

	unhand(meth)->clazz = clazz;
	unhand(meth)->slot = slot;
	unhand(meth)->name = checkPtr(utf8Const2Java(mth->name));
	unhand(meth)->parameterTypes = makeParameters(mth);
	unhand(meth)->exceptionTypes = makeExceptions(mth);
	unhand(meth)->returnType = makeReturn(mth);

	return (meth);
}

Hjava_lang_reflect_Field*
KaffeVM_makeReflectField(struct Hjava_lang_Class* clazz, int slot)
{
	Hjava_lang_reflect_Field* field;
	Field* fld;
	errorInfo info;

	fld = CLASS_FIELDS(clazz) + slot;
	field = (Hjava_lang_reflect_Field*)
	    AllocObject("java/lang/reflect/Field", NULL);
	unhand(field)->clazz = clazz;
	unhand(field)->slot = slot;
	unhand(field)->type = resolveFieldType(fld, clazz, &info);
	if (unhand(field)->type == NULL) {
		throwError(&info);
	}
	unhand(field)->name = checkPtr(utf8Const2Java(fld->name));
	return (field);
}
