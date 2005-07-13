/*
 * jniweaklib.c -- Test the handling of JNI weak reference.
 *
 * Copyright (C) 2005
 *    The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#include <jni.h>

JNIEXPORT jobject JNICALL
Java_JNIWeakTest_testWeak(JNIEnv *env, jobject obj)
{
	return (*env)->NewWeakGlobalRef(env, obj);
}	

JNIEXPORT void JNICALL 
Java_JNIWeakTest_passWeakArg(JNIEnv *env, jclass clazz,
			     jobject o)
{
  jmethodID weak_fun = (*env)->GetStaticMethodID(env, clazz, "testArgWeak",
						 "(Ljava/lang/Object;Ljava/lang/Object;)V");
  jweak wo = (*env)->NewWeakGlobalRef(env, o);

  (*env)->CallStaticVoidMethod(env, clazz, weak_fun, wo, o);
  
}

JNIEXPORT void JNICALL 
Java_JNIWeakTest_invokeWeak(JNIEnv *env, jclass clazz,
			    jobject o)
{
  jmethodID weak_fun = (*env)->GetMethodID(env, clazz, "testInvoke",
					   "()V");
  jweak wo = (*env)->NewWeakGlobalRef(env, o);

  (*env)->CallVoidMethod(env, wo, weak_fun);
}
