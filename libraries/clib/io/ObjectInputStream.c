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
#include <kaffe/jni_md.h>
#include <native.h>
#include "object.h"
#include "classMethod.h"
#include "exception.h"
#include "lookup.h"
#include "soft.h"
#include "support.h"
#include "java_lang_SecurityManager.h"
#include "java_io_ObjectInputStream.h"
#include "java_lang_reflect_Field.h"
#include "java_io_VMObjectStreamClass.h"


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
		struct Hjava_io_ObjectInputStream* this UNUSED,
		struct Hjava_lang_Class* clazz)
{
	if (CLASS_IS_INTERFACE(clazz) || CLASS_IS_ABSTRACT(clazz))
		throwException(InstantiationException(clazz->name->data));

	return newObject(clazz);
}

void
java_io_ObjectInputStream_callConstructor(
		struct Hjava_io_ObjectInputStream* this UNUSED,
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

static void*
getFieldAddress (struct Hjava_lang_reflect_Field* sfield, struct Hjava_lang_Object *obj)
{
	if (unhand(sfield)->slot < CLASS_NSFIELDS(OBJECT_CLASS(obj))) {
		SignalError ("java.lang.IllegalArgumentException", "");
	}

	return (void *)((uintp)obj + FIELD_BOFFSET(CLASS_FIELDS(sfield->clazz) + unhand(sfield)->slot));
}

void java_io_VMObjectStreamClass_setBooleanNative
                (struct Hjava_lang_reflect_Field* sfield,
		 struct Hjava_lang_Object* obj,
		 jboolean value)
{
	*(jboolean *)getFieldAddress (sfield, obj) = value;
}

void java_io_VMObjectStreamClass_setByteNative
                (struct Hjava_lang_reflect_Field* sfield, 
		 struct Hjava_lang_Object* obj,
		 jbyte value)
{
	*(jbyte *)getFieldAddress (sfield, obj) = value;
}

void java_io_VMObjectStreamClass_setCharNative
                (struct Hjava_lang_reflect_Field* sfield, 
		 struct Hjava_lang_Object* obj,
		 jchar value)
{
	*(jchar *)getFieldAddress (sfield, obj) = value;
}

void java_io_VMObjectStreamClass_setShortNative
                (struct Hjava_lang_reflect_Field* sfield, 
		 struct Hjava_lang_Object* obj,
		 jshort value)
{
	*(jshort *)getFieldAddress (sfield, obj) = value;
}

void java_io_VMObjectStreamClass_setIntNative
                (struct Hjava_lang_reflect_Field* sfield, 
		 struct Hjava_lang_Object* obj,
		 jint value)
{
	*(jint *)getFieldAddress (sfield, obj) = value;
}

void java_io_VMObjectStreamClass_setLongNative
                (struct Hjava_lang_reflect_Field* sfield, 
		 struct Hjava_lang_Object* obj,
		 jlong value)
{
	*(jlong *)getFieldAddress (sfield, obj) = value;
}

void java_io_VMObjectStreamClass_setFloatNative
                (struct Hjava_lang_reflect_Field* sfield, 
		 struct Hjava_lang_Object* obj,
		 jfloat value)
{
	*(jfloat *)getFieldAddress (sfield, obj) = value;
}

void java_io_VMObjectStreamClass_setDoubleNative
                (struct Hjava_lang_reflect_Field* sfield, 
		 struct Hjava_lang_Object* obj,
		 jdouble value)
{
	*(jdouble *)getFieldAddress (sfield, obj) = value;
}

void java_io_VMObjectStreamClass_setObjectNative
                (struct Hjava_lang_reflect_Field* sfield,
		 struct Hjava_lang_Object* obj, 
		 struct Hjava_lang_Object* value)
{
	*(struct Hjava_lang_Object **)getFieldAddress (sfield, obj) = value;
}
