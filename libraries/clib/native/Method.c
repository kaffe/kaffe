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
#include "../../../kaffe/kaffevm/exception.h"
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
#include "jni.h"
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

jobject
Java_java_lang_reflect_Method_invoke(JNIEnv* env, jobject _this, jobject _obj, jarray _argobj)
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
	char rettype;

	/* Bit of a hack this */
	Hjava_lang_Object* obj = (Hjava_lang_Object*)_obj;
	Hjava_lang_reflect_Method* this = (Hjava_lang_reflect_Method*)_this;
	HArrayOfObject* argobj = (HArrayOfObject*)_argobj;
	jthrowable targetexc;

	clazz = unhand(this)->clazz;

	/* 
	 * make sure constants are resolved and static initializers are run 
	 * before invoking a method on this class
	 */
	if (clazz->state != CSTATE_OK) {
		processClass(clazz, CSTATE_OK);
	}

	slot = unhand(this)->slot;

	assert(slot < clazz->nmethods);

	/* Note: we assume here that `meth' is identical to the jmethodID which
	 * would be returned by JNIEnv::GetMethodID for this method.
	 */
	meth = &clazz->methods[slot];

	len = argobj ? obj_length(argobj) : 0;
	body = argobj ? (Hjava_lang_Object**)unhand(argobj)->body : 0;
	for (sig = meth->signature->data + 1, i = j = 0;
	     *sig != ')' && i < len; ++sig, ++i, ++j) {
		a = body[i] ? OBJECT_CLASS(body[i]) : 0;
		switch (*sig) {
		case 'Z':
			if (a == javaLangBooleanClass) {
				args[j].z = unhand((Hjava_lang_Boolean*)body[i])->value;
			} else {
				SignalError("java.lang.IllegalArgumentException", "");
			}
			break;
		case 'B':
			if (a == javaLangByteClass) {
				args[j].b = unhand((Hjava_lang_Byte*)body[i])->value;
			} else {
				SignalError("java.lang.IllegalArgumentException", "");
			}
			break;
		case 'C':
			if (a == javaLangCharacterClass) {
				args[j].c = unhand((Hjava_lang_Character*)body[i])->value;
			}
			else if (a == javaLangByteClass) {
				args[j].c = unhand((Hjava_lang_Byte*)body[i])->value;
			} else {
				SignalError("java.lang.IllegalArgumentException", "");
			}
			break;
		case 'S':
			if (a == javaLangShortClass) {
				args[j].s = unhand((Hjava_lang_Short*)body[i])->value;
			}
			else if (a == javaLangByteClass) {
				args[j].s = unhand((Hjava_lang_Byte*)body[i])->value;
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
	if (*sig != ')' || i < len) {
		SignalError("java.lang.IllegalArgumentException", "");
	}

	rettype = *++sig;

	/* Select which method to really call, and call it */
	if (METHOD_IS_STATIC(meth)) {	/* static method */

		switch (rettype) {

		/* invoke proper method via JNI CallStatic<Type>MethodA */
#define CallStaticTypeMethodA(type) \
	(*env)->CallStatic##type##MethodA(env, clazz, meth, args)

		case 'V': CallStaticTypeMethodA(Void); break;
		case 'J': ret.j = CallStaticTypeMethodA(Long); break;
		case 'F': ret.f = CallStaticTypeMethodA(Float); break;
		case 'D': ret.d = CallStaticTypeMethodA(Double); break;
		case 'I': ret.i = CallStaticTypeMethodA(Int); break;
		case 'S': ret.s = CallStaticTypeMethodA(Short); break;
		case 'B': ret.b = CallStaticTypeMethodA(Byte); break;
		case 'Z': ret.z = CallStaticTypeMethodA(Boolean); break;
		case 'C': ret.c = CallStaticTypeMethodA(Char); break;
		case 'L': 
		case '[': ret.l = CallStaticTypeMethodA(Object); break;

#undef CallStaticTypeMethodA
		default:
			ABORT();
		}
	}
	else if (meth->accflags & ACC_CONSTRUCTOR) { /* a constructor */
		/*
		 * This if applies if we are called from Constructor.newInstance
		 */
		ret.l = (*env)->NewObjectA(env, clazz, meth, args);

		/* override return type parsed from signature */
		rettype = 'L';
	}
	else {			/* nonstatic method */
		switch (rettype) {

		/* Why Call<Type>MethodA and not CallNonvirtual<Type>MethodA?
		 *
		 * Because the spec says:
		 * If the underlying method is an instance method, it is 
		 * invoked using dynamic method lookup as documented in The 
		 * Java Language Specification, section 15.11.4.4; in 
		 * particular, overriding based on the runtime type of the 
		 * target object will occur. 
		 */
#define CallTypeMethodA(type) \
	(*env)->Call##type##MethodA(env, obj, meth, args)

		case 'V': CallTypeMethodA(Void); break;
		case 'J': ret.j = CallTypeMethodA(Long); break;
		case 'F': ret.f = CallTypeMethodA(Float); break;
		case 'D': ret.d = CallTypeMethodA(Double); break;
		case 'I': ret.i = CallTypeMethodA(Int); break;
		case 'S': ret.s = CallTypeMethodA(Short); break;
		case 'B': ret.b = CallTypeMethodA(Byte); break;
		case 'Z': ret.z = CallTypeMethodA(Boolean); break;
		case 'C': ret.c = CallTypeMethodA(Char); break;
		case 'L': 
		case '[': ret.l = CallTypeMethodA(Object); break;
#undef CallTypeMethodA
		default:
			ABORT();
		}
	}

	/* If the method completes abruptly by throwing an exception, the 
	 * exception is placed in an InvocationTargetException and thrown 
	 * in turn to the caller of invoke. 
	 */
	targetexc = (*env)->ExceptionOccurred(env);
	if (targetexc != 0) {
		Hjava_lang_Object* obj;

		(*env)->ExceptionClear(env);
		obj = execute_java_constructor(
			"java.lang.reflect.InvocationTargetException", 0, 
			"(Ljava/lang/Throwable;)V", targetexc);
		throwException(obj);
		assert(!"Not here");
	}

	switch (rettype) {
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
		robj = execute_java_constructor(0, javaLangShortClass, "(S)V", ret.s);
		break;
	case 'B':
		robj = execute_java_constructor(0, javaLangByteClass, "(B)V", ret.b);
		break;
	case 'Z':
		robj = execute_java_constructor(0, javaLangBooleanClass, "(Z)V", ret.z);
		break;
	case 'C':
		robj = execute_java_constructor(0, javaLangCharacterClass, "(C)V", ret.c);
		break;
	case 'L':
	case '[':
		robj = ret.l;
		break;
	default:
		robj = 0;	/* avoid compiler warning */
		ABORT();
	}

	return ((jobject)robj);
}
