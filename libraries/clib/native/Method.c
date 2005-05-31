/*
 * java.lang.reflect.Method.c
 *
 * Copyright (c) 1996, 1997, 2004
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "access.h"
#include "constants.h"
#include "object.h"
#include "classMethod.h"
#include "itypes.h"
#include "support.h"
#include "soft.h"
#include "exception.h"
#include "baseClasses.h"
#include "java_lang_reflect_Method.h"
#include <native.h>
#include "jni.h"
#include "defs.h"

static jclass Zclass;
static jclass Bclass;
static jclass Sclass;
static jclass Cclass;
static jclass Iclass;
static jclass Jclass;
static jclass Fclass;
static jclass Dclass;
static jfieldID Zvalue;
static jfieldID Bvalue;
static jfieldID Svalue;
static jfieldID Cvalue;
static jfieldID Ivalue;
static jfieldID Jvalue;
static jfieldID Fvalue;
static jfieldID Dvalue;
static jmethodID Zmeth;
static jmethodID Bmeth;
static jmethodID Smeth;
static jmethodID Cmeth;
static jmethodID Imeth;
static jmethodID Jmeth;
static jmethodID Fmeth;
static jmethodID Dmeth;

JNIEXPORT void JNICALL
Java_java_lang_reflect_Method_init0(JNIEnv* env)
{
	Zclass = (*env)->FindClass(env, "java.lang.Boolean");
	Zvalue = (*env)->GetFieldID(env, Zclass, "value", "Z");
	Zmeth = (*env)->GetMethodID(env, Zclass, "<init>", "(Z)V");

	Bclass = (*env)->FindClass(env, "java.lang.Byte");
	Bvalue = (*env)->GetFieldID(env, Bclass, "value", "B");
	Bmeth = (*env)->GetMethodID(env, Bclass, "<init>", "(B)V");

	Sclass = (*env)->FindClass(env, "java.lang.Short");
	Svalue = (*env)->GetFieldID(env, Sclass, "value", "S");
	Smeth = (*env)->GetMethodID(env, Sclass, "<init>", "(S)V");

	Cclass = (*env)->FindClass(env, "java.lang.Character");
	Cvalue = (*env)->GetFieldID(env, Cclass, "value", "C");
	Cmeth = (*env)->GetMethodID(env, Cclass, "<init>", "(C)V");

	Iclass = (*env)->FindClass(env, "java.lang.Integer");
	Ivalue = (*env)->GetFieldID(env, Iclass, "value", "I");
	Imeth = (*env)->GetMethodID(env, Iclass, "<init>", "(I)V");

	Jclass = (*env)->FindClass(env, "java.lang.Long");
	Jvalue = (*env)->GetFieldID(env, Jclass, "value", "J");
	Jmeth = (*env)->GetMethodID(env, Jclass, "<init>", "(J)V");

	Fclass = (*env)->FindClass(env, "java.lang.Float");
	Fvalue = (*env)->GetFieldID(env, Fclass, "value", "F");
	Fmeth = (*env)->GetMethodID(env, Fclass, "<init>", "(F)V");

	Dclass = (*env)->FindClass(env, "java.lang.Double");
	Dvalue = (*env)->GetFieldID(env, Dclass, "value", "D");
	Dmeth = (*env)->GetMethodID(env, Dclass, "<init>", "(D)V");
}

jint
java_lang_reflect_Method_getModifiers(struct Hjava_lang_reflect_Method* this)
{
	Hjava_lang_Class* clazz;
	jint slot;
	int flags;

	clazz = unhand(this)->clazz;
	slot = unhand(this)->slot;

	assert(slot < CLASS_NMETHODS(clazz));

	flags = clazz->methods[slot].accflags;
	if (flags & ACC_ABSTRACT)
		/* If an abstract method is ever referenced,
		 * it's native code is throwAbstractMethodError, and
		 * ACC_NATIVE is set in findLocalMethod.
		 */
		return (flags & (ACC_MASK-ACC_NATIVE));
	else
		return (flags & ACC_MASK);
}

