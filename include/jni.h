/*
 * jni.h
 * Java Native Interface.
 *
 * Copyright (c) 1996, 1997, 1998
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 */

#ifndef __jni_h
#define	__jni_h

#include <stdio.h>
#include <stdarg.h>
#include "jtypes.h"
#include "jmalloc.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define	JNIEXPORT
#define	JNICALL

struct JNIEnv_;
struct JNINativeInterface;
struct JavaVM_;
struct JNINativeMethod_;
struct JavaVMAttachArgs;
struct JavaVMInitArgs;

#if defined(__cplusplus)
typedef JNIEnv_ JNIEnv;
typedef JavaVM_ JavaVM;
#else
typedef const struct JNINativeInterface* JNIEnv;
typedef const struct JNIInvokeInterface* JavaVM;
#endif

typedef struct JNINativeMethod_ JNINativeMethod;
typedef struct JavaVMAttachArgs ThreadAttachArgs;
typedef struct JavaVMInitArgs JavaVMInitArgs;

typedef	void*	jmethodID;
typedef	void*	jfieldID;

#define	JNI_TRUE	1
#define	JNI_FALSE	0

#define	JNI_COMMIT	1
#define	JNI_ABORT	2

struct JNINativeInterface {

	void*	reserved0;
	void*	reserved1;
	void*	reserved2;
	void*	reserved3;
	jint	(*GetVersion)			(JNIEnv*);
	jclass	(*DefineClass)			(JNIEnv*, jobject, const jbyte*, jsize);
	jclass	(*FindClass)			(JNIEnv*, const char*);
	void*	reserved4;
	void*	reserved5;
	void*	reserved6;
	jclass	(*GetSuperClass)		(JNIEnv*, jclass);
	jboolean	(*IsAssignableFrom)		(JNIEnv*, jclass, jclass);
	void*	reserved7;
	jint	(*Throw)			(JNIEnv*, jobject);
	jint	(*ThrowNew)			(JNIEnv*, jclass, const char*);
	jthrowable (*ExceptionOccurred)		(JNIEnv*);
	void	(*ExceptionDescribe)		(JNIEnv*);
	void	(*ExceptionClear)		(JNIEnv*);
	void	(*FatalError)			(JNIEnv*, const char*);
	void*	reserved8;
	void*	reserved9;
	jref	(*NewGlobalRef)			(JNIEnv*, jref);
	void	(*DeleteGlobalRef)		(JNIEnv*, jref);
	void	(*DeleteLocalRef)		(JNIEnv*, jref);
	jboolean	(*IsSameObject)			(JNIEnv*, jobject, jobject);
	void*	reserved10;
	void*	reserved11;
	jobject	(*AllocObject)			(JNIEnv*, jclass);
	jobject	(*NewObject)			(JNIEnv*, jclass, jmethodID, ...);
	jobject	(*NewObjectV)			(JNIEnv*, jclass, jmethodID, va_list);
	jobject	(*NewObjectA)			(JNIEnv*, jclass, jmethodID, jvalue*);
	jclass	(*GetObjectClass)		(JNIEnv*, jobject);
	jboolean	(*IsInstanceOf)			(JNIEnv*, jobject, jclass);
	jmethodID (*GetMethodID)		(JNIEnv*, jclass, const char*, const char*);
	jobject	(*CallObjectMethod)		(JNIEnv*, jobject, jmethodID, ...);
	jobject	(*CallObjectMethodV)		(JNIEnv*, jobject, jmethodID, va_list);
	jobject	(*CallObjectMethodA)		(JNIEnv*, jobject, jmethodID, jvalue*);
	jboolean	(*CallBooleanMethod)		(JNIEnv*, jobject, jmethodID, ...);
	jboolean	(*CallBooleanMethodV)		(JNIEnv*, jobject, jmethodID, va_list);
	jboolean	(*CallBooleanMethodA)		(JNIEnv*, jobject, jmethodID, jvalue*);
	jbyte	(*CallByteMethod)		(JNIEnv*, jobject, jmethodID, ...);
	jbyte	(*CallByteMethodV)		(JNIEnv*, jobject, jmethodID, va_list);
	jbyte	(*CallByteMethodA)		(JNIEnv*, jobject, jmethodID, jvalue*);
	jchar	(*CallCharMethod)		(JNIEnv*, jobject, jmethodID, ...);
	jchar	(*CallCharMethodV)		(JNIEnv*, jobject, jmethodID, va_list);
	jchar	(*CallCharMethodA)		(JNIEnv*, jobject, jmethodID, jvalue*);
	jshort	(*CallShortMethod)		(JNIEnv*, jobject, jmethodID, ...);
	jshort	(*CallShortMethodV)		(JNIEnv*, jobject, jmethodID, va_list);
	jshort	(*CallShortMethodA)		(JNIEnv*, jobject, jmethodID, jvalue*);
	jint	(*CallIntMethod)		(JNIEnv*, jobject, jmethodID, ...);
	jint	(*CallIntMethodV)		(JNIEnv*, jobject, jmethodID, va_list);
	jint	(*CallIntMethodA)		(JNIEnv*, jobject, jmethodID, jvalue*);
	jlong	(*CallLongMethod)		(JNIEnv*, jobject, jmethodID, ...);
	jlong	(*CallLongMethodV)		(JNIEnv*, jobject, jmethodID, va_list);
	jlong	(*CallLongMethodA)		(JNIEnv*, jobject, jmethodID, jvalue*);
	jfloat	(*CallFloatMethod)		(JNIEnv*, jobject, jmethodID, ...);
	jfloat	(*CallFloatMethodV)		(JNIEnv*, jobject, jmethodID, va_list);
	jfloat	(*CallFloatMethodA)		(JNIEnv*, jobject, jmethodID, jvalue*);
	jdouble	(*CallDoubleMethod)		(JNIEnv*, jobject, jmethodID, ...);
	jdouble	(*CallDoubleMethodV)		(JNIEnv*, jobject, jmethodID, va_list);
	jdouble	(*CallDoubleMethodA)		(JNIEnv*, jobject, jmethodID, jvalue*);
	void	(*CallVoidMethod)		(JNIEnv*, jobject, jmethodID, ...);
	void	(*CallVoidMethodV)		(JNIEnv*, jobject, jmethodID, va_list);
	void	(*CallVoidMethodA)		(JNIEnv*, jobject, jmethodID, jvalue*);
	jobject	(*CallNonvirtualObjectMethod)	(JNIEnv*, jobject, jclass, jmethodID, ...);
	jobject	(*CallNonvirtualObjectMethodV)	(JNIEnv*, jobject, jclass, jmethodID, va_list);
	jobject	(*CallNonvirtualObjectMethodA)	(JNIEnv*, jobject, jclass, jmethodID, jvalue*);
	jboolean	(*CallNonvirtualBooleanMethod)	(JNIEnv*, jobject, jclass, jmethodID, ...);
	jboolean	(*CallNonvirtualBooleanMethodV)	(JNIEnv*, jobject, jclass, jmethodID, va_list);
	jboolean	(*CallNonvirtualBooleanMethodA)	(JNIEnv*, jobject, jclass, jmethodID, jvalue*);
	jbyte	(*CallNonvirtualByteMethod)	(JNIEnv*, jobject, jclass, jmethodID, ...);
	jbyte	(*CallNonvirtualByteMethodV)	(JNIEnv*, jobject, jclass, jmethodID, va_list);
	jbyte	(*CallNonvirtualByteMethodA)	(JNIEnv*, jobject, jclass, jmethodID, jvalue*);
	jchar	(*CallNonvirtualCharMethod)	(JNIEnv*, jobject, jclass, jmethodID, ...);
	jchar	(*CallNonvirtualCharMethodV)	(JNIEnv*, jobject, jclass, jmethodID, va_list);
	jchar	(*CallNonvirtualCharMethodA)	(JNIEnv*, jobject, jclass, jmethodID, jvalue*);
	jshort	(*CallNonvirtualShortMethod)	(JNIEnv*, jobject, jclass, jmethodID, ...);
	jshort	(*CallNonvirtualShortMethodV)	(JNIEnv*, jobject, jclass, jmethodID, va_list);
	jshort	(*CallNonvirtualShortMethodA)	(JNIEnv*, jobject, jclass, jmethodID, jvalue*);
	jint	(*CallNonvirtualIntMethod)	(JNIEnv*, jobject, jclass, jmethodID, ...);
	jint	(*CallNonvirtualIntMethodV)	(JNIEnv*, jobject, jclass, jmethodID, va_list);
	jint	(*CallNonvirtualIntMethodA)	(JNIEnv*, jobject, jclass, jmethodID, jvalue*);
	jlong	(*CallNonvirtualLongMethod)	(JNIEnv*, jobject, jclass, jmethodID, ...);
	jlong	(*CallNonvirtualLongMethodV)	(JNIEnv*, jobject, jclass, jmethodID, va_list);
	jlong	(*CallNonvirtualLongMethodA)	(JNIEnv*, jobject, jclass, jmethodID, jvalue*);
	jfloat	(*CallNonvirtualFloatMethod)	(JNIEnv*, jobject, jclass, jmethodID, ...);
	jfloat	(*CallNonvirtualFloatMethodV)	(JNIEnv*, jobject, jclass, jmethodID, va_list);
	jfloat	(*CallNonvirtualFloatMethodA)	(JNIEnv*, jobject, jclass, jmethodID, jvalue*);
	jdouble	(*CallNonvirtualDoubleMethod)	(JNIEnv*, jobject, jclass, jmethodID, ...);
	jdouble	(*CallNonvirtualDoubleMethodV)	(JNIEnv*, jobject, jclass, jmethodID, va_list);
	jdouble	(*CallNonvirtualDoubleMethodA)	(JNIEnv*, jobject, jclass, jmethodID, jvalue*);
	void	(*CallNonvirtualVoidMethod)	(JNIEnv*, jobject, jclass, jmethodID, ...);
	void	(*CallNonvirtualVoidMethodV)	(JNIEnv*, jobject, jclass, jmethodID, va_list);
	void	(*CallNonvirtualVoidMethodA)	(JNIEnv*, jobject, jclass, jmethodID, jvalue*);
	jfieldID (*GetFieldID)			(JNIEnv*, jclass, const char*, const char*);
	jobject	(*GetObjectField)		(JNIEnv*, jobject, jfieldID);
	jboolean	(*GetBooleanField)		(JNIEnv*, jobject, jfieldID);
	jbyte	(*GetByteField)			(JNIEnv*, jobject, jfieldID);
	jchar	(*GetCharField)			(JNIEnv*, jobject, jfieldID);
	jshort	(*GetShortField)		(JNIEnv*, jobject, jfieldID);
	jint	(*GetIntField)			(JNIEnv*, jobject, jfieldID);
	jlong	(*GetLongField)			(JNIEnv*, jobject, jfieldID);
	jfloat	(*GetFloatField)		(JNIEnv*, jobject, jfieldID);
	jdouble	(*GetDoubleField)		(JNIEnv*, jobject, jfieldID);
	void	(*SetObjectField)		(JNIEnv*, jobject, jfieldID, jobject);
	void	(*SetBooleanField)		(JNIEnv*, jobject, jfieldID, jboolean);
	void	(*SetByteField)			(JNIEnv*, jobject, jfieldID, jbyte);
	void	(*SetCharField)			(JNIEnv*, jobject, jfieldID, jchar);
	void	(*SetShortField)		(JNIEnv*, jobject, jfieldID, jshort);
	void	(*SetIntField)			(JNIEnv*, jobject, jfieldID, jint);
	void	(*SetLongField)			(JNIEnv*, jobject, jfieldID, jlong);
	void	(*SetFloatField)		(JNIEnv*, jobject, jfieldID, jfloat);
	void	(*SetDoubleField)		(JNIEnv*, jobject, jfieldID, jdouble);
	jmethodID (*GetStaticMethodID)		(JNIEnv*, jclass, const char*, const char*);
	jobject	(*CallStaticObjectMethod)	(JNIEnv*, jclass, jmethodID, ...);
	jobject	(*CallStaticObjectMethodV)	(JNIEnv*, jclass, jmethodID, va_list);
	jobject	(*CallStaticObjectMethodA)	(JNIEnv*, jclass, jmethodID, jvalue*);
	jboolean	(*CallStaticBooleanMethod)	(JNIEnv*, jclass, jmethodID, ...);
	jboolean	(*CallStaticBooleanMethodV)	(JNIEnv*, jclass, jmethodID, va_list);
	jboolean	(*CallStaticBooleanMethodA)	(JNIEnv*, jclass, jmethodID, jvalue*);
	jbyte	(*CallStaticByteMethod)		(JNIEnv*, jclass, jmethodID, ...);
	jbyte	(*CallStaticByteMethodV)	(JNIEnv*, jclass, jmethodID, va_list);
	jbyte	(*CallStaticByteMethodA)	(JNIEnv*, jclass, jmethodID, jvalue*);
	jchar	(*CallStaticCharMethod)		(JNIEnv*, jclass, jmethodID, ...);
	jchar	(*CallStaticCharMethodV)	(JNIEnv*, jclass, jmethodID, va_list);
	jchar	(*CallStaticCharMethodA)	(JNIEnv*, jclass, jmethodID, jvalue*);
	jshort	(*CallStaticShortMethod)	(JNIEnv*, jclass, jmethodID, ...);
	jshort	(*CallStaticShortMethodV)	(JNIEnv*, jclass, jmethodID, va_list);
	jshort	(*CallStaticShortMethodA)	(JNIEnv*, jclass, jmethodID, jvalue*);
	jint	(*CallStaticIntMethod)		(JNIEnv*, jclass, jmethodID, ...);
	jint	(*CallStaticIntMethodV)		(JNIEnv*, jclass, jmethodID, va_list);
	jint	(*CallStaticIntMethodA)		(JNIEnv*, jclass, jmethodID, jvalue*);
	jlong	(*CallStaticLongMethod)		(JNIEnv*, jclass, jmethodID, ...);
	jlong	(*CallStaticLongMethodV)	(JNIEnv*, jclass, jmethodID, va_list);
	jlong	(*CallStaticLongMethodA)	(JNIEnv*, jclass, jmethodID, jvalue*);
	jfloat	(*CallStaticFloatMethod)	(JNIEnv*, jclass, jmethodID, ...);
	jfloat	(*CallStaticFloatMethodV)	(JNIEnv*, jclass, jmethodID, va_list);
	jfloat	(*CallStaticFloatMethodA)	(JNIEnv*, jclass, jmethodID, jvalue*);
	jdouble	(*CallStaticDoubleMethod)	(JNIEnv*, jclass, jmethodID, ...);
	jdouble	(*CallStaticDoubleMethodV)	(JNIEnv*, jclass, jmethodID, va_list);
	jdouble	(*CallStaticDoubleMethodA)	(JNIEnv*, jclass, jmethodID, jvalue*);
	void	(*CallStaticVoidMethod)		(JNIEnv*, jclass, jmethodID, ...);
	void	(*CallStaticVoidMethodV)	(JNIEnv*, jclass, jmethodID, va_list);
	void	(*CallStaticVoidMethodA)	(JNIEnv*, jclass, jmethodID, jvalue*);
	jfieldID (*GetStaticFieldID)		(JNIEnv*, jclass, const char*, const char*);
	jobject	(*GetStaticObjectField)		(JNIEnv*, jclass, jfieldID);
	jboolean	(*GetStaticBooleanField)	(JNIEnv*, jclass, jfieldID);
	jbyte	(*GetStaticByteField)		(JNIEnv*, jclass, jfieldID);
	jchar	(*GetStaticCharField)		(JNIEnv*, jclass, jfieldID);
	jshort	(*GetStaticShortField)		(JNIEnv*, jclass, jfieldID);
	jint	(*GetStaticIntField)		(JNIEnv*, jclass, jfieldID);
	jlong	(*GetStaticLongField)		(JNIEnv*, jclass, jfieldID);
	jfloat	(*GetStaticFloatField)		(JNIEnv*, jclass, jfieldID);
	jdouble	(*GetStaticDoubleField)		(JNIEnv*, jclass, jfieldID);
	void	(*SetStaticObjectField)		(JNIEnv*, jclass, jfieldID, jobject);
	void	(*SetStaticBooleanField)	(JNIEnv*, jclass, jfieldID, jboolean);
	void	(*SetStaticByteField)		(JNIEnv*, jclass, jfieldID, jbyte);
	void	(*SetStaticCharField)		(JNIEnv*, jclass, jfieldID, jchar);
	void	(*SetStaticShortField)		(JNIEnv*, jclass, jfieldID, jshort);
	void	(*SetStaticIntField)		(JNIEnv*, jclass, jfieldID, jint);
	void	(*SetStaticLongField)		(JNIEnv*, jclass, jfieldID, jlong);
	void	(*SetStaticFloatField)		(JNIEnv*, jclass, jfieldID, jfloat);
	void	(*SetStaticDoubleField)		(JNIEnv*, jclass, jfieldID, jdouble);
	jstring	(*NewString)			(JNIEnv*, const jchar*, jsize);
	jsize	(*GetStringLength)		(JNIEnv*, jstring);
	const jchar* (*GetStringChars)		(JNIEnv*, jstring, jboolean*);
	void	(*ReleaseStringChars)		(JNIEnv*, jstring, const jchar*);
	jstring	(*NewStringUTF)			(JNIEnv*, const char*);
	jsize	(*GetStringUTFLength)		(JNIEnv*, jstring);
	const jbyte* (*GetStringUTFChars)	(JNIEnv*, jstring, jboolean*);
	void	(*ReleaseStringUTFChars)	(JNIEnv*, jstring, const jbyte*);
	jsize	(*GetArrayLength)		(JNIEnv*, jarray);
	jarray	(*NewObjectArray)		(JNIEnv*, jsize, jclass, jobject);
	jobject	(*GetObjectArrayElement)	(JNIEnv*, jarray, jsize);
	void	(*SetObjectArrayElement)	(JNIEnv*, jarray, jsize, jobject);
	jarray	(*NewBooleanArray)		(JNIEnv*, jsize);
	jarray	(*NewByteArray)			(JNIEnv*, jsize);
	jarray	(*NewCharArray)			(JNIEnv*, jsize);
	jarray	(*NewShortArray)		(JNIEnv*, jsize);
	jarray	(*NewIntArray)			(JNIEnv*, jsize);
	jarray	(*NewLongArray)			(JNIEnv*, jsize);
	jarray	(*NewFloatArray)		(JNIEnv*, jsize);
	jarray	(*NewDoubleArray)		(JNIEnv*, jsize);
	jboolean*	(*GetBooleanArrayElements)	(JNIEnv*, jarray, jboolean*);
	jbyte*	(*GetByteArrayElements)		(JNIEnv*, jarray, jboolean*);
	jchar*	(*GetCharArrayElements)		(JNIEnv*, jarray, jboolean*);
	jshort*	(*GetShortArrayElements)	(JNIEnv*, jarray, jboolean*);
	jint*	(*GetIntArrayElements)		(JNIEnv*, jarray, jboolean*);
	jlong*	(*GetLongArrayElements)		(JNIEnv*, jarray, jboolean*);
	jfloat*	(*GetFloatArrayElements)	(JNIEnv*, jarray, jboolean*);
	jdouble* (*GetDoubleArrayElements)	(JNIEnv*, jarray, jboolean*);
	void	(*ReleaseBooleanArrayElements)	(JNIEnv*, jarray, jboolean*, jint);
	void	(*ReleaseByteArrayElements)	(JNIEnv*, jarray, jbyte*, jint);
	void	(*ReleaseCharArrayElements)	(JNIEnv*, jarray, jchar*, jint);
	void	(*ReleaseShortArrayElements)	(JNIEnv*, jarray, jshort*, jint);
	void	(*ReleaseIntArrayElements)	(JNIEnv*, jarray, jint*, jint);
	void	(*ReleaseLongArrayElements)	(JNIEnv*, jarray, jlong*, jint);
	void	(*ReleaseFloatArrayElements)	(JNIEnv*, jarray, jfloat*, jint);
	void	(*ReleaseDoubleArrayElements)	(JNIEnv*, jarray, jdouble*, jint);
	void	(*GetBooleanArrayRegion)	(JNIEnv*, jarray, jsize, jsize, jboolean*);
	void	(*GetByteArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jbyte*);
	void	(*GetCharArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jchar*);
	void	(*GetShortArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jshort*);
	void	(*GetIntArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jint*);
	void	(*GetLongArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jlong*);
	void	(*GetFloatArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jfloat*);
	void	(*GetDoubleArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jdouble*);
	void	(*SetBooleanArrayRegion)	(JNIEnv*, jarray, jsize, jsize, jboolean*);
	void	(*SetByteArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jbyte*);
	void	(*SetCharArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jchar*);
	void	(*SetShortArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jshort*);
	void	(*SetIntArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jint*);
	void	(*SetLongArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jlong*);
	void	(*SetFloatArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jfloat*);
	void	(*SetDoubleArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jdouble*);
	jint	(*RegisterNatives)		(JNIEnv*, jclass, const JNINativeMethod*, jint);
	jint	(*UnregisterNatives)		(JNIEnv*, jclass);
	jint	(*MonitorEnter)			(JNIEnv*, jobject);
	jint	(*MonitorExit)			(JNIEnv*, jobject);
	jint	(*GetJavaVM)			(JNIEnv*, JavaVM**);

};

