/*
 * java.lang.reflect.Method.c
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
#include "Method.h"
#include "Boolean.h"
#include "Byte.h"
#include "Character.h"
#include "Short.h"
#include "Integer.h"
#include "Long.h"
#include "Float.h"
#include "Double.h"
#include <native.h>
#include "defs.h"

jint
java_lang_reflect_Method_getModifiers(struct Hjava_lang_reflect_Method* this)
{
	Hjava_lang_Class* clazz;
	jint slot;

	clazz = unhand(this)->clazz;
	slot = unhand(this)->slot;

	assert(slot < clazz->nmethods);

	return (clazz->methods[slot].accflags & ACC_MASK);
}

struct Hjava_lang_Object*
java_lang_reflect_Method_invoke(struct Hjava_lang_reflect_Method* this, struct Hjava_lang_Object* obj, HArrayOfObject* argobj)
{
	Hjava_lang_Class* clazz;
	Hjava_lang_Class* retclazz;
	Hjava_lang_Object* robj;
	Method* meth;
	jint slot;
	jvalue args[16];
	jvalue ret;
	Hjava_lang_Class* a;
	Hjava_lang_Object** body;
	int i;

	clazz = unhand(this)->clazz;
	slot = unhand(this)->slot;

	assert(slot < clazz->nmethods);

	meth = &clazz->methods[slot];

	if (argobj != 0 && obj_length(argobj) > 0) {
		body = (Hjava_lang_Object**)unhand(argobj)->body;
		for (i = obj_length(argobj) - 1; i >= 0; i--) {
			a = body[i] ? OBJECT_CLASS(body[i]) : 0;
			if (a == javaLangLongClass) {
				args[i].j = unhand((Hjava_lang_Long*)body[i])->value;
			}
			else if (a == javaLangFloatClass) {
				args[i].f = unhand((Hjava_lang_Float*)body[i])->value;
			}
			else if (a == javaLangDoubleClass) {
				args[i].d = unhand((Hjava_lang_Double*)body[i])->value;
			}
			else if (a == javaLangIntegerClass) {
				args[i].i = unhand((Hjava_lang_Integer*)body[i])->value;
			}
			else if (a == javaLangShortClass) {
				args[i].s = unhand((Hjava_lang_Short*)body[i])->value;
			}
			else if (a == javaLangByteClass) {
				args[i].b = unhand((Hjava_lang_Byte*)body[i])->value;
			}
			else if (a == javaLangBooleanClass) {
				args[i].z = unhand((Hjava_lang_Boolean*)body[i])->value;
			}
			else if (a == javaLangCharacterClass) {
				args[i].c = unhand((Hjava_lang_Character*)body[i])->value;
			}
			else {
				args[i].l = body[i];
			}
		}
	}
	callMethodA(meth, METHOD_NATIVECODE(meth), obj, args, &ret);

	retclazz = unhand(this)->returnType;
	if (retclazz == javaLangVoidClass) {
		robj = 0;
	}
	else if (retclazz == javaLangLongClass) {
		robj = execute_java_constructor(0, retclazz, "(J)V", ret.j);

	}
	else if (retclazz == javaLangFloatClass) {
		robj = execute_java_constructor(0, retclazz, "(F)V", ret.f);
	}
	else if (retclazz == javaLangDoubleClass) {
		robj = execute_java_constructor(0, retclazz, "(D)V", ret.d);
	}
	else if (retclazz == javaLangIntegerClass) {
		robj = execute_java_constructor(0, retclazz, "(I)V", ret.i);
	}
	else if (retclazz == javaLangShortClass) {
		robj = execute_java_constructor(0, retclazz, "(S)V", ret.s);
	}
	else if (retclazz == javaLangByteClass) {
		robj = execute_java_constructor(0, retclazz, "(B)V", ret.b);
	}
	else if (retclazz == javaLangBooleanClass) {
		robj = execute_java_constructor(0, retclazz, "(Z)V", ret.z);
	}
	else if (retclazz == javaLangCharacterClass) {
		robj = execute_java_constructor(0, retclazz, "(C)V", ret.c);
	}
	else {
		robj = ret.l;
	}

	return (robj);
}
