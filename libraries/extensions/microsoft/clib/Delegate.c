/*
 * com.ms.lang.Delegate.c
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
#include "config.h"
#include "config-std.h"
#include "config-mem.h"
#include "gtypes.h"
#include "classMethod.h"
#include "lookup.h"
#include "stringSupport.h"
#include <jni.h>
#include <native.h>
#include "java_lang_reflect_Method.h"

/* Copied from Class.c */
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

/* Copied from Class.c */
static
HArrayOfObject*
makeParameters(Method* meth)
{
	int i;
	HArrayOfObject* array;
	errorInfo info;
	Hjava_lang_Class* clazz;

	array = (HArrayOfObject*)AllocObjectArray(METHOD_NARGS(meth),
	    "Ljava/lang/Class;", 0);
	for (i = 0; i < METHOD_NARGS(meth); ++i) {
		clazz = getClassFromSignaturePart(METHOD_ARG_TYPE(meth, i),
					      meth->class->loader, &info);
		if (clazz == 0) {
			throwError(&info);
		}
		unhand_array(array)->body[i] = &clazz->head;
	}		

        return (array);
}

/* Copied from Class.c */
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
	array = (HArrayOfObject*)AllocObjectArray(nr, "Ljava/lang/Class;", 0);
	ptr = (Hjava_lang_Class**)&unhand_array(array)->body[0];
	for (i = 0; i < nr; i++) {
		errorInfo info;
		Hjava_lang_Class* clazz;
		clazz = getClass(meth->declared_exceptions[i], meth->class, 
				&info);
		if (clazz == 0) {
			throwError(&info);
		}
		*ptr++ = clazz;
	}
	return (array);
}


/* We need to find the method corresponding to the name and signature
 * given.
 */
jobject
Java_com_ms_lang_Delegate_getMethod0(JNIEnv* env, jclass delegate, jobject o, jstring name, jstring sig)
{
	jboolean ncopy;
	jboolean scopy;
	jbyte* bname;
	jbyte* bsig;
	Method* m;
	Hjava_lang_reflect_Method* meth;
	Hjava_lang_Class* clazz;
	Utf8Const* fname;
	Utf8Const* fsig;

	bname = (jbyte*)(*env)->GetStringUTFChars(env, name, &ncopy);
	bsig = (jbyte*)(*env)->GetStringUTFChars(env, sig, &scopy);
	clazz = (Hjava_lang_Class*)(*env)->GetObjectClass(env, o);

	fname = utf8ConstNew(bname, -1);
	fsig = utf8ConstNew(bsig, -1);

	m = findMethodLocal(clazz, fname, fsig);

	utf8ConstRelease(fname);
	utf8ConstRelease(fsig);

	meth = (Hjava_lang_reflect_Method*)
	    AllocObject("java/lang/reflect/Method", 0);
	unhand(meth)->clazz = clazz;
	unhand(meth)->slot = m - CLASS_METHODS(clazz);
	unhand(meth)->name = name;
	unhand(meth)->parameterTypes = makeParameters(m);
	unhand(meth)->exceptionTypes = makeExceptions(m);
	unhand(meth)->returnType = makeReturn(m);

	(*env)->ReleaseStringUTFChars(env, name, bname);
	(*env)->ReleaseStringUTFChars(env, sig, bsig);

	return (meth);
}

