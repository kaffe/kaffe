/*
 * jni.h
 * Java Native Interface.
 *
 * Copyright (c) 1996, 1997
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

#define	JNIEXPORT
#define	JNICALL

struct JNIEnv_;
struct JNINativeInterface;
struct JavaVM_;
struct JNINativeMethod_;
struct JavaVMAttachArgs;
struct JavaVMInitArgs;
typedef const struct JNINativeInterface* JNIEnv;
typedef const struct JNIInvokeInterface* JavaVM;
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
	jbool	(*IsAssignableFrom)		(JNIEnv*, jclass, jclass);
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
	jbool	(*IsSameObject)			(JNIEnv*, jobject, jobject);
	void*	reserved10;
	void*	reserved11;
	jobject	(*AllocObject)			(JNIEnv*, jclass);
	jobject	(*NewObject)			(JNIEnv*, jclass, jmethodID, ...);
	jobject	(*NewObjectV)			(JNIEnv*, jclass, jmethodID, va_list);
	jobject	(*NewObjectA)			(JNIEnv*, jclass, jmethodID, jvalue*);
	jclass	(*GetObjectClass)		(JNIEnv*, jobject);
	jbool	(*IsInstanceOf)			(JNIEnv*, jobject, jclass);
	jmethodID (*GetMethodID)		(JNIEnv*, jclass, const char*, const char*);
	jobject	(*CallObjectMethod)		(JNIEnv*, jobject, jmethodID, ...);
	jobject	(*CallObjectMethodV)		(JNIEnv*, jobject, jmethodID, va_list);
	jobject	(*CallObjectMethodA)		(JNIEnv*, jobject, jmethodID, jvalue*);
	jbool	(*CallBooleanMethod)		(JNIEnv*, jobject, jmethodID, ...);
	jbool	(*CallBooleanMethodV)		(JNIEnv*, jobject, jmethodID, va_list);
	jbool	(*CallBooleanMethodA)		(JNIEnv*, jobject, jmethodID, jvalue*);
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
	jbool	(*CallNonvirtualBooleanMethod)	(JNIEnv*, jobject, jclass, jmethodID, ...);
	jbool	(*CallNonvirtualBooleanMethodV)	(JNIEnv*, jobject, jclass, jmethodID, va_list);
	jbool	(*CallNonvirtualBooleanMethodA)	(JNIEnv*, jobject, jclass, jmethodID, jvalue*);
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
	jbool	(*GetBooleanField)		(JNIEnv*, jobject, jfieldID);
	jbyte	(*GetByteField)			(JNIEnv*, jobject, jfieldID);
	jchar	(*GetCharField)			(JNIEnv*, jobject, jfieldID);
	jshort	(*GetShortField)		(JNIEnv*, jobject, jfieldID);
	jint	(*GetIntField)			(JNIEnv*, jobject, jfieldID);
	jlong	(*GetLongField)			(JNIEnv*, jobject, jfieldID);
	jfloat	(*GetFloatField)		(JNIEnv*, jobject, jfieldID);
	jdouble	(*GetDoubleField)		(JNIEnv*, jobject, jfieldID);
	void	(*SetObjectField)		(JNIEnv*, jobject, jfieldID, jobject);
	void	(*SetBooleanField)		(JNIEnv*, jobject, jfieldID, jbool);
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
	jbool	(*CallStaticBooleanMethod)	(JNIEnv*, jclass, jmethodID, ...);
	jbool	(*CallStaticBooleanMethodV)	(JNIEnv*, jclass, jmethodID, va_list);
	jbool	(*CallStaticBooleanMethodA)	(JNIEnv*, jclass, jmethodID, jvalue*);
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
	jbool	(*GetStaticBooleanField)	(JNIEnv*, jclass, jfieldID);
	jbyte	(*GetStaticByteField)		(JNIEnv*, jclass, jfieldID);
	jchar	(*GetStaticCharField)		(JNIEnv*, jclass, jfieldID);
	jshort	(*GetStaticShortField)		(JNIEnv*, jclass, jfieldID);
	jint	(*GetStaticIntField)		(JNIEnv*, jclass, jfieldID);
	jlong	(*GetStaticLongField)		(JNIEnv*, jclass, jfieldID);
	jfloat	(*GetStaticFloatField)		(JNIEnv*, jclass, jfieldID);
	jdouble	(*GetStaticDoubleField)		(JNIEnv*, jclass, jfieldID);
	void	(*SetStaticObjectField)		(JNIEnv*, jclass, jfieldID, jobject);
	void	(*SetStaticBooleanField)	(JNIEnv*, jclass, jfieldID, jbool);
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
	const jbyte* (*GetStringUTFChars)	(JNIEnv*, jstring, jbool*);
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
	jbool*	(*GetBooleanArrayElements)	(JNIEnv*, jarray, jbool*);
	jbyte*	(*GetByteArrayElements)		(JNIEnv*, jarray, jbool*);
	jchar*	(*GetCharArrayElements)		(JNIEnv*, jarray, jbool*);
	jshort*	(*GetShortArrayElements)	(JNIEnv*, jarray, jbool*);
	jint*	(*GetIntArrayElements)		(JNIEnv*, jarray, jbool*);
	jlong*	(*GetLongArrayElements)		(JNIEnv*, jarray, jbool*);
	jfloat*	(*GetFloatArrayElements)	(JNIEnv*, jarray, jbool*);
	jdouble* (*GetDoubleArrayElements)	(JNIEnv*, jarray, jbool*);
	void	(*ReleaseBooleanArrayElements)	(JNIEnv*, jarray, jbool*, jint);
	void	(*ReleaseByteArrayElements)	(JNIEnv*, jarray, jbyte*, jint);
	void	(*ReleaseCharArrayElements)	(JNIEnv*, jarray, jchar*, jint);
	void	(*ReleaseShortArrayElements)	(JNIEnv*, jarray, jshort*, jint);
	void	(*ReleaseIntArrayElements)	(JNIEnv*, jarray, jint*, jint);
	void	(*ReleaseLongArrayElements)	(JNIEnv*, jarray, jlong*, jint);
	void	(*ReleaseFloatArrayElements)	(JNIEnv*, jarray, jfloat*, jint);
	void	(*ReleaseDoubleArrayElements)	(JNIEnv*, jarray, jdouble*, jint);
	void	(*GetBooleanArrayRegion)	(JNIEnv*, jarray, jsize, jsize, jbool*);
	void	(*GetByteArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jbyte*);
	void	(*GetCharArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jchar*);
	void	(*GetShortArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jshort*);
	void	(*GetIntArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jint*);
	void	(*GetLongArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jlong*);
	void	(*GetFloatArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jfloat*);
	void	(*GetDoubleArrayRegion)		(JNIEnv*, jarray, jsize, jsize, jdouble*);
	void	(*SetBooleanArrayRegion)	(JNIEnv*, jarray, jsize, jsize, jbool*);
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
	jint		allocHeapSize;
	const char*	classhome;
	const char*	librarypath;
};

extern JavaVMInitArgs Kaffe_JavaVMArgs[];

extern jint	JNI_GetDefaultJavaVMInitArgs(JavaVMInitArgs*);
extern jint	JNI_CreateJavaVM(JavaVM**, JNIEnv**, JavaVMInitArgs*);
extern jint	JNI_GetCreatedJavaVMs(JavaVM**, jsize, jsize*);

#endif
