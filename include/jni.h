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
#include <sys/types.h>
#include "kaffe/jtypes.h"
#include "kaffe/jmalloc.h"

#undef  __NORETURN__
#if defined(__GNUC__)
#define __NORETURN__ __attribute__((noreturn))
#else
#define __NORETURN__
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#define	JNIEXPORT extern
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

typedef	void*	jobjectID;
typedef	void*	jmethodID;
typedef	void*	jfieldID;

#define	JNI_TRUE	1
#define	JNI_FALSE	0

#define	JNI_COMMIT	1
#define	JNI_ABORT	2

/* JNI return codes */
#define JNI_OK		0
#define JNI_ERR		(-1)
#define JNI_EDETACHED	(-2)
#define JNI_EVERSION	(-3)

struct JNINativeInterface {

	void*	reserved0;
	void*	reserved1;
	void*	reserved2;
	void*	reserved3;
	jint	(*GetVersion)			(JNIEnv*);
	jclass	(*DefineClass)			(JNIEnv*, jobject, const jbyte*, jsize);
	jclass	(*FindClass)			(JNIEnv*, const char*);
        void*	reserved4; /* FromReflectedMethod */
        void*	reserved5; /* FromReflectedField */
        void*	reserved6; /* ToReflectedMethod */
	jclass	(*GetSuperclass)		(JNIEnv*, jclass);
	jboolean	(*IsAssignableFrom)		(JNIEnv*, jclass, jclass);
        void*	reserved7; /* ToReflectedField */
	jint	(*Throw)			(JNIEnv*, jobject);
	jint	(*ThrowNew)			(JNIEnv*, jclass, const char*);
	jthrowable (*ExceptionOccurred)		(JNIEnv*);
	void	(*ExceptionDescribe)		(JNIEnv*);
	void	(*ExceptionClear)		(JNIEnv*);
	void	(*FatalError)			(JNIEnv*, const char*);
        void*	reserved8; /* PushLocalFrame */
        void*	reserved9; /* PopLocalFrame */
	jref	(*NewGlobalRef)			(JNIEnv*, jref);
	void	(*DeleteGlobalRef)		(JNIEnv*, jref);
	void	(*DeleteLocalRef)		(JNIEnv*, jref);
	jboolean	(*IsSameObject)			(JNIEnv*, jobject, jobject);
        void*	reserved10; /* NewLocalRef */
        void*	reserved11; /* EnsureLocalCapacity */
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
	jobjectArray	(*NewObjectArray)	(JNIEnv*, jsize, jclass, jobject);
	jobject	(*GetObjectArrayElement)	(JNIEnv*, jobjectArray, jsize);
	void	(*SetObjectArrayElement)	(JNIEnv*, jobjectArray, jsize, jobject);
	jbooleanArray	(*NewBooleanArray)		(JNIEnv*, jsize);
	jbyteArray	(*NewByteArray)			(JNIEnv*, jsize);
	jcharArray	(*NewCharArray)			(JNIEnv*, jsize);
	jshortArray	(*NewShortArray)		(JNIEnv*, jsize);
	jintArray	(*NewIntArray)			(JNIEnv*, jsize);
	jlongArray	(*NewLongArray)			(JNIEnv*, jsize);
	jfloatArray	(*NewFloatArray)		(JNIEnv*, jsize);
	jdoubleArray	(*NewDoubleArray)		(JNIEnv*, jsize);
	jboolean* (*GetBooleanArrayElements)	(JNIEnv*, jbooleanArray, jboolean*);
	jbyte*	(*GetByteArrayElements)		(JNIEnv*, jbyteArray, jboolean*);
	jchar*	(*GetCharArrayElements)		(JNIEnv*, jcharArray, jboolean*);
	jshort*	(*GetShortArrayElements)	(JNIEnv*, jshortArray, jboolean*);
	jint*	(*GetIntArrayElements)		(JNIEnv*, jintArray, jboolean*);
	jlong*	(*GetLongArrayElements)		(JNIEnv*, jlongArray, jboolean*);
	jfloat*	(*GetFloatArrayElements)	(JNIEnv*, jfloatArray, jboolean*);
	jdouble* (*GetDoubleArrayElements)	(JNIEnv*, jdoubleArray, jboolean*);
	void	(*ReleaseBooleanArrayElements)	(JNIEnv*, jbooleanArray, jboolean*, jint);
	void	(*ReleaseByteArrayElements)	(JNIEnv*, jbyteArray, jbyte*, jint);
	void	(*ReleaseCharArrayElements)	(JNIEnv*, jcharArray, jchar*, jint);
	void	(*ReleaseShortArrayElements)	(JNIEnv*, jshortArray, jshort*, jint);
	void	(*ReleaseIntArrayElements)	(JNIEnv*, jintArray, jint*, jint);
	void	(*ReleaseLongArrayElements)	(JNIEnv*, jlongArray, jlong*, jint);
	void	(*ReleaseFloatArrayElements)	(JNIEnv*, jfloatArray, jfloat*, jint);
	void	(*ReleaseDoubleArrayElements)	(JNIEnv*, jdoubleArray, jdouble*, jint);
	void	(*GetBooleanArrayRegion)	(JNIEnv*, jbooleanArray, jsize, jsize, jboolean*);
	void	(*GetByteArrayRegion)		(JNIEnv*, jbyteArray, jsize, jsize, jbyte*);
	void	(*GetCharArrayRegion)		(JNIEnv*, jcharArray, jsize, jsize, jchar*);
	void	(*GetShortArrayRegion)		(JNIEnv*, jshortArray, jsize, jsize, jshort*);
	void	(*GetIntArrayRegion)		(JNIEnv*, jintArray, jsize, jsize, jint*);
	void	(*GetLongArrayRegion)		(JNIEnv*, jlongArray, jsize, jsize, jlong*);
	void	(*GetFloatArrayRegion)		(JNIEnv*, jfloatArray, jsize, jsize, jfloat*);
	void	(*GetDoubleArrayRegion)		(JNIEnv*, jdoubleArray, jsize, jsize, jdouble*);
	void	(*SetBooleanArrayRegion)	(JNIEnv*, jbooleanArray, jsize, jsize, jboolean*);
	void	(*SetByteArrayRegion)		(JNIEnv*, jbyteArray, jsize, jsize, jbyte*);
	void	(*SetCharArrayRegion)		(JNIEnv*, jcharArray, jsize, jsize, jchar*);
	void	(*SetShortArrayRegion)		(JNIEnv*, jshortArray, jsize, jsize, jshort*);
	void	(*SetIntArrayRegion)		(JNIEnv*, jintArray, jsize, jsize, jint*);
	void	(*SetLongArrayRegion)		(JNIEnv*, jlongArray, jsize, jsize, jlong*);
	void	(*SetFloatArrayRegion)		(JNIEnv*, jfloatArray, jsize, jsize, jfloat*);
	void	(*SetDoubleArrayRegion)		(JNIEnv*, jdoubleArray, jsize, jsize, jdouble*);
	jint	(*RegisterNatives)		(JNIEnv*, jclass, const JNINativeMethod*, jint);
	jint	(*UnregisterNatives)		(JNIEnv*, jclass);
	jint	(*MonitorEnter)			(JNIEnv*, jobject);
	jint	(*MonitorExit)			(JNIEnv*, jobject);
	jint	(*GetJavaVM)			(JNIEnv*, JavaVM**);