JNIEXPORT jobject JNICALL
Java_java_lang_reflect_Method_invoke0(JNIEnv* env, jobject _this, jobject _obj, jobjectArray _argobj)
{
	Hjava_lang_Class* clazz;
	jarray paramtypes;
	jobject arg;
	jclass argc;
	Method* meth;
	jmethodID methID;
	jint slot;
	jvalue args[255]; /* should this be allocated dynamically? */
	jvalue ret;
	int len;
	int i;
	char rettype;
	errorInfo info;

	/* Bit of a hack this */
	Hjava_lang_Object* obj = (Hjava_lang_Object*)_obj;
	Hjava_lang_reflect_Method* this = (Hjava_lang_reflect_Method*)_this;
	HArrayOfObject* argobj = (HArrayOfObject*)_argobj;
	jthrowable targetexc;

	clazz = unhand(this)->clazz;
	paramtypes = unhand(this)->parameterTypes;

	/* 
	 * A method invocation via reflection is a first active use, so
	 * initialize the class object.  (XXX: check whether that's true.)
	 */
	/* XXX use JNI here XXX */
	if (clazz->state < CSTATE_USABLE || (_this == NULL)) {
		if (processClass(clazz, CSTATE_COMPLETE, &info) == false) {
			throwError(&info);
		}
	}

	slot = unhand(this)->slot;

	assert(slot < CLASS_NMETHODS(clazz));

	/* Note: we assume here that `meth' is identical to the jmethodID which
	 * would be returned by JNIEnv::GetMethodID for this method.
	 */
	meth = &CLASS_METHODS(clazz)[slot];
	methID = (*env)->FromReflectedMethod(env, _this);

	len = argobj ? obj_length(argobj) : 0;

	rettype = *METHOD_RET_TYPE(meth);

	for (i = len - 1; i >= 0; i--) {
		arg = (*env)->GetObjectArrayElement(env, argobj, i);
		argc = (*env)->GetObjectArrayElement(env, paramtypes, i);
		if (!CLASS_IS_PRIMITIVE((Hjava_lang_Class*)argc)) {
			args[i].l = arg;
		}
		else switch (CLASS_PRIM_SIG((Hjava_lang_Class*)argc)) {
		case 'I':
			args[i].i = (*env)->GetIntField(env, arg, Ivalue);
			break;
		case 'Z':
			args[i].z = (*env)->GetBooleanField(env, arg, Zvalue);
			break;
		case 'S':
			args[i].s = (*env)->GetShortField(env, arg, Svalue);
			break;
		case 'B':
			args[i].b = (*env)->GetByteField(env, arg, Bvalue);
			break;
		case 'C':
			args[i].c = (*env)->GetCharField(env, arg, Cvalue);
			break;
		case 'F':
			args[i].f = (*env)->GetFloatField(env, arg, Fvalue);
			break;
		case 'D':
			args[i].d = (*env)->GetDoubleField(env, arg, Dvalue);
			break;
		case 'J':
			args[i].j = (*env)->GetLongField(env, arg, Jvalue);
			break;
		default:
			break;
		}
	}

	/* Select which method to really call, and call it */
	if (METHOD_IS_STATIC(meth)) {	/* static method */

		switch (rettype) {

		/* invoke proper method via JNI CallStatic<Type>MethodA */
#define CallStaticTypeMethodA(type) \
	(*env)->CallStatic##type##MethodA(env, clazz, methID, args)

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
			KAFFEVM_ABORT();
		}
	}
	else if (METHOD_IS_CONSTRUCTOR(meth)) { /* a constructor */
		/*
		 * This if applies if we are called from Constructor.newInstance
		 */
		ret.l = (*env)->NewObjectA(env, clazz, methID, args);
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
	(*env)->Call##type##MethodA(env, obj, methID, args)

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
			KAFFEVM_ABORT();
		}
	}

	/* If the method completes abruptly by throwing an exception, the 
	 * exception is placed in an InvocationTargetException and thrown 
	 * in turn to the caller of invoke. 
	 */
	targetexc = (*env)->ExceptionOccurred(env);
	if (targetexc != 0) {
		struct Hjava_lang_Throwable* e;

		(*env)->ExceptionClear(env);
		e = (struct Hjava_lang_Throwable *)execute_java_constructor(
			"java.lang.reflect.InvocationTargetException", NULL, NULL,
			"(Ljava/lang/Throwable;)V", targetexc);
		throwException(e);
		assert(!"Not here");
	}
	else switch (rettype) {
	case 'I':
		return ((*env)->NewObject(env, Iclass, Imeth, ret.i));
		break;
	case 'Z':
		return ((*env)->NewObject(env, Zclass, Zmeth, ret.z));
		break;
	case 'S':
		return ((*env)->NewObject(env, Sclass, Smeth, ret.s));
		break;
	case 'B':
		return ((*env)->NewObject(env, Bclass, Bmeth, ret.b));
		break;
	case 'C':
		return ((*env)->NewObject(env, Cclass, Cmeth, ret.c));
		break;
	case 'F':
		return ((*env)->NewObject(env, Fclass, Fmeth, ret.f));
		break;
	case 'D':
		return ((*env)->NewObject(env, Dclass, Dmeth, ret.d));
		break;
	case 'J':
		return ((*env)->NewObject(env, Jclass, Jmeth, ret.j));
		break;
	case 'L':
	case '[':
		return (ret.l);
	default:
		break;
	}

	return (NULL);
}
