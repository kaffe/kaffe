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
	Hjava_lang_Object* robj;
	Method* meth;
	jint slot;
	jvalue args[255]; /* should this be allocated dynamically? */
	jvalue ret;
	Hjava_lang_Class* a;
	Hjava_lang_Object** body;
	int i;
	int j;
	int len;
	char* sig;
	void* code;

	clazz = unhand(this)->clazz;
	slot = unhand(this)->slot;

	assert(slot < clazz->nmethods);

	meth = &clazz->methods[slot];

	len = argobj ? obj_length(argobj) : 0;
	body = argobj ? (Hjava_lang_Object**)unhand(argobj)->body : 0;
	for (sig = meth->signature->data + 1, i = j = 0;
	     *sig != ')' && i < len; ++sig, ++i, ++j) {
		a = body[i] ? OBJECT_CLASS(body[i]) : 0;
		switch (*sig) {
		case 'Z':
			if (a == javaLangBooleanClass) {
				args[j].i = unhand((Hjava_lang_Boolean*)body[i])->value;
			} else {
				SignalError("java.lang.IllegalArgumentException", "");
			}
			break;
		case 'B':
			if (a == javaLangByteClass) {
				args[j].i = unhand((Hjava_lang_Byte*)body[i])->value;
			} else {
				SignalError("java.lang.IllegalArgumentException", "");
			}
			break;
		case 'C':
			if (a == javaLangCharacterClass) {
				args[j].i = unhand((Hjava_lang_Character*)body[i])->value;
			}
			else if (a == javaLangByteClass) {
				args[j].i = unhand((Hjava_lang_Byte*)body[i])->value;
			} else {
				SignalError("java.lang.IllegalArgumentException", "");
			}
			break;
		case 'S':
			if (a == javaLangShortClass) {
				args[j].i = unhand((Hjava_lang_Short*)body[i])->value;
			}
			else if (a == javaLangByteClass) {
				args[j].i = unhand((Hjava_lang_Byte*)body[i])->value;
			} else {
				SignalError("java.lang.IllegalArgumentException", "");
			}
			break;
		case 'I':
			if (a == javaLangIntegerClass) {
				args[j].i = unhand((Hjava_lang_Integer*)body[i])->value;
			}
			else if (a == javaLangCharacterClass) {
				args[j].i = unhand((Hjava_lang_Character*)body[i])->value;
			}
			else if (a == javaLangShortClass) {
				args[j].i = unhand((Hjava_lang_Short*)body[i])->value;
			}
			else if (a == javaLangByteClass) {
				args[j].i = unhand((Hjava_lang_Byte*)body[i])->value;
			} else {
				SignalError("java.lang.IllegalArgumentException", "");
			}
			break;
		case 'J':
			if (a == javaLangLongClass) {
				args[j].j = unhand((Hjava_lang_Long*)body[i])->value;
			}
			else if (a == javaLangIntegerClass) {
				args[j].j = unhand((Hjava_lang_Integer*)body[i])->value;
			}
			else if (a == javaLangCharacterClass) {
				args[j].j = unhand((Hjava_lang_Character*)body[i])->value;
			}
			else if (a == javaLangShortClass) {
				args[j].j = unhand((Hjava_lang_Short*)body[i])->value;
			}
			else if (a == javaLangByteClass) {
				args[j].j = unhand((Hjava_lang_Byte*)body[i])->value;
			} else {
				SignalError("java.lang.IllegalArgumentException", "");
			}
			++j;
			break;
		case 'F':
			if (a == javaLangFloatClass) {
				args[j].f = unhand((Hjava_lang_Float*)body[i])->value;
			}
			else if (a == javaLangLongClass) {
				args[j].f = unhand((Hjava_lang_Long*)body[i])->value;
			}
			else if (a == javaLangIntegerClass) {
				args[j].f = unhand((Hjava_lang_Integer*)body[i])->value;
			}
			else if (a == javaLangCharacterClass) {
				args[j].f = unhand((Hjava_lang_Character*)body[i])->value;
			}
			else if (a == javaLangShortClass) {
				args[j].f = unhand((Hjava_lang_Short*)body[i])->value;
			}
			else if (a == javaLangByteClass) {
				args[j].f = unhand((Hjava_lang_Byte*)body[i])->value;
			} else {
				SignalError("java.lang.IllegalArgumentException", "");
			}
			break;
		case 'D':
			if (a == javaLangDoubleClass) {
				args[j].d = unhand((Hjava_lang_Double*)body[i])->value;
			}
			else if (a == javaLangFloatClass) {
				args[j].d = unhand((Hjava_lang_Float*)body[i])->value;
			}
			else if (a == javaLangLongClass) {
				args[j].d = unhand((Hjava_lang_Long*)body[i])->value;
			}
			else if (a == javaLangIntegerClass) {
				args[j].d = unhand((Hjava_lang_Integer*)body[i])->value;
			}
			else if (a == javaLangCharacterClass) {
				args[j].d = unhand((Hjava_lang_Character*)body[i])->value;
			}
			else if (a == javaLangShortClass) {
				args[j].d = unhand((Hjava_lang_Short*)body[i])->value;
			}
			else if (a == javaLangByteClass) {
				args[j].d = unhand((Hjava_lang_Byte*)body[i])->value;
			} else {
				SignalError("java.lang.IllegalArgumentException", "");
			}
			++j;
			break;
		case '[':
			while (*++sig == '[')
				;
			if (*sig == 'L')
				/* fall through */
		case 'L':
			while (*++sig != ';')
				;
			args[j].l = body[i];
			break;
		default:
			ABORT();
		}
	}
	if (*sig != ')' || i < len)
		SignalError("java.lang.IllegalArgumentException", "");

	/* Select which method to really call, and call it */
	if (METHOD_IS_STATIC(meth)) {
		code = METHOD_INDIRECTMETHOD(meth);
	}
	else {
		code = obj->dtable->method[meth->idx];
	}
	callMethodA(meth, code, obj, args, &ret);

	switch (*++sig) {
	case 'V':
		robj = 0;
		break;
	case 'J':
		robj = execute_java_constructor(0, javaLangLongClass, "(J)V", ret.j);
		break;
	case 'F':
		robj = execute_java_constructor(0, javaLangFloatClass, "(F)V", ret.f);
		break;
	case 'D':
		robj = execute_java_constructor(0, javaLangDoubleClass, "(D)V", ret.d);
		break;
	case 'I':
		robj = execute_java_constructor(0, javaLangIntegerClass, "(I)V", ret.i);
		break;
	case 'S':
		robj = execute_java_constructor(0, javaLangShortClass, "(S)V", ret.i);
		break;
	case 'B':
		robj = execute_java_constructor(0, javaLangByteClass, "(B)V", ret.i);
		break;
	case 'Z':
		robj = execute_java_constructor(0, javaLangBooleanClass, "(Z)V", ret.i);
		break;
	case 'C':
		robj = execute_java_constructor(0, javaLangCharacterClass, "(C)V", ret.i);
		break;
	case 'L':
	case '[':
		robj = ret.l;
		break;
	default:
		ABORT();
	}

	return (robj);
}
