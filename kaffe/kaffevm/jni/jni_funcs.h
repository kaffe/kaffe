/*
 * jni_funcs.h
 * Java Native Interface - Handles JNI function declarations.
 *
 * Copyright (c) 2004
 *      The Kaffe.org's developers. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef KAFFE_JNI_FUNCTIONS_H
#define KAFFE_JNI_FUNCTIONS_H

jobject
KaffeJNI_CallNonvirtualObjectMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jobject
KaffeJNI_CallNonvirtualObjectMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jobject
KaffeJNI_CallNonvirtualObjectMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

jboolean
KaffeJNI_CallNonvirtualBooleanMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jboolean
KaffeJNI_CallNonvirtualBooleanMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jboolean
KaffeJNI_CallNonvirtualBooleanMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

jbyte
KaffeJNI_CallNonvirtualByteMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jbyte
KaffeJNI_CallNonvirtualByteMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jbyte
KaffeJNI_CallNonvirtualByteMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

jchar
KaffeJNI_CallNonvirtualCharMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jchar
KaffeJNI_CallNonvirtualCharMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jchar
KaffeJNI_CallNonvirtualCharMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

jshort
KaffeJNI_CallNonvirtualShortMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jshort
KaffeJNI_CallNonvirtualShortMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jshort
KaffeJNI_CallNonvirtualShortMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

jint
KaffeJNI_CallNonvirtualIntMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jint
KaffeJNI_CallNonvirtualIntMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jint
KaffeJNI_CallNonvirtualIntMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);


void
KaffeJNI_CallVoidMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
void
KaffeJNI_CallVoidMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
void
KaffeJNI_CallVoidMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jobject
KaffeJNI_CallObjectMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jobject
KaffeJNI_CallObjectMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jobject
KaffeJNI_CallObjectMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jboolean
KaffeJNI_CallBooleanMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jboolean
KaffeJNI_CallBooleanMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jboolean
KaffeJNI_CallBooleanMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jbyte
KaffeJNI_CallByteMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jbyte
KaffeJNI_CallByteMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jbyte
KaffeJNI_CallByteMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jchar
KaffeJNI_CallCharMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jchar
KaffeJNI_CallCharMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jchar
KaffeJNI_CallCharMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jshort
KaffeJNI_CallShortMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jshort
KaffeJNI_CallShortMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jshort
KaffeJNI_CallShortMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jint
KaffeJNI_CallIntMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jint
KaffeJNI_CallIntMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jint
KaffeJNI_CallIntMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jlong
KaffeJNI_CallLongMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jlong
KaffeJNI_CallLongMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jlong
KaffeJNI_CallLongMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jfloat
KaffeJNI_CallFloatMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jfloat
KaffeJNI_CallFloatMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jfloat
KaffeJNI_CallFloatMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jdouble
KaffeJNI_CallDoubleMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jdouble
KaffeJNI_CallDoubleMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jdouble
KaffeJNI_CallDoubleMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jlong
KaffeJNI_CallNonvirtualLongMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jlong
KaffeJNI_CallNonvirtualLongMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jlong
KaffeJNI_CallNonvirtualLongMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

jfloat
KaffeJNI_CallNonvirtualFloatMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jfloat
KaffeJNI_CallNonvirtualFloatMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jfloat
KaffeJNI_CallNonvirtualFloatMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

jdouble
KaffeJNI_CallNonvirtualDoubleMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jdouble
KaffeJNI_CallNonvirtualDoubleMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jdouble
KaffeJNI_CallNonvirtualDoubleMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

void
KaffeJNI_CallNonvirtualVoidMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
void
KaffeJNI_CallNonvirtualVoidMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
void
KaffeJNI_CallNonvirtualVoidMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

void
KaffeJNI_CallStaticVoidMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
void
KaffeJNI_CallStaticVoidMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
void
KaffeJNI_CallStaticVoidMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jchar
KaffeJNI_CallStaticCharMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jchar
KaffeJNI_CallStaticCharMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jchar
KaffeJNI_CallStaticCharMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jbyte
KaffeJNI_CallStaticByteMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jbyte
KaffeJNI_CallStaticByteMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jbyte
KaffeJNI_CallStaticByteMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jshort
KaffeJNI_CallStaticShortMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jshort
KaffeJNI_CallStaticShortMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jshort
KaffeJNI_CallStaticShortMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jint
KaffeJNI_CallStaticIntMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jint
KaffeJNI_CallStaticIntMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jint
KaffeJNI_CallStaticIntMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jlong
KaffeJNI_CallStaticLongMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jlong
KaffeJNI_CallStaticLongMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jlong
KaffeJNI_CallStaticLongMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jfloat
KaffeJNI_CallStaticFloatMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jfloat
KaffeJNI_CallStaticFloatMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jfloat
KaffeJNI_CallStaticFloatMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jdouble
KaffeJNI_CallStaticDoubleMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jdouble
KaffeJNI_CallStaticDoubleMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jdouble
KaffeJNI_CallStaticDoubleMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jboolean
KaffeJNI_CallStaticBooleanMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jboolean
KaffeJNI_CallStaticBooleanMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jboolean
KaffeJNI_CallStaticBooleanMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jobject
KaffeJNI_CallStaticObjectMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jobject
KaffeJNI_CallStaticObjectMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jobject
KaffeJNI_CallStaticObjectMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

#endif
