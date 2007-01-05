/*
 * jni-helpers.c
 * Java Native Interface - JNI helper functions (JNI 1.2).
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
#include "native.h"
#include "jni.h"
#include "jni_i.h"
#include "jni_funcs.h"
#include "native.h"
#include "classMethod.h"
#include "reflect.h"
#include "java_lang_reflect_Method.h"
#include "java_lang_reflect_Field.h"
#include "soft.h"
#include "baseClasses.h"

jobject KaffeJNI_NewDirectByteBuffer(JNIEnv *env UNUSED, void *buffer, jlong size)
{
  jobject bbuf;
  jobject pointer;

  BEGIN_EXCEPTION_HANDLING(NULL);

  /* We cannot use FindClass as it uses the loader from the stack trace and we
   * want to use the bootstrap class loader
   */
#if SIZEOF_VOID_P == 4
  pointer = execute_java_constructor(NULL, NULL, gnuClasspathPointerClass, "(I)V", (jint)buffer);
#elif SIZEOF_VOID_P == 8
  pointer = execute_java_constructor(NULL, NULL, gnuClasspathPointerClass, "(J)V", (jlong)buffer);
#else
#error "Unknown void pointer width"
#endif

  bbuf = execute_java_constructor(NULL, NULL, javaNioDirectByteBufferImplReadWriteClass, "(Ljava/lang/Object;Lgnu/classpath/Pointer;III)V",
				  NULL, pointer, (jint)size, (jint)size, (jint)0);

  END_EXCEPTION_HANDLING();

  return bbuf;
}

void *KaffeJNI_GetDirectBufferAddress(JNIEnv *env UNUSED, jobject buffer)
{
  jobject buffer_obj = buffer;
  jobject address_obj;
  void *address = NULL;
  Hjava_lang_Object *buffer_local;

  BEGIN_EXCEPTION_HANDLING(NULL);

  buffer_local = (Hjava_lang_Object *)unveil(buffer_obj);

  if (!instanceof(javaNioBufferClass, OBJECT_CLASS(buffer_local)))
  {
	  printf("buffer_local is %s\n", OBJECT_CLASS(buffer_local)->name->data);
  } else
    {
      address_obj = KNI_GET_FIELD(Hjava_lang_Object *, buffer_local, directByteBufferImplAddress);
      if (NULL != address_obj)
        address = KNI_GET_FIELD(void *, address_obj, gnuClasspathPointerAddress);
    }

  END_EXCEPTION_HANDLING();

  return address;
}

jlong KaffeJNI_GetDirectBufferCapacity(JNIEnv *env UNUSED, jobject buffer)
{  
  jmethodID capacity_method;
  jint capacity;
  jclass clazz;
  jobject buffer_local;

  BEGIN_EXCEPTION_HANDLING(-1);
  
  buffer_local = unveil(buffer);

  clazz = (*env)->FindClass(env, "java/nio/DirectByteBufferImpl");
  if (!(*env)->IsInstanceOf(env, buffer_local, clazz))
    capacity = -1;
  else
    {
      clazz = (*env)->GetObjectClass(env, buffer_local);
      capacity_method = (*env)->GetMethodID(env, clazz, "capacity", "()I");
      
      capacity = (*env)->CallIntMethod(env, buffer_local, capacity_method);
    }

  END_EXCEPTION_HANDLING();

  return capacity;
}


jmethodID
KaffeJNI_FromReflectedMethod (JNIEnv *env UNUSED, jobject method)
{
  jobject method_local;
  jmethodID id;
  Hjava_lang_reflect_Method *realMethod;

  BEGIN_EXCEPTION_HANDLING(NULL);
  method_local = unveil(method);
  realMethod = (Hjava_lang_reflect_Method *)method_local;

  id = (jmethodID) &(Kaffe_get_class_methods(unhand(realMethod)->declaringClass)[unhand(realMethod)->slot]);

  END_EXCEPTION_HANDLING();

  return id;
}

jfieldID
KaffeJNI_FromReflectedField (JNIEnv *env UNUSED, jobject field)
{
  jobject field_local;
  Hjava_lang_reflect_Field *realField;
  jfieldID id;

  BEGIN_EXCEPTION_HANDLING(NULL);
  field_local = unveil(field);

  realField = (Hjava_lang_reflect_Field *)field_local;

  id = (jfieldID) &(unhand(realField)->declaringClass->fields[unhand(realField)->slot]);
  END_EXCEPTION_HANDLING();
  
  return id;
}

jobject
KaffeJNI_ToReflectedMethod (JNIEnv *env UNUSED, jclass cls, jmethodID mid, jboolean isStatic UNUSED)
{
        Hjava_lang_reflect_Method *refMeth;
	Hjava_lang_Class *clazz;
	Method *allMethods;
	int i;
	jclass cls_local;

	BEGIN_EXCEPTION_HANDLING(NULL);
	
	cls_local = unveil(cls);
	clazz = (Hjava_lang_Class *)cls_local;
	refMeth = NULL;
	do
	  {
	    for (allMethods = Kaffe_get_class_methods(clazz), i = 0;
		 i < CLASS_NMETHODS(clazz); 
		 i++, allMethods++)
	      {
		if (allMethods == (Method *)mid)
		  {
		    refMeth = KaffeVM_makeReflectMethod(clazz, i);
		    break;
		  }
	      }
	    clazz = clazz->superclass;
	  }
	while (clazz != NULL && refMeth == NULL);

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
	jclass cls_local;

	BEGIN_EXCEPTION_HANDLING(NULL);

	cls_local = unveil(cls);
	clazz = (Hjava_lang_Class *)cls_local;
	refField = NULL;
	do
	  {
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
	    clazz = clazz->superclass;
	  }
	while (clazz != NULL && refField == NULL);

	END_EXCEPTION_HANDLING();
	
	return (jobject) refField;
}

