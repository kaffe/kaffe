/*
 * ObjectInputStream.c
 * 
 * Copyright (C) 2003, 2004 Kaffe.org's team. 
 *
 * See "licence.terms" for information on usage and redistribution 
 * of this file.
 */
#include "config.h"
#include "config-std.h"
#include <kaffe/jtypes.h>
#include <native.h>
#include "java_lang_SecurityManager.h"
#include "java_io_ObjectInputStream.h"
#include "../../../kaffe/kaffevm/object.h"
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/exception.h"
#include "../../../kaffe/kaffevm/lookup.h"
#include "../../../kaffe/kaffevm/soft.h"
#include "../../../kaffe/kaffevm/support.h"

struct Hjava_lang_ClassLoader*
java_io_ObjectInputStream_currentClassLoader(
		struct Hjava_lang_SecurityManager* mgr)
{
	return (struct Hjava_lang_ClassLoader *)
		do_execute_java_method(mgr, "currentClassLoader",
				"()Ljava/lang/ClassLoader;", 0, 0).l;
}

struct Hjava_lang_Object*
java_io_ObjectInputStream_allocateObject(
		struct Hjava_io_ObjectInputStream* this,
		struct Hjava_lang_Class* clazz)
{
	if (CLASS_IS_INTERFACE(clazz) || CLASS_IS_ABSTRACT(clazz))
		throwException(InstantiationException(clazz->name->data));

	return newObject(clazz);
}

void
java_io_ObjectInputStream_callConstructor(
		struct Hjava_io_ObjectInputStream* this,
		struct Hjava_lang_Class* clazz,
		struct Hjava_lang_Object* object)
{
	Method* meth;

	/* Taken from the ObjectStreamClassImpl.c */
	meth = findMethodLocal(clazz, constructor_name, void_signature);
	if (meth == 0) {
		SignalErrorf("java.io.InvalidClassException",
			     "%s; Missing no-arg constructor for class",
			     CLASS_CNAME(clazz));
	}
	else if ((meth->accflags & (ACC_CONSTRUCTOR|ACC_PRIVATE)) != ACC_CONSTRUCTOR) {
		SignalErrorf("java.io.InvalidClassException",
			     "%s; IllegalAccessException",
			     CLASS_CNAME(clazz));
	}
	else {
		do_execute_java_method(object, 0, 0, meth, 0);
	}
}


jboolean
java_io_VMObjectStreamClass_hasClassInitializer(struct Hjava_lang_Class* clazz)
{
	Method* meth;

	/* I prefer to remain prudent */
	if (clazz == NULL)
	{
		soft_nullpointer();
	}

	meth = findMethodLocal(clazz, init_name, void_signature);
	return (meth != NULL);		
}