struct JavaVMAttachArgs {
	void*	reserved0;
};

struct JNINativeMethod_ {
	char*	name;
	char*	signature;
	void*	fnPtr;
};

struct JNIInvokeInterface {
	void*	reserved0;
	void*	reserved1;
	void*	reserved2;
	jint	(*DestroyJavaVM)		(JavaVM*);
	jint	(*AttachCurrentThread)		(JavaVM*, JNIEnv**, ThreadAttachArgs*);
	jint	(*DetachCurrentThread)		(JavaVM*);
};

struct JNIEnv_ {
	const struct JNINativeInterface*	functions;
#if defined(__cplusplus)
	jint GetVersion(void);
	jclass DefineClass(jobject, const jbyte*, jsize);
	jclass FindClass(const char*);
	jclass GetSuperClass(jclass);
	jboolean IsAssignableFrom(jclass, jclass);
	jint Throw(jobject);
	jint ThrowNew(jclass, const char*);
	jthrowable ExceptionOccured(void);
	void ExceptionDescribe(void);
	void ExceptionClear(void);
	void FatalError(const char*);
	jref NewGlobalRef(jref);
	void DeleteGlobalRef(jref);
	void DeleteLocalRef(jref);
	jboolean IsSameObject(jobject, jobject);
	jobject AllocObject(jclass);
	jobject NewObject(jclass, jmethodID, ...);
	jobject NewObjectV(jclass, jmethodID, va_list);
	jobject NewObjectA(jclass, jmethodID, jvalue*);
	jclass GetObjectClass(jobject);
	jboolean IsInstanceOf(jobject, jclass);
	jmethodID GetMethodID(jclass, const char*, const char*);
	jobject CallObjectMethod(jobject, jmethodID, ...);
	jobject CallObjectMethodV(jobject, jmethodID, va_list);
	jobject CallObjectMethodA(jobject, jmethodID, jvalue*);
	jboolean CallBooleanMethod(jobject, jmethodID, ...);
	jboolean CallBooleanMethodV(jobject, jmethodID, va_list);
	jboolean CallBooleanMethodA(jobject, jmethodID, jvalue*);
	jbyte CallByteMethod(jobject, jmethodID, ...);
	jbyte CallByteMethodV(jobject, jmethodID, va_list);
	jbyte CallByteMethodA(jobject, jmethodID, jvalue*);
	jchar CallCharMethod(jobject, jmethodID, ...);
	jchar CallCharMethodV(jobject, jmethodID, va_list);
	jchar CallCharMethodA(jobject, jmethodID, jvalue*);
	jshort CallShortMethod(jobject, jmethodID, ...);
	jshort CallShortMethodV(jobject, jmethodID, va_list);
	jshort CallShortMethodA(jobject, jmethodID, jvalue*);
	jint CallIntMethod(jobject, jmethodID, ...);
	jint CallIntMethodV(jobject, jmethodID, va_list);
	jint CallIntMethodA(jobject, jmethodID, jvalue*);
	jlong CallLongMethod(jobject, jmethodID, ...);
	jlong CallLongMethodV(jobject, jmethodID, va_list);
	jlong CallLongMethodA(jobject, jmethodID, jvalue*);
	jfloat CallFloatMethod(jobject, jmethodID, ...);
	jfloat CallFloatMethodV(jobject, jmethodID, va_list);
	jfloat CallFloatMethodA(jobject, jmethodID, jvalue*);
	jdouble CallDoubleMethod(jobject, jmethodID, ...);
	jdouble CallDoubleMethodV(jobject, jmethodID, va_list);
	jdouble CallDoubleMethodA(jobject, jmethodID, jvalue*);
	void CallVoidMethod(jobject, jmethodID, ...);
	void CallVoidMethodV(jobject, jmethodID, va_list);
	void CallVoidMethodA(jobject, jmethodID, jvalue*);
	jobject CallNonvirtualObjectMethod(jobject, jclass, jmethodID, ...);
	jobject CallNonvirtualObjectMethodV(jobject, jclass, jmethodID, va_list);
	jobject CallNonvirtualObjectMethodA(jobject, jclass, jmethodID, jvalue*);
	jboolean CallNonvirtualBooleanMethod(jobject, jclass, jmethodID, ...);
	jboolean CallNonvirtualBooleanMethodV(jobject, jclass, jmethodID, va_list);
	jboolean CallNonvirtualBooleanMethodA(jobject, jclass, jmethodID, jvalue*);
	jbyte CallNonvirtualByteMethod(jobject, jclass, jmethodID, ...);
	jbyte CallNonvirtualByteMethodV(jobject, jclass, jmethodID, va_list);
	jbyte CallNonvirtualByteMethodA(jobject, jclass, jmethodID, jvalue*);
	jchar CallNonvirtualCharMethod(jobject, jclass, jmethodID, ...);
	jchar CallNonvirtualCharMethodV(jobject, jclass, jmethodID, va_list);
	jchar CallNonvirtualCharMethodA(jobject, jclass, jmethodID, jvalue*);
	jshort CallNonvirtualShortMethod(jobject, jclass, jmethodID, ...);
	jshort CallNonvirtualShortMethodV(jobject, jclass, jmethodID, va_list);
	jshort CallNonvirtualShortMethodA(jobject, jclass, jmethodID, jvalue*);
	jint CallNonvirtualIntMethod(jobject, jclass, jmethodID, ...);
	jint CallNonvirtualIntMethodV(jobject, jclass, jmethodID, va_list);
	jint CallNonvirtualIntMethodA(jobject, jclass, jmethodID, jvalue*);
	jlong CallNonvirtualLongMethod(jobject, jclass, jmethodID, ...);
	jlong CallNonvirtualLongMethodV(jobject, jclass, jmethodID, va_list);
	jlong CallNonvirtualLongMethodA(jobject, jclass, jmethodID, jvalue*);
	jfloat CallNonvirtualFloatMethod(jobject, jclass, jmethodID, ...);
	jfloat CallNonvirtualFloatMethodV(jobject, jclass, jmethodID, va_list);
	jfloat CallNonvirtualFloatMethodA(jobject, jclass, jmethodID, jvalue*);
	jdouble CallNonvirtualDoubleMethod(jobject, jclass, jmethodID, ...);
	jdouble CallNonvirtualDoubleMethodV(jobject, jclass, jmethodID, va_list);
	jdouble CallNonvirtualDoubleMethodA(jobject, jclass, jmethodID, jvalue*);
	void CallNonvirtualVoidMethod(jobject, jclass, jmethodID, ...);
	void CallNonvirtualVoidMethodV(jobject, jclass, jmethodID, va_list);
	void CallNonvirtualVoidMethodA(jobject, jclass, jmethodID, jvalue*);
	jfieldID GetFieldID(jclass, const char*, const char*);
	jobject GetObjectField(jobject, jfieldID);
	jboolean GetBooleanField(jobject, jfieldID);
	jbyte GetByteField(jobject, jfieldID);
	jchar GetCharField(jobject, jfieldID);
	jshort GetShortField(jobject, jfieldID);
	jint GetIntField(jobject, jfieldID);
	jlong GetLongField(jobject, jfieldID);
	jfloat GetFloatField(jobject, jfieldID);
	jdouble GetDoubleField(jobject, jfieldID);
	void SetObjectField(jobject, jfieldID, jobject);
	void SetBooleanField(jobject, jfieldID, jboolean);
	void SetByteField(jobject, jfieldID, jbyte);
	void SetCharField(jobject, jfieldID, jchar);
	void SetShortField(jobject, jfieldID, jshort);
	void SetIntField(jobject, jfieldID, jint);
	void SetLongField(jobject, jfieldID, jlong);
	void SetFloatField(jobject, jfieldID, jfloat);
	void SetDoubleField(jobject, jfieldID, jdouble);
	jmethodID GetStaticMethodID(jclass, const char*, const char*);
	jobject CallStaticObjectMethod(jclass, jmethodID, ...);
	jobject CallStaticObjectMethodV(jclass, jmethodID, va_list);
	jobject CallStaticObjectMethodA(jclass, jmethodID, jvalue*);
	jboolean CallStaticBooleanMethod(jclass, jmethodID, ...);
	jboolean CallStaticBooleanMethodV(jclass, jmethodID, va_list);
	jboolean CallStaticBooleanMethodA(jclass, jmethodID, jvalue*);
	jbyte CallStaticByteMethod(jclass, jmethodID, ...);
	jbyte CallStaticByteMethodV(jclass, jmethodID, va_list);
	jbyte CallStaticByteMethodA(jclass, jmethodID, jvalue*);
	jchar CallStaticCharMethod(jclass, jmethodID, ...);
	jchar CallStaticCharMethodV(jclass, jmethodID, va_list);
	jchar CallStaticCharMethodA(jclass, jmethodID, jvalue*);
	jshort CallStaticShortMethod(jclass, jmethodID, ...);
	jshort CallStaticShortMethodV(jclass, jmethodID, va_list);
	jshort CallStaticShortMethodA(jclass, jmethodID, jvalue*);
	jint CallStaticIntMethod(jclass, jmethodID, ...);
	jint CallStaticIntMethodV(jclass, jmethodID, va_list);
	jint CallStaticIntMethodA(jclass, jmethodID, jvalue*);
	jlong CallStaticLongMethod(jclass, jmethodID, ...);
	jlong CallStaticLongMethodV(jclass, jmethodID, va_list);
	jlong CallStaticLongMethodA(jclass, jmethodID, jvalue*);
	jfloat CallStaticFloatMethod(jclass, jmethodID, ...);
	jfloat CallStaticFloatMethodV(jclass, jmethodID, va_list);
	jfloat CallStaticFloatMethodA(jclass, jmethodID, jvalue*);
	jdouble CallStaticDoubleMethod(jclass, jmethodID, ...);
	jdouble CallStaticDoubleMethodV(jclass, jmethodID, va_list);
	jdouble CallStaticDoubleMethodA(jclass, jmethodID, jvalue*);
	void CallStaticVoidMethod(jclass, jmethodID, ...);
	void CallStaticVoidMethodV(jclass, jmethodID, va_list);
	void CallStaticVoidMethodA(jclass, jmethodID, jvalue*);
	jfieldID GetStaticFieldID(jclass, const char*, const char*);
	jobject GetStaticObjectField(jclass, jfieldID);
	jboolean GetStaticBooleanField(jclass, jfieldID);
	jbyte GetStaticByteField(jclass, jfieldID);
	jchar GetStaticCharField(jclass, jfieldID);
	jshort GetStaticShortField(jclass, jfieldID);
	jint GetStaticIntField(jclass, jfieldID);
	jlong GetStaticLongField(jclass, jfieldID);
	jfloat GetStaticFloatField(jclass, jfieldID);
	jdouble GetStaticDoubleField(jclass, jfieldID);
	void SetStaticObjectField(jclass, jfieldID, jobject);
	void SetStaticBooleanField(jclass, jfieldID, jboolean);
	void SetStaticByteField(jclass, jfieldID, jbyte);
	void SetStaticCharField(jclass, jfieldID, jchar);
	void SetStaticShortField(jclass, jfieldID, jshort);
	void SetStaticIntField(jclass, jfieldID, jint);
	void SetStaticLongField(jclass, jfieldID, jlong);
	void SetStaticFloatField(jclass, jfieldID, jfloat);
	void SetStaticDoubleField(jclass, jfieldID, jdouble);
	jstring NewString(const jchar*, jsize);
	jsize GetStringLength(jstring);
	const jchar* GetStringChars(jstring, jboolean*);
	void ReleaseStringChars(jstring, const jchar*);
	jstring NewStringUTF(const char*);
	jsize GetStringUTFLength(jstring);
	const jbyte* GetStringUTFChars(jstring, jboolean*);
	void ReleaseStringUTFChars(jstring, const jbyte*);
	jsize GetArrayLength(jarray);
	jarray NewObjectArray(jsize, jclass, jobject);
	jobject GetObjectArrayElement(jarray, jsize);
	void SetObjectArrayElement(jarray, jsize, jobject);
	jarray NewBooleanArray(jsize);
	jarray NewByteArray(jsize);
	jarray NewCharArray(jsize);
	jarray NewShortArray(jsize);
	jarray NewIntArray(jsize);
	jarray NewLongArray(jsize);
	jarray NewFloatArray(jsize);
	jarray NewDoubleArray(jsize);
	jboolean* GetBooleanArrayElements(jarray, jboolean*);
	jbyte* GetByteArrayElements(jarray, jboolean*);
	jchar* GetCharArrayElements(jarray, jboolean*);
	jshort* GetShortArrayElements(jarray, jboolean*);
	jint* GetIntArrayElements(jarray, jboolean*);
	jlong* GetLongArrayElements(jarray, jboolean*);
	jfloat* GetFloatArrayElements(jarray, jboolean*);
	jdouble* GetDoubleArrayElements(jarray, jboolean*);
	void ReleaseBooleanArrayElements(jarray, jboolean*, jint);
	void ReleaseByteArrayElements(jarray, jbyte*, jint);
	void ReleaseCharArrayElements(jarray, jchar*, jint);
	void ReleaseShortArrayElements(jarray, jshort*, jint);
	void ReleaseIntArrayElements(jarray, jint*, jint);
	void ReleaseLongArrayElements(jarray, jlong*, jint);
	void ReleaseFloatArrayElements(jarray, jfloat*, jint);
	void ReleaseDoubleArrayElements(jarray, jdouble*, jint);
	void GetBooleanArrayRegion(jarray, jsize, jsize, jboolean*);
	void GetByteArrayRegion(jarray, jsize, jsize, jbyte*);
	void GetCharArrayRegion(jarray, jsize, jsize, jchar*);
	void GetShortArrayRegion(jarray, jsize, jsize, jshort*);
	void GetIntArrayRegion(jarray, jsize, jsize, jint*);
	void GetLongArrayRegion(jarray, jsize, jsize, jlong*);
	void GetFloatArrayRegion(jarray, jsize, jsize, jfloat*);
	void GetDoubleArrayRegion(jarray, jsize, jsize, jdouble*);
	void SetBooleanArrayRegion(jarray, jsize, jsize, jboolean*);
	void SetByteArrayRegion(jarray, jsize, jsize, jbyte*);
	void SetCharArrayRegion(jarray, jsize, jsize, jchar*);
	void SetShortArrayRegion(jarray, jsize, jsize, jshort*);
	void SetIntArrayRegion(jarray, jsize, jsize, jint*);
	void SetLongArrayRegion(jarray, jsize, jsize, jlong*);
	void SetFloatArrayRegion(jarray, jsize, jsize, jfloat*);
	void SetDoubleArrayRegion(jarray, jsize, jsize, jdouble*);
	jint RegisterNatives(jclass, const JNINativeMethod*, jint);
	jint UnregisterNatives(jclass);
	jint MonitorEnter(jobject);
	jint MonitorExit(jobject);
	jint GetJavaVM(JavaVM**);
#endif
};

struct JavaVM_ {
	const struct JNIInvokeInterface*	functions;
};

struct JavaVMInitArgs {
	jint		version;
	char**		properties;
	jint		checkSource;
	jint		nativeStackSize;
	jint		javaStackSize;
	jint		minHeapSize;
	jint		maxHeapSize;
	jint		verifyMode;
	const char*	classpath;
	jint		(*vfprintf)(FILE*, const char*, va_list);
	void		(*exit)(jint);
	void		(*abort)(void);
	jint		enableClassGC;
	jint		enableVerboseGC;
	jint		disableAsyncGC;
	jint		enableVerboseClassloading;
	jint		enableVerboseJIT;
	jint		enableVerboseCall;
	jint		allocHeapSize;
	const char*	classhome;
	const char*	libraryhome;
};

extern JavaVMInitArgs Kaffe_JavaVMArgs[];

extern jint	JNI_GetDefaultJavaVMInitArgs(JavaVMInitArgs*);
extern jint	JNI_CreateJavaVM(JavaVM**, JNIEnv**, JavaVMInitArgs*);
extern jint	JNI_GetCreatedJavaVMs(JavaVM**, jsize, jsize*);

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
#include "jni_cpp.h"
#endif

#endif
