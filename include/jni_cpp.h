/*
 * jni_cpp.h
 * Java Native Interface.
 *
 * Copyright (c) 1996, 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

#ifndef __jni_cpp_h
#define __jni_cpp_h

inline jint JNIEnv::GetVersion(void)
{
	return (functions->GetVersion(this));
}

inline jclass JNIEnv::DefineClass(jobject a, const jbyte* b, jsize c)
{
	return (functions->DefineClass(this, a, b, c));
}

inline jclass JNIEnv::FindClass(const char* a)
{
	return (functions->FindClass(this, a));
}

inline jclass JNIEnv::GetSuperclass(jclass a)
{
	return (functions->GetSuperclass(this, a));
}

inline jboolean JNIEnv::IsAssignableFrom(jclass a, jclass b)
{
	return (functions->IsAssignableFrom(this, a, b));
}

inline jint JNIEnv::Throw(jobject a)
{
	return (functions->Throw(this, a));
}

inline jint JNIEnv::ThrowNew(jclass a, const char* b)
{
	return (functions->ThrowNew(this, a, b));
}

inline jthrowable JNIEnv::ExceptionOccurred(void)
{
	return (functions->ExceptionOccurred(this));
}

inline void JNIEnv::ExceptionDescribe(void)
{
	functions->ExceptionDescribe(this);
}

inline void JNIEnv::ExceptionClear(void)
{
	functions->ExceptionClear(this);
}

inline void JNIEnv::FatalError(const char* a)
{
	functions->FatalError(this, a);
}

inline jref JNIEnv::NewGlobalRef(jref a)
{
	return (functions->NewGlobalRef(this, a));
}

inline void JNIEnv::DeleteGlobalRef(jref a)
{
	functions->DeleteGlobalRef(this, a);
}

inline void JNIEnv::DeleteLocalRef(jref a)
{
	functions->DeleteLocalRef(this, a);
}

inline jboolean JNIEnv::IsSameObject(jobject a, jobject b)
{
	return (functions->IsSameObject(this, a, b));
}

inline jobject JNIEnv::AllocObject(jclass a)
{
	return (functions->AllocObject(this, a));
}

inline jobject JNIEnv::NewObject(jclass a, jmethodID b, ...)
{
	va_list args;
	jobject obj;
	va_start(args, b);
	obj = functions->NewObjectV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jobject JNIEnv::NewObjectV(jclass a, jmethodID b, va_list c)
{
	return (functions->NewObjectV(this, a, b, c));
}

inline jobject JNIEnv::NewObjectA(jclass a, jmethodID b, jvalue* c)
{
	return (functions->NewObjectA(this, a, b, c));
}

inline jclass JNIEnv::GetObjectClass(jobject a)
{
	return (functions->GetObjectClass(this, a));
}

inline jboolean JNIEnv::IsInstanceOf(jobject a, jclass b)
{
	return (functions->IsInstanceOf(this, a, b));
}

inline jmethodID JNIEnv::GetMethodID(jclass a, const char* b, const char* c)
{
	return (functions->GetMethodID(this, a, b, c));
}

inline jobject JNIEnv::CallObjectMethod(jobject a, jmethodID b, ...)
{
	va_list args;
	jobject obj;
	va_start(args, b);
	obj = functions->CallObjectMethod(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jobject JNIEnv::CallObjectMethodV(jobject a, jmethodID b, va_list c)
{
	return (functions->CallObjectMethodV(this, a, b, c));
}

inline jobject JNIEnv::CallObjectMethodA(jobject a, jmethodID b, jvalue* c)
{
	return (functions->CallObjectMethodA(this, a, b, c));
}

inline jboolean JNIEnv::CallBooleanMethod(jobject a, jmethodID b, ...)
{
	va_list args;
	jboolean obj;
	va_start(args, b);
	obj = functions->CallBooleanMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jboolean JNIEnv::CallBooleanMethodV(jobject a, jmethodID b, va_list c)
{
	return (functions->CallBooleanMethodV(this, a, b, c));
}

inline jboolean JNIEnv::CallBooleanMethodA(jobject a, jmethodID b, jvalue* c)
{
	return (functions->CallBooleanMethodA(this, a, b, c));
}

inline jbyte JNIEnv::CallByteMethod(jobject a, jmethodID b, ...)
{
	va_list args;
	jbyte obj;
	va_start(args, b);
	obj = functions->CallByteMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jbyte JNIEnv::CallByteMethodV(jobject a, jmethodID b, va_list c)
{
	return (functions->CallByteMethodV(this, a, b, c));
}

inline jbyte JNIEnv::CallByteMethodA(jobject a, jmethodID b, jvalue* c)
{
	return (functions->CallByteMethodA(this, a, b, c));
}

inline jchar JNIEnv::CallCharMethod(jobject a, jmethodID b, ...)
{
	va_list args;
	jchar obj;
	va_start(args, b);
	obj = functions->CallCharMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jchar JNIEnv::CallCharMethodV(jobject a, jmethodID b, va_list c)
{
	return (functions->CallCharMethodV(this, a, b, c));
}

inline jchar JNIEnv::CallCharMethodA(jobject a, jmethodID b, jvalue* c)
{
	return (functions->CallCharMethodA(this, a, b, c));
}

inline jshort JNIEnv::CallShortMethod(jobject a, jmethodID b, ...)
{
	va_list args;
	jshort obj;
	va_start(args, b);
	obj = functions->CallShortMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jshort JNIEnv::CallShortMethodV(jobject a, jmethodID b, va_list c)
{
	return (functions->CallShortMethodV(this, a, b, c));
}

inline jshort JNIEnv::CallShortMethodA(jobject a, jmethodID b, jvalue* c)
{
	return (functions->CallShortMethodA(this, a, b, c));
}

inline jint JNIEnv::CallIntMethod(jobject a, jmethodID b, ...)
{
	va_list args;
	jint obj;
	va_start(args, b);
	obj = functions->CallIntMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jint JNIEnv::CallIntMethodV(jobject a, jmethodID b, va_list c)
{
	return (functions->CallIntMethodV(this, a, b, c));
}

inline jint JNIEnv::CallIntMethodA(jobject a, jmethodID b, jvalue* c)
{
	return (functions->CallIntMethodA(this, a, b, c));
}

inline jlong JNIEnv::CallLongMethod(jobject a, jmethodID b, ...)
{
	va_list args;
	jlong obj;
	va_start(args, b);
	obj = functions->CallLongMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jlong JNIEnv::CallLongMethodV(jobject a, jmethodID b, va_list c)
{
	return (functions->CallLongMethodV(this, a, b, c));
}

inline jlong JNIEnv::CallLongMethodA(jobject a, jmethodID b, jvalue* c)
{
	return (functions->CallLongMethodA(this, a, b, c));
}

inline jfloat JNIEnv::CallFloatMethod(jobject a, jmethodID b, ...)
{
	va_list args;
	jfloat obj;
	va_start(args, b);
	obj = functions->CallFloatMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jfloat JNIEnv::CallFloatMethodV(jobject a, jmethodID b, va_list c)
{
	return (functions->CallFloatMethodV(this, a, b, c));
}

inline jfloat JNIEnv::CallFloatMethodA(jobject a, jmethodID b, jvalue* c)
{
	return (functions->CallFloatMethodA(this, a, b, c));
}

inline jdouble JNIEnv::CallDoubleMethod(jobject a, jmethodID b, ...)
{
	va_list args;
	jdouble obj;
	va_start(args, b);
	obj = functions->CallDoubleMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jdouble JNIEnv::CallDoubleMethodV(jobject a, jmethodID b, va_list c)
{
	return (functions->CallDoubleMethodV(this, a, b, c));
}

inline jdouble JNIEnv::CallDoubleMethodA(jobject a, jmethodID b, jvalue* c)
{
	return (functions->CallDoubleMethodA(this, a, b, c));
}

inline void JNIEnv::CallVoidMethod(jobject a, jmethodID b, ...)
{
	va_list args;
	va_start(args, b);
	functions->CallVoidMethodV(this, a, b, args);
	va_end(args);
}

inline void JNIEnv::CallVoidMethodV(jobject a, jmethodID b, va_list c)
{
	functions->CallVoidMethodV(this, a, b, c);
}

inline void JNIEnv::CallVoidMethodA(jobject a, jmethodID b, jvalue* c)
{
	functions->CallVoidMethodA(this, a, b, c);
}

inline jobject JNIEnv::CallNonvirtualObjectMethod(jobject a, jclass b, jmethodID c, ...)
{
	va_list args;
	jobject obj;
	va_start(args, c);
	obj = functions->CallNonvirtualObjectMethodV(this, a, b, c, args);
	va_end(args);
	return (obj);
}

inline jobject JNIEnv::CallNonvirtualObjectMethodV(jobject a, jclass b, jmethodID c, va_list d)
{
	return (functions->CallNonvirtualObjectMethodV(this, a, b, c, d));
}

inline jobject JNIEnv::CallNonvirtualObjectMethodA(jobject a, jclass b, jmethodID c, jvalue* d)
{
	return (functions->CallNonvirtualObjectMethodA(this, a, b, c, d));
}

inline jboolean JNIEnv::CallNonvirtualBooleanMethod(jobject a, jclass b, jmethodID c, ...)
{
	va_list args;
	jboolean obj;
	va_start(args, c);
	obj = functions->CallNonvirtualBooleanMethodV(this, a, b, c, args);
	va_end(args);
	return (obj);
}

inline jboolean JNIEnv::CallNonvirtualBooleanMethodV(jobject a, jclass b, jmethodID c, va_list d)
{
	return (functions->CallNonvirtualBooleanMethodV(this, a, b, c, d));
}

inline jboolean JNIEnv::CallNonvirtualBooleanMethodA(jobject a, jclass b, jmethodID c, jvalue* d)
{
	return (functions->CallNonvirtualBooleanMethodA(this, a, b, c, d));
}

inline jbyte JNIEnv::CallNonvirtualByteMethod(jobject a, jclass b, jmethodID c, ...)
{
	va_list args;
	jbyte obj;
	va_start(args, c);
	obj = functions->CallNonvirtualByteMethodV(this, a, b, c, args);
	va_end(args);
	return (obj);
}

inline jbyte JNIEnv::CallNonvirtualByteMethodV(jobject a, jclass b, jmethodID c, va_list d)
{
	return (functions->CallNonvirtualByteMethodV(this, a, b, c, d));
}

inline jbyte JNIEnv::CallNonvirtualByteMethodA(jobject a, jclass b, jmethodID c, jvalue* d)
{
	return (functions->CallNonvirtualByteMethodA(this, a, b, c, d));
}

inline jchar JNIEnv::CallNonvirtualCharMethod(jobject a, jclass b, jmethodID c, ...)
{
	va_list args;
	jchar obj;
	va_start(args, c);
	obj = functions->CallNonvirtualCharMethodV(this, a, b, c, args);
	va_end(args);
	return (obj);
}

inline jchar JNIEnv::CallNonvirtualCharMethodV(jobject a, jclass b, jmethodID c, va_list d)
{
	return (functions->CallNonvirtualCharMethodV(this, a, b, c, d));
}

inline jchar JNIEnv::CallNonvirtualCharMethodA(jobject a, jclass b, jmethodID c, jvalue* d)
{
	return (functions->CallNonvirtualCharMethodA(this, a, b, c, d));
}

inline jshort JNIEnv::CallNonvirtualShortMethod(jobject a, jclass b, jmethodID c, ...)
{
	va_list args;
	jshort obj;
	va_start(args, c);
	obj = functions->CallNonvirtualShortMethodV(this, a, b, c, args);
	va_end(args);
	return (obj);
}

inline jshort JNIEnv::CallNonvirtualShortMethodV(jobject a, jclass b, jmethodID c, va_list d)
{
	return (functions->CallNonvirtualShortMethodV(this, a, b, c, d));
}

inline jshort JNIEnv::CallNonvirtualShortMethodA(jobject a, jclass b, jmethodID c, jvalue* d)
{
	return (functions->CallNonvirtualShortMethodA(this, a, b, c, d));
}

inline jint JNIEnv::CallNonvirtualIntMethod(jobject a, jclass b, jmethodID c, ...)
{
	va_list args;
	jint obj;
	va_start(args, c);
	obj = functions->CallNonvirtualIntMethodV(this, a, b, c, args);
	va_end(args);
	return (obj);
}

inline jint JNIEnv::CallNonvirtualIntMethodV(jobject a, jclass b, jmethodID c, va_list d)
{
	return (functions->CallNonvirtualIntMethodV(this, a, b, c, d));
}

inline jint JNIEnv::CallNonvirtualIntMethodA(jobject a, jclass b, jmethodID c, jvalue* d)
{
	return (functions->CallNonvirtualIntMethodA(this, a, b, c, d));
}

inline jlong JNIEnv::CallNonvirtualLongMethod(jobject a, jclass b, jmethodID c, ...)
{
	va_list args;
	jlong obj;
	va_start(args, c);
	obj = functions->CallNonvirtualLongMethodV(this, a, b, c, args);
	va_end(args);
	return (obj);
}

inline jlong JNIEnv::CallNonvirtualLongMethodV(jobject a, jclass b, jmethodID c, va_list d)
{
	return (functions->CallNonvirtualLongMethodV(this, a, b, c, d));
}

inline jlong JNIEnv::CallNonvirtualLongMethodA(jobject a, jclass b, jmethodID c, jvalue* d)
{
	return (functions->CallNonvirtualLongMethodA(this, a, b, c, d));
}

inline jfloat JNIEnv::CallNonvirtualFloatMethod(jobject a, jclass b, jmethodID c, ...)
{
	va_list args;
	jfloat obj;
	va_start(args, c);
	obj = functions->CallNonvirtualFloatMethodV(this, a, b, c, args);
	va_end(args);
	return (obj);
}

inline jfloat JNIEnv::CallNonvirtualFloatMethodV(jobject a, jclass b, jmethodID c, va_list d)
{
	return (functions->CallNonvirtualFloatMethodV(this, a, b, c, d));
}

inline jfloat JNIEnv::CallNonvirtualFloatMethodA(jobject a, jclass b, jmethodID c, jvalue* d)
{
	return (functions->CallNonvirtualFloatMethodA(this, a, b, c, d));
}

inline jdouble JNIEnv::CallNonvirtualDoubleMethod(jobject a, jclass b, jmethodID c, ...)
{
	va_list args;
	jdouble obj;
	va_start(args, c);
	obj = functions->CallNonvirtualDoubleMethodV(this, a, b, c, args);
	va_end(args);
	return (obj);
}

inline jdouble JNIEnv::CallNonvirtualDoubleMethodV(jobject a, jclass b, jmethodID c, va_list d)
{
	return (functions->CallNonvirtualDoubleMethodV(this, a, b, c, d));
}

inline jdouble JNIEnv::CallNonvirtualDoubleMethodA(jobject a, jclass b, jmethodID c, jvalue* d)
{
	return (functions->CallNonvirtualDoubleMethodA(this, a, b, c, d));
}

inline void JNIEnv::CallNonvirtualVoidMethod(jobject a, jclass b, jmethodID c, ...)
{
	va_list args;
	va_start(args, c);
	functions->CallNonvirtualVoidMethodV(this, a, b, c, args);
	va_end(args);
}

inline void JNIEnv::CallNonvirtualVoidMethodV(jobject a, jclass b, jmethodID c, va_list d)
{
	functions->CallNonvirtualVoidMethodV(this, a, b, c, d);
}

inline void JNIEnv::CallNonvirtualVoidMethodA(jobject a, jclass b, jmethodID c, jvalue* d)
{
	functions->CallNonvirtualVoidMethodA(this, a, b, c, d);
}

inline jfieldID JNIEnv::GetFieldID(jclass a, const char* b, const char* c)
{
	return (functions->GetFieldID(this, a, b, c));
}

inline jobject JNIEnv::GetObjectField(jobject a, jfieldID b)
{
	return (functions->GetObjectField(this, a, b));
}

inline jboolean JNIEnv::GetBooleanField(jobject a, jfieldID b)
{
	return (functions->GetBooleanField(this, a, b));
}

inline jbyte JNIEnv::GetByteField(jobject a, jfieldID b)
{
	return (functions->GetByteField(this, a, b));
}

inline jchar JNIEnv::GetCharField(jobject a, jfieldID b)
{
	return (functions->GetCharField(this, a, b));
}

inline jshort JNIEnv::GetShortField(jobject a, jfieldID b)
{
	return (functions->GetShortField(this, a, b));
}

inline jint JNIEnv::GetIntField(jobject a, jfieldID b)
{
	return (functions->GetIntField(this, a, b));
}

inline jlong JNIEnv::GetLongField(jobject a, jfieldID b)
{
	return (functions->GetLongField(this, a, b));
}

inline jfloat JNIEnv::GetFloatField(jobject a, jfieldID b)
{
	return (functions->GetFloatField(this, a, b));
}

inline jdouble JNIEnv::GetDoubleField(jobject a, jfieldID b)
{
	return (functions->GetDoubleField(this, a, b));
}

inline void JNIEnv::SetObjectField(jobject a, jfieldID b, jobject c)
{
	functions->SetObjectField(this, a, b, c);
}

inline void JNIEnv::SetBooleanField(jobject a, jfieldID b, jboolean c)
{
	functions->SetBooleanField(this, a, b, c);
}

inline void JNIEnv::SetByteField(jobject a, jfieldID b, jbyte c)
{
	functions->SetByteField(this, a, b, c);
}

inline void JNIEnv::SetCharField(jobject a, jfieldID b, jchar c)
{
	functions->SetCharField(this, a, b, c);
}

inline void JNIEnv::SetShortField(jobject a, jfieldID b, jshort c)
{
	functions->SetShortField(this, a, b, c);
}

inline void JNIEnv::SetIntField(jobject a, jfieldID b, jint c)
{
	functions->SetIntField(this, a, b, c);
}

inline void JNIEnv::SetLongField(jobject a, jfieldID b, jlong c)
{
	functions->SetLongField(this, a, b, c);
}

inline void JNIEnv::SetFloatField(jobject a, jfieldID b, jfloat c)
{
	functions->SetFloatField(this, a, b, c);
}

inline void JNIEnv::SetDoubleField(jobject a, jfieldID b, jdouble c)
{
	functions->SetDoubleField(this, a, b, c);
}

inline jmethodID JNIEnv::GetStaticMethodID(jclass a, const char* b, const char* c)
{
	return (functions->GetStaticMethodID(this, a, b, c));
}

inline jobject JNIEnv::CallStaticObjectMethod(jclass a, jmethodID b, ...)
{
	va_list args;
	jobject obj;
	va_start(args, b);
	obj = functions->CallStaticObjectMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jobject JNIEnv::CallStaticObjectMethodV(jclass a, jmethodID b, va_list c)
{
	return (functions->CallStaticObjectMethodV(this, a, b, c));
}

inline jobject JNIEnv::CallStaticObjectMethodA(jclass a, jmethodID b, jvalue* c)
{
	return (functions->CallStaticObjectMethodA(this, a, b, c));
}

inline jboolean JNIEnv::CallStaticBooleanMethod(jclass a, jmethodID b, ...)
{
	va_list args;
	jboolean obj;
	va_start(args, b);
	obj = functions->CallStaticBooleanMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jboolean JNIEnv::CallStaticBooleanMethodV(jclass a, jmethodID b, va_list c)
{
	return (functions->CallStaticBooleanMethodV(this, a, b, c));
}

inline jboolean JNIEnv::CallStaticBooleanMethodA(jclass a, jmethodID b, jvalue* c)
{
	return (functions->CallStaticBooleanMethodA(this, a, b, c));
}

inline jbyte JNIEnv::CallStaticByteMethod(jclass a, jmethodID b, ...)
{
	va_list args;
	jbyte obj;
	va_start(args, b);
	obj = functions->CallStaticByteMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jbyte JNIEnv::CallStaticByteMethodV(jclass a, jmethodID b, va_list c)
{
	return (functions->CallStaticByteMethodV(this, a, b, c));
}

inline jbyte JNIEnv::CallStaticByteMethodA(jclass a, jmethodID b, jvalue* c)
{
	return (functions->CallStaticByteMethodA(this, a, b, c));
}

inline jchar JNIEnv::CallStaticCharMethod(jclass a, jmethodID b, ...)
{
	va_list args;
	jchar obj;
	va_start(args, b);
	obj = functions->CallStaticCharMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jchar JNIEnv::CallStaticCharMethodV(jclass a, jmethodID b, va_list c)
{
	return (functions->CallStaticCharMethodV(this, a, b, c));
}

inline jchar JNIEnv::CallStaticCharMethodA(jclass a, jmethodID b, jvalue* c)
{
	return (functions->CallStaticCharMethodA(this, a, b, c));
}

inline jshort JNIEnv::CallStaticShortMethod(jclass a, jmethodID b, ...)
{
	va_list args;
	jshort obj;
	va_start(args, b);
	obj = functions->CallStaticShortMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jshort JNIEnv::CallStaticShortMethodV(jclass a, jmethodID b, va_list c)
{
	return (functions->CallStaticShortMethodV(this, a, b, c));
}

inline jshort JNIEnv::CallStaticShortMethodA(jclass a, jmethodID b, jvalue* c)
{
	return (functions->CallStaticShortMethodA(this, a, b, c));
}

inline jint JNIEnv::CallStaticIntMethod(jclass a, jmethodID b, ...)
{
	va_list args;
	jint obj;
	va_start(args, b);
	obj = functions->CallStaticIntMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jint JNIEnv::CallStaticIntMethodV(jclass a, jmethodID b, va_list c)
{
	return (functions->CallStaticIntMethodV(this, a, b, c));
}

inline jint JNIEnv::CallStaticIntMethodA(jclass a, jmethodID b, jvalue* c)
{
	return (functions->CallStaticIntMethodA(this, a, b, c));
}

inline jlong JNIEnv::CallStaticLongMethod(jclass a, jmethodID b, ...)
{
	va_list args;
	jlong obj;
	va_start(args, b);
	obj = functions->CallStaticLongMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jlong JNIEnv::CallStaticLongMethodV(jclass a, jmethodID b, va_list c)
{
	return (functions->CallStaticLongMethodV(this, a, b, c));
}

inline jlong JNIEnv::CallStaticLongMethodA(jclass a, jmethodID b, jvalue* c)
{
	return (functions->CallStaticLongMethodA(this, a, b, c));
}

inline jfloat JNIEnv::CallStaticFloatMethod(jclass a, jmethodID b, ...)
{
	va_list args;
	jfloat obj;
	va_start(args, b);
	obj = functions->CallStaticFloatMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jfloat JNIEnv::CallStaticFloatMethodV(jclass a, jmethodID b, va_list c)
{
	return (functions->CallStaticFloatMethodV(this, a, b, c));
}

inline jfloat JNIEnv::CallStaticFloatMethodA(jclass a, jmethodID b, jvalue* c)
{
	return (functions->CallStaticFloatMethodA(this, a, b, c));
}

inline jdouble JNIEnv::CallStaticDoubleMethod(jclass a, jmethodID b, ...)
{
	va_list args;
	jdouble obj;
	va_start(args, b);
	obj = functions->CallStaticDoubleMethodV(this, a, b, args);
	va_end(args);
	return (obj);
}

inline jdouble JNIEnv::CallStaticDoubleMethodV(jclass a, jmethodID b, va_list c)
{
	return (functions->CallStaticDoubleMethodV(this, a, b, c));
}

inline jdouble JNIEnv::CallStaticDoubleMethodA(jclass a, jmethodID b, jvalue* c)
{
	return (functions->CallStaticDoubleMethodA(this, a, b, c));
}

inline void JNIEnv::CallStaticVoidMethod(jclass a, jmethodID b, ...)
{
	va_list args;
	va_start(args, b);
	functions->CallStaticVoidMethodV(this, a, b, args);
	va_end(args);
}

inline void JNIEnv::CallStaticVoidMethodV(jclass a, jmethodID b, va_list c)
{
	functions->CallStaticVoidMethodV(this, a, b, c);
}

inline void JNIEnv::CallStaticVoidMethodA(jclass a, jmethodID b, jvalue* c)
{
	functions->CallStaticVoidMethodA(this, a, b, c);
}

inline jfieldID JNIEnv::GetStaticFieldID(jclass a, const char* b, const char* c)
{
	return (functions->GetStaticFieldID(this, a, b, c));
}

inline jobject JNIEnv::GetStaticObjectField(jclass a, jfieldID b)
{
	return (functions->GetStaticObjectField(this, a, b));
}

inline jboolean JNIEnv::GetStaticBooleanField(jclass a, jfieldID b)
{
	return (functions->GetStaticBooleanField(this, a, b));
}

inline jbyte JNIEnv::GetStaticByteField(jclass a, jfieldID b)
{
	return (functions->GetStaticByteField(this, a, b));
}

inline jchar JNIEnv::GetStaticCharField(jclass a, jfieldID b)
{
	return (functions->GetStaticCharField(this, a, b));
}

inline jshort JNIEnv::GetStaticShortField(jclass a, jfieldID b)
{
	return (functions->GetStaticShortField(this, a, b));
}

inline jint JNIEnv::GetStaticIntField(jclass a, jfieldID b)
{
	return (functions->GetStaticIntField(this, a, b));
}

inline jlong JNIEnv::GetStaticLongField(jclass a, jfieldID b)
{
	return (functions->GetStaticLongField(this, a, b));
}

inline jfloat JNIEnv::GetStaticFloatField(jclass a, jfieldID b)
{
	return (functions->GetStaticFloatField(this, a, b));
}

inline jdouble JNIEnv::GetStaticDoubleField(jclass a, jfieldID b)
{
	return (functions->GetStaticDoubleField(this, a, b));
}

inline void JNIEnv::SetStaticObjectField(jclass a, jfieldID b, jobject c)
{
	functions->SetStaticObjectField(this, a, b, c);
}

inline void JNIEnv::SetStaticBooleanField(jclass a, jfieldID b, jboolean c)
{
	functions->SetStaticBooleanField(this, a, b, c);
}

inline void JNIEnv::SetStaticByteField(jclass a, jfieldID b, jbyte c)
{
	functions->SetStaticByteField(this, a, b, c);
}

inline void JNIEnv::SetStaticCharField(jclass a, jfieldID b, jchar c)
{
	functions->SetStaticCharField(this, a, b, c);
}

inline void JNIEnv::SetStaticShortField(jclass a, jfieldID b, jshort c)
{
	functions->SetStaticShortField(this, a, b, c);
}

inline void JNIEnv::SetStaticIntField(jclass a, jfieldID b, jint c)
{
	functions->SetStaticIntField(this, a, b, c);
}

inline void JNIEnv::SetStaticLongField(jclass a, jfieldID b, jlong c)
{
	functions->SetStaticLongField(this, a, b, c);
}

inline void JNIEnv::SetStaticFloatField(jclass a, jfieldID b, jfloat c)
{
	functions->SetStaticFloatField(this, a, b, c);
}

inline void JNIEnv::SetStaticDoubleField(jclass a, jfieldID b, jdouble c)
{
	functions->SetStaticDoubleField(this, a, b, c);
}

inline jstring JNIEnv::NewString(const jchar* a, jsize b)
{
	return (functions->NewString(this, a, b));
}

inline jsize JNIEnv::GetStringLength(jstring a)
{
	return (functions->GetStringLength(this, a));
}

inline const jchar* JNIEnv::GetStringChars(jstring a, jboolean* b)
{
	return (functions->GetStringChars(this, a, b));
}

inline void JNIEnv::ReleaseStringChars(jstring a, const jchar* b)
{
	functions->ReleaseStringChars(this, a, b);
}

inline jstring JNIEnv::NewStringUTF(const char* a)
{
	return (functions->NewStringUTF(this, a));
}

inline jsize JNIEnv::GetStringUTFLength(jstring a)
{
	return (functions->GetStringUTFLength(this, a));
}

inline const jbyte* JNIEnv::GetStringUTFChars(jstring a, jboolean* b)
{
	return (functions->GetStringUTFChars(this, a, b));
}

inline void JNIEnv::ReleaseStringUTFChars(jstring a, const jbyte* b)
{
	functions->ReleaseStringUTFChars(this, a, b);
}

inline jsize JNIEnv::GetArrayLength(jarray a)
{
	return (functions->GetArrayLength(this, a));
}

inline jarray JNIEnv::NewObjectArray(jsize a, jclass b, jobject c)
{
	return (functions->NewObjectArray(this, a, b, c));
}

inline jobject JNIEnv::GetObjectArrayElement(jarray a, jsize b)
{
	return (functions->GetObjectArrayElement(this, a, b));
}

inline void JNIEnv::SetObjectArrayElement(jarray a, jsize b, jobject c)
{
	functions->SetObjectArrayElement(this, a, b, c);
}

inline jarray JNIEnv::NewBooleanArray(jsize a)
{
	return (functions->NewBooleanArray(this, a));
}

inline jarray JNIEnv::NewByteArray(jsize a)
{
	return (functions->NewByteArray(this, a));
}

inline jarray JNIEnv::NewCharArray(jsize a)
{
	return (functions->NewCharArray(this, a));
}

inline jarray JNIEnv::NewShortArray(jsize a)
{
	return (functions->NewShortArray(this, a));
}

inline jarray JNIEnv::NewIntArray(jsize a)
{
	return (functions->NewIntArray(this, a));
}

inline jarray JNIEnv::NewLongArray(jsize a)
{
	return (functions->NewLongArray(this, a));
}

inline jarray JNIEnv::NewFloatArray(jsize a)
{
	return (functions->NewFloatArray(this, a));
}

inline jarray JNIEnv::NewDoubleArray(jsize a)
{
	return (functions->NewDoubleArray(this, a));
}

inline jboolean* JNIEnv::GetBooleanArrayElements(jarray a, jboolean* b)
{
	return (functions->GetBooleanArrayElements(this, a, b));
}

inline jbyte* JNIEnv::GetByteArrayElements(jarray a, jboolean* b)
{
	return (functions->GetByteArrayElements(this, a, b));
}

inline jchar* JNIEnv::GetCharArrayElements(jarray a, jboolean* b)
{
	return (functions->GetCharArrayElements(this, a, b));
}

inline jshort* JNIEnv::GetShortArrayElements(jarray a, jboolean* b)
{
	return (functions->GetShortArrayElements(this, a, b));
}

inline jint* JNIEnv::GetIntArrayElements(jarray a, jboolean* b)
{
	return (functions->GetIntArrayElements(this, a, b));
}

inline jlong* JNIEnv::GetLongArrayElements(jarray a, jboolean* b)
{
	return (functions->GetLongArrayElements(this, a, b));
}

inline jfloat* JNIEnv::GetFloatArrayElements(jarray a, jboolean* b)
{
	return (functions->GetFloatArrayElements(this, a, b));
}

inline jdouble* JNIEnv::GetDoubleArrayElements(jarray a, jboolean* b)
{
	return (functions->GetDoubleArrayElements(this, a, b));
}

inline void JNIEnv::ReleaseBooleanArrayElements(jarray a, jboolean* b, jint c)
{
	functions->ReleaseBooleanArrayElements(this, a, b, c);
}

inline void JNIEnv::ReleaseByteArrayElements(jarray a, jbyte* b, jint c)
{
	functions->ReleaseByteArrayElements(this, a, b, c);
}

inline void JNIEnv::ReleaseCharArrayElements(jarray a, jchar* b, jint c)
{
	functions->ReleaseCharArrayElements(this, a, b, c);
}

inline void JNIEnv::ReleaseShortArrayElements(jarray a, jshort* b, jint c)
{
	functions->ReleaseShortArrayElements(this, a, b, c);
}

inline void JNIEnv::ReleaseIntArrayElements(jarray a, jint* b, jint c)
{
	functions->ReleaseIntArrayElements(this, a, b, c);
}

inline void JNIEnv::ReleaseLongArrayElements(jarray a, jlong* b, jint c)
{
	functions->ReleaseLongArrayElements(this, a, b, c);
}

inline void JNIEnv::ReleaseFloatArrayElements(jarray a, jfloat* b, jint c)
{
	functions->ReleaseFloatArrayElements(this, a, b, c);
}

inline void JNIEnv::ReleaseDoubleArrayElements(jarray a, jdouble* b, jint c)
{
	functions->ReleaseDoubleArrayElements(this, a, b, c);
}

inline void JNIEnv::GetBooleanArrayRegion(jarray a, jsize b, jsize c, jboolean* d)
{
	functions->GetBooleanArrayRegion(this, a, b, c, d);
}

inline void JNIEnv::GetByteArrayRegion(jarray a, jsize b, jsize c, jbyte* d)
{
	functions->GetByteArrayRegion(this, a, b, c, d);
}

inline void JNIEnv::GetCharArrayRegion(jarray a, jsize b, jsize c, jchar* d)
{
	functions->GetCharArrayRegion(this, a, b, c, d);
}

inline void JNIEnv::GetShortArrayRegion(jarray a, jsize b, jsize c, jshort* d)
{
	functions->GetShortArrayRegion(this, a, b, c, d);
}

inline void JNIEnv::GetIntArrayRegion(jarray a, jsize b, jsize c, jint* d)
{
	functions->GetIntArrayRegion(this, a, b, c, d);
}

inline void JNIEnv::GetLongArrayRegion(jarray a, jsize b, jsize c, jlong* d)
{
	functions->GetLongArrayRegion(this, a, b, c, d);
}

inline void JNIEnv::GetFloatArrayRegion(jarray a, jsize b, jsize c, jfloat* d)
{
	functions->GetFloatArrayRegion(this, a, b, c, d);
}

inline void JNIEnv::GetDoubleArrayRegion(jarray a, jsize b, jsize c, jdouble* d)
{
	functions->GetDoubleArrayRegion(this, a, b, c, d);
}

inline void JNIEnv::SetBooleanArrayRegion(jarray a, jsize b, jsize c, jboolean* d)
{
	functions->SetBooleanArrayRegion(this, a, b, c, d);
}

inline void JNIEnv::SetByteArrayRegion(jarray a, jsize b, jsize c, jbyte* d)
{
	functions->SetByteArrayRegion(this, a, b, c, d);
}

inline void JNIEnv::SetCharArrayRegion(jarray a, jsize b, jsize c, jchar* d)
{
	functions->SetCharArrayRegion(this, a, b, c, d);
}

inline void JNIEnv::SetShortArrayRegion(jarray a, jsize b, jsize c, jshort* d)
{
	functions->SetShortArrayRegion(this, a, b, c, d);
}

inline void JNIEnv::SetIntArrayRegion(jarray a, jsize b, jsize c, jint* d)
{
	functions->SetIntArrayRegion(this, a, b, c, d);
}

inline void JNIEnv::SetLongArrayRegion(jarray a, jsize b, jsize c, jlong* d)
{
	functions->SetLongArrayRegion(this, a, b, c, d);
}

inline void JNIEnv::SetFloatArrayRegion(jarray a, jsize b, jsize c, jfloat* d)
{
	functions->SetFloatArrayRegion(this, a, b, c, d);
}

inline void JNIEnv::SetDoubleArrayRegion(jarray a, jsize b, jsize c, jdouble* d)
{
	functions->SetDoubleArrayRegion(this, a, b, c, d);
}

inline jint JNIEnv::RegisterNatives(jclass a, const JNINativeMethod* b, jint c)
{
	return (functions->RegisterNatives(this, a, b, c));
}

inline jint JNIEnv::UnregisterNatives(jclass a)
{
	return (functions->UnregisterNatives(this, a));
}

inline jint JNIEnv::MonitorEnter(jobject a)
{
	return (functions->MonitorEnter(this, a));
}

inline jint JNIEnv::MonitorExit(jobject a)
{
	return (functions->MonitorExit(this, a));
}

inline jint JNIEnv::GetJavaVM(JavaVM** a)
{
	return (functions->GetJavaVM(this, a));
}

inline jboolean JNIEnv::ExceptionCheck(void)
{
	return (functions->ExceptionCheck(this));
}

inline jint JavaVM::DestroyJavaVM(void)
{
	return (functions->DestroyJavaVM(this));
}

inline jint JavaVM::AttachCurrentThread(void** env, ThreadAttachArgs* args)
{
	return (functions->AttachCurrentThread(this, env, args));
}

inline jint JavaVM::DetachCurrentThread(void)
{
	return (functions->DetachCurrentThread(this));
}

inline jint JavaVM::GetEnv(void** envPtr, jint version)
{
	return (functions->GetEnv(this, envPtr, version));
}
#endif
