/*
 * java.lang.reflect.Constructor.c
 *
 * Copyright (c) 1996, 1997
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "lib-license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "../../../kaffe/kaffevm/gtypes.h"
#include "../../../kaffe/kaffevm/access.h"
#include "../../../kaffe/kaffevm/constants.h"
#include "../../../kaffe/kaffevm/object.h"
#include "../../../kaffe/kaffevm/classMethod.h"
#include "../../../kaffe/kaffevm/itypes.h"
#include "../../../kaffe/kaffevm/support.h"
#include "../../../kaffe/kaffevm/baseClasses.h"
#include "Constructor.h"
#include "Method.h"
#include <native.h>
#include <jni.h>
#include "defs.h"

jint
java_lang_reflect_Constructor_getModifiers(struct Hjava_lang_reflect_Constructor* this)
{
	Hjava_lang_Class* clazz;
	jint slot;

	clazz = unhand(this)->clazz;
	slot = unhand(this)->slot;

	assert(slot < clazz->nmethods);

	return (clazz->methods[slot].accflags & ACC_MASK);
}

jobject
Java_java_lang_reflect_Constructor_newInstance(JNIEnv* env, jobject _this, jarray argobj)
{
	Hjava_lang_Object* obj;
	Hjava_lang_reflect_Method meth[1];

	Hjava_lang_reflect_Constructor* this = (Hjava_lang_reflect_Constructor*)_this;

	obj = newObject(unhand(this)->clazz);

	unhand(meth)->clazz = unhand(this)->clazz;
	unhand(meth)->slot = unhand(this)->slot;
	unhand(meth)->name = 0; /* It's a constructor!! */
	unhand(meth)->returnType = javaLangVoidClass;
	unhand(meth)->parameterTypes = unhand(this)->parameterTypes;
	unhand(meth)->exceptionTypes = unhand(this)->exceptionTypes;

	Java_java_lang_reflect_Method_invoke(env, (jobject)meth, (jobject)obj, argobj);

	return ((jobject)obj);
}
