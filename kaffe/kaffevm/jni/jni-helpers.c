/*
 * jni-helpers.c
 * Java Native Interface - JNI helper functions (JNI 1.2).
 *
 * Copyright (c) 1996, 1997, 2004
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#include "config.h"
#include "native.h"
#include "jni.h"
#include "jni_i.h"
#include "jni_funcs.h"
#include "native.h"
#include "classMethod.h"
#include "reflect.h"
#include "java_lang_reflect_Method.h"
#include "java_lang_reflect_Field.h"

jmethodID
KaffeJNI_FromReflectedMethod (JNIEnv *env UNUSED, jobject method)
{
	Hjava_lang_reflect_Method *realMethod = (Hjava_lang_reflect_Method *)method;
	
	return (jmethodID) &(unhand(realMethod)->clazz->methods[unhand(realMethod)->slot]);
}

jfieldID
KaffeJNI_FromReflectedField (JNIEnv *env UNUSED, jobject field)
{
	Hjava_lang_reflect_Field *realField = (Hjava_lang_reflect_Field *)field;

	return (jfieldID) &(unhand(realField)->clazz->fields[unhand(realField)->slot]);
}

jobject
KaffeJNI_ToReflectedMethod (JNIEnv *env UNUSED, jclass cls, jmethodID mid, jboolean isStatic UNUSED)
{
        Hjava_lang_reflect_Method *refMeth;
	Hjava_lang_Class *clazz;
	Method *allMethods;
	int i;

	BEGIN_EXCEPTION_HANDLING(NULL);
	
	clazz = (Hjava_lang_Class *)cls;
	refMeth = NULL;
	for (allMethods = CLASS_METHODS(clazz), i = 0;
	     i < CLASS_NMETHODS(clazz); 
	     i++, allMethods++)
	  {
	    if (allMethods == (Method *)mid)
	      {
		refMeth = KaffeVM_makeReflectMethod(clazz, i);
		break;
	      }
	  }
	END_EXCEPTION_HANDLING();
	
	return (jobject) refMeth;
}

jobject
KaffeJNI_ToReflectedField (JNIEnv *env UNUSED, jclass cls, jfieldID fid, jboolean isStatic UNUSED)
{
        Hjava_lang_reflect_Field *refField;
	Hjava_lang_Class *clazz;
	Field *allFields;
	int i;

	BEGIN_EXCEPTION_HANDLING(NULL);

	clazz = (Hjava_lang_Class *)cls;
	refField = NULL;
	for (allFields = CLASS_FIELDS(clazz), i = 0;
	     i < CLASS_NFIELDS(clazz);
	     i++, allFields++)
	  {
	    if (allFields == (Field *)fid)
	      {
		refField = KaffeVM_makeReflectField(clazz, i);
		break;
	      }
	  }

	END_EXCEPTION_HANDLING();
	
	return (jobject) refField;
}
	