        /* New JNI 1.2 functions */
	
        void* reserved12; /* GetStringRegion */
        void* reserved13; /* GetStringUTFRegion */
	void* (*GetPrimitiveArrayCritical)	(JNIEnv*, jarray, jboolean*);
	void  (*ReleasePrimitiveArrayCritical)	(JNIEnv*, jarray, void*, jint);
        void* reserved16; /* GetStringCritical */
        void* reserved17; /* ReleaseStringCritical */
        void* reserved18; /* NewWeakGlobalRef */
        void* reserved19; /* DeleteWeakGlobalRef */
	jboolean	(*ExceptionCheck)	(JNIEnv*);

        /* New JNI 1.4 functions */
	
        void* reserved20; /* NewDirectByteBuffer */
        void* reserved21; /* GetDirectBufferAddress */
        void* reserved22; /* GetDirectBufferCapacity */
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
	jint	(*AttachCurrentThread)		(JavaVM*, void**, ThreadAttachArgs*);
	jint	(*DetachCurrentThread)		(JavaVM*);
	jint	(*GetEnv)			(JavaVM*, void**, jint);
        void*	reserved3; /* AttachCurrentThreadAsDaemon */
};

struct JNIEnv_ {
	const struct JNINativeInterface*	functions;
#if defined(__cplusplus)
	jint GetVersion(void);
	jclass DefineClass(jobject, const jbyte*, jsize);
	jclass FindClass(const char*);
	jclass GetSuperclass(jclass);
	jboolean IsAssignableFrom(jclass, jclass);
	jint Throw(jobject);
	jint ThrowNew(jclass, const char*);
	jthrowable ExceptionOccurred(void);
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
	jobjectArray NewObjectArray(jsize, jclass, jobject);
	jobject GetObjectArrayElement(jobjectArray, jsize);
	void SetObjectArrayElement(jobjectArray, jsize, jobject);
	jbooleanArray NewBooleanArray(jsize);
	jbyteArray NewByteArray(jsize);
	jcharArray NewCharArray(jsize);
	jshortArray NewShortArray(jsize);
	jintArray NewIntArray(jsize);
	jlongArray NewLongArray(jsize);
	jfloatArray NewFloatArray(jsize);
	jdoubleArray NewDoubleArray(jsize);
	jboolean* GetBooleanArrayElements(jbooleanArray, jboolean*);
	jbyte* GetByteArrayElements(jbyteArray, jboolean*);
	jchar* GetCharArrayElements(jcharArray, jboolean*);
	jshort* GetShortArrayElements(jshortArray, jboolean*);
	jint* GetIntArrayElements(jintArray, jboolean*);
	jlong* GetLongArrayElements(jlongArray, jboolean*);
	jfloat* GetFloatArrayElements(jfloatArray, jboolean*);
	jdouble* GetDoubleArrayElements(jdoubleArray, jboolean*);
	void ReleaseBooleanArrayElements(jbooleanArray, jboolean*, jint);
	void ReleaseByteArrayElements(jbyteArray, jbyte*, jint);
	void ReleaseCharArrayElements(jcharArray, jchar*, jint);
	void ReleaseShortArrayElements(jshortArray, jshort*, jint);
	void ReleaseIntArrayElements(jintArray, jint*, jint);
	void ReleaseLongArrayElements(jlongArray, jlong*, jint);
	void ReleaseFloatArrayElements(jfloatArray, jfloat*, jint);
	void ReleaseDoubleArrayElements(jdoubleArray, jdouble*, jint);
	void GetBooleanArrayRegion(jbooleanArray, jsize, jsize, jboolean*);
	void GetByteArrayRegion(jbyteArray, jsize, jsize, jbyte*);
	void GetCharArrayRegion(jcharArray, jsize, jsize, jchar*);
	void GetShortArrayRegion(jshortArray, jsize, jsize, jshort*);
	void GetIntArrayRegion(jintArray, jsize, jsize, jint*);
	void GetLongArrayRegion(jlongArray, jsize, jsize, jlong*);
	void GetFloatArrayRegion(jfloatArray, jsize, jsize, jfloat*);
	void GetDoubleArrayRegion(jdoubleArray, jsize, jsize, jdouble*);
	void SetBooleanArrayRegion(jbooleanArray, jsize, jsize, jboolean*);
	void SetByteArrayRegion(jbyteArray, jsize, jsize, jbyte*);
	void SetCharArrayRegion(jcharArray, jsize, jsize, jchar*);
	void SetShortArrayRegion(jshortArray, jsize, jsize, jshort*);
	void SetIntArrayRegion(jintArray, jsize, jsize, jint*);
	void SetLongArrayRegion(jlongArray, jsize, jsize, jlong*);
	void SetFloatArrayRegion(jfloatArray, jsize, jsize, jfloat*);
	void SetDoubleArrayRegion(jdoubleArray, jsize, jsize, jdouble*);
	jint RegisterNatives(jclass, const JNINativeMethod*, jint);
	jint UnregisterNatives(jclass);
	jint MonitorEnter(jobject);
	jint MonitorExit(jobject);
	jint GetJavaVM(JavaVM**);
	jboolean ExceptionCheck(void);
#endif
};

struct JavaVM_ {
	const struct JNIInvokeInterface*	functions;
#ifdef __cplusplus
        jint    DestroyJavaVM(void);
        jint    AttachCurrentThread(void**, ThreadAttachArgs*);
        jint    DetachCurrentThread(void);
        jint    GetEnv(void**, jint);
#endif
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
	const char*     bootClasspath;
	jint		(*vfprintf)(FILE*, const char*, va_list);
	void		(*exit)(jint) __NORETURN__;
	void		(*abort)(void) __NORETURN__;
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

#define JNI_VERSION_1_1		0x00010001
#define JNI_VERSION_1_2		0x00010002
#define JVMPI_VERSION_1 	0x10000001
#define JVMPI_VERSION_1_1	0x10000002

#if defined(__cplusplus)
}
#endif

#if defined(__cplusplus)
#include "jni_cpp.h"
#endif

#endif
