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

/* ====== Invokation API ============================================================= */

jint KaffeJNI_DestroyJavaVM(JavaVM* vm);

/* ====== Helpers =================================================================== */

jmethodID KaffeJNI_FromReflectedMethod (JNIEnv *, jobject);
jfieldID  KaffeJNI_FromReflectedField (JNIEnv *, jobject);
jobject   KaffeJNI_ToReflectedMethod (JNIEnv *, jclass, jmethodID, jboolean);
jobject   KaffeJNI_ToReflectedField (JNIEnv *, jclass, jfieldID, jboolean);
jobject   KaffeJNI_NewDirectByteBuffer(JNIEnv *, void *, jlong);
void *    KaffeJNI_GetDirectBufferAddress(JNIEnv *, jobject);
jlong     KaffeJNI_GetDirectBufferCapacity(JNIEnv *, jobject);
void NONRETURNING
          KaffeJNI_FatalError(JNIEnv* env, const char* mess);


/* ====== String handling =========================================================== */

void    KaffeJNI_ReleaseStringChars(JNIEnv* env, jstring data, const jchar* chars);
jstring KaffeJNI_NewString(JNIEnv* env, const jchar* data, jsize len);
jsize   KaffeJNI_GetStringLength(JNIEnv* env, jstring data);
const jchar*
        KaffeJNI_GetStringChars(JNIEnv* env, jstring data, jboolean* copy);
jstring KaffeJNI_NewStringUTF(JNIEnv* env, const char* data);
jsize   KaffeJNI_GetStringUTFLength(JNIEnv* env, jstring data);
const char*
        KaffeJNI_GetStringUTFChars(JNIEnv* env, jstring data, jboolean* copy);
void    KaffeJNI_ReleaseStringUTFChars(JNIEnv* env, jstring data, const char* chars);
void    KaffeJNI_GetStringRegion(JNIEnv *env, jstring data, jsize start, jsize len, jchar *buf);
void    KaffeJNI_GetStringUTFRegion(JNIEnv *env, jstring data, jsize start, jsize len, char *buf);

/* ====== Functions handling arrays ================================================= */

jobject KaffeJNI_GetObjectArrayElement(JNIEnv* env, jobjectArray arr, jsize elem);
void    KaffeJNI_SetObjectArrayElement(JNIEnv* env, jobjectArray arr, jsize elem, jobject val);

jobjectArray
        KaffeJNI_NewObjectArray(JNIEnv* env, jsize len, jclass cls, jobject init);
jbooleanArray
        KaffeJNI_NewBooleanArray(JNIEnv* env, jsize len);
jbyteArray
        KaffeJNI_NewByteArray(JNIEnv* env, jsize len);
jcharArray
        KaffeJNI_NewCharArray(JNIEnv* env, jsize len);
jshortArray
        KaffeJNI_NewShortArray(JNIEnv* env, jsize len);
jintArray
        KaffeJNI_NewIntArray(JNIEnv* env, jsize len);
jlongArray
        KaffeJNI_NewLongArray(JNIEnv* env, jsize len);
jfloatArray
        KaffeJNI_NewFloatArray(JNIEnv* env, jsize len);
jdoubleArray
        KaffeJNI_NewDoubleArray(JNIEnv* env, jsize len);

jboolean* KaffeJNI_GetBooleanArrayElements(JNIEnv* env, jbooleanArray arr, jboolean* iscopy);
jbyte*    KaffeJNI_GetByteArrayElements(JNIEnv* env, jbyteArray arr, jboolean* iscopy);
void*     KaffeJNI_GetPrimitiveArrayCritical(JNIEnv* env, jarray arr, jboolean* iscopy);
jchar*    KaffeJNI_GetCharArrayElements(JNIEnv* env, jcharArray arr, jboolean* iscopy);
jshort*   KaffeJNI_GetShortArrayElements(JNIEnv* env, jshortArray arr, jboolean* iscopy);
jint*     KaffeJNI_GetIntArrayElements(JNIEnv* env, jintArray arr, jboolean* iscopy);
jlong*    KaffeJNI_GetLongArrayElements(JNIEnv* env, jlongArray arr, jboolean* iscopy);
jfloat*   KaffeJNI_GetFloatArrayElements(JNIEnv* env, jfloatArray arr, jboolean* iscopy);
jdouble*  KaffeJNI_GetDoubleArrayElements(JNIEnv* env, jdoubleArray arr, jboolean* iscopy);

void      KaffeJNI_ReleaseBooleanArrayElements(JNIEnv* env, jbooleanArray arr, jboolean* elems, jint mode);
void      KaffeJNI_ReleaseByteArrayElements(JNIEnv* env, jbyteArray arr, jbyte* elems, jint mode);
void      KaffeJNI_ReleasePrimitiveArrayCritical(JNIEnv* env, jbyteArray arr, void* elems, jint mode);
void      KaffeJNI_ReleaseCharArrayElements(JNIEnv* env, jcharArray arr, jchar* elems, jint mode);
void      KaffeJNI_ReleaseShortArrayElements(JNIEnv* env, jshortArray arr, jshort* elems, jint mode);
void      KaffeJNI_ReleaseIntArrayElements(JNIEnv* env, jintArray arr, jint* elems, jint mode);
void      KaffeJNI_ReleaseLongArrayElements(JNIEnv* env, jlongArray arr, jlong* elems, jint mode);
void      KaffeJNI_ReleaseFloatArrayElements(JNIEnv* env, jfloatArray arr, jfloat* elems, jint mode);
void      KaffeJNI_ReleaseDoubleArrayElements(JNIEnv* env, jdoubleArray arr, jdouble* elems, jint mode);

void      KaffeJNI_GetBooleanArrayRegion(JNIEnv* env, jbooleanArray arr, jsize start, jsize len, jboolean* data);
void      KaffeJNI_GetByteArrayRegion(JNIEnv* env, jbyteArray arr, jsize start, jsize len, jbyte* data);
void      KaffeJNI_GetCharArrayRegion(JNIEnv* env, jcharArray arr, jsize start, jsize len, jchar* data);
void      KaffeJNI_GetShortArrayRegion(JNIEnv* env, jshortArray arr, jsize start, jsize len, jshort* data);
void      KaffeJNI_GetIntArrayRegion(JNIEnv* env, jintArray arr, jsize start, jsize len, jint* data);
void      KaffeJNI_GetLongArrayRegion(JNIEnv* env, jlongArray arr, jsize start, jsize len, jlong* data);
void      KaffeJNI_GetFloatArrayRegion(JNIEnv* env, jfloatArray arr, jsize start, jsize len, jfloat* data);
void      KaffeJNI_GetDoubleArrayRegion(JNIEnv* env, jdoubleArray arr, jsize start, jsize len, jdouble* data);

void      KaffeJNI_SetBooleanArrayRegion(JNIEnv* env, jbooleanArray arr, jsize start, jsize len, jboolean* data);
void      KaffeJNI_SetByteArrayRegion(JNIEnv* env, jbyteArray arr, jsize start, jsize len, jbyte* data);
void      KaffeJNI_SetCharArrayRegion(JNIEnv* env, jcharArray arr, jsize start, jsize len, jchar* data);
void      KaffeJNI_SetShortArrayRegion(JNIEnv* env, jshortArray arr, jsize start, jsize len, jshort* data);
void      KaffeJNI_SetIntArrayRegion(JNIEnv* env, jintArray arr, jsize start, jsize len, jint* data);
void      KaffeJNI_SetLongArrayRegion(JNIEnv* env, jlongArray arr, jsize start, jsize len, jlong* data);
void      KaffeJNI_SetFloatArrayRegion(JNIEnv* env, jfloatArray arr, jsize start, jsize len, jfloat* data);
void      KaffeJNI_SetDoubleArrayRegion(JNIEnv* env, jdoubleArray arr, jsize start, jsize len, jdouble* data);

/* ====== Functions to get fields =================================================== */

jobject   KaffeJNI_GetObjectField(JNIEnv* env, jobject obj, jfieldID fld);
jboolean  KaffeJNI_GetBooleanField(JNIEnv* env, jobject obj, jfieldID fld);
jbyte     KaffeJNI_GetByteField(JNIEnv* env, jobject obj, jfieldID fld);
jchar     KaffeJNI_GetCharField(JNIEnv* env, jobject obj, jfieldID fld);
jshort    KaffeJNI_GetShortField(JNIEnv* env, jobject obj, jfieldID fld);
jint      KaffeJNI_GetIntField(JNIEnv* env, jobject obj, jfieldID fld);
jlong     KaffeJNI_GetLongField(JNIEnv* env, jobject obj, jfieldID fld);
jfloat    KaffeJNI_GetFloatField(JNIEnv* env, jobject obj, jfieldID fld);
jdouble   KaffeJNI_GetDoubleField(JNIEnv* env, jobject obj, jfieldID fld);

/* ====== Functions to set fields =================================================== */

void      KaffeJNI_SetObjectField(JNIEnv* env, jobject obj, jfieldID fld, jobject val);
void      KaffeJNI_SetBooleanField(JNIEnv* env, jobject obj, jfieldID fld, jboolean val);
void      KaffeJNI_SetByteField(JNIEnv* env, jobject obj, jfieldID fld, jbyte val);
void      KaffeJNI_SetCharField(JNIEnv* env, jobject obj, jfieldID fld, jchar val);
void      KaffeJNI_SetShortField(JNIEnv* env, jobject obj, jfieldID fld, jshort val);
void      KaffeJNI_SetIntField(JNIEnv* env, jobject obj, jfieldID fld, jint val);
void      KaffeJNI_SetLongField(JNIEnv* env, jobject obj, jfieldID fld, jlong val);
void      KaffeJNI_SetFloatField(JNIEnv* env, jobject obj, jfieldID fld, jfloat val);
void      KaffeJNI_SetDoubleField(JNIEnv* env, jobject obj, jfieldID fld, jdouble val);

/* ====== Functions to get static fields ============================================ */

jobject   KaffeJNI_GetStaticObjectField(JNIEnv* env, jclass cls, jfieldID fld);
jboolean  KaffeJNI_GetStaticBooleanField(JNIEnv* env, jclass cls, jfieldID fld);
jbyte     KaffeJNI_GetStaticByteField(JNIEnv* env, jclass cls, jfieldID fld);
jchar     KaffeJNI_GetStaticCharField(JNIEnv* env, jclass cls, jfieldID fld);
jshort    KaffeJNI_GetStaticShortField(JNIEnv* env, jclass cls, jfieldID fld);
jint      KaffeJNI_GetStaticIntField(JNIEnv* env, jclass cls, jfieldID fld);
jlong     KaffeJNI_GetStaticLongField(JNIEnv* env, jclass cls, jfieldID fld);
jfloat    KaffeJNI_GetStaticFloatField(JNIEnv* env, jclass cls, jfieldID fld);
jdouble   KaffeJNI_GetStaticDoubleField(JNIEnv* env, jclass cls, jfieldID fld);

/* ====== Functions to set static fields ============================================ */

void     KaffeJNI_SetStaticObjectField(JNIEnv* env, jclass cls, jfieldID fld, jobject val);
void     KaffeJNI_SetStaticBooleanField(JNIEnv* env, jclass cls, jfieldID fld, jboolean val);
void     KaffeJNI_SetStaticByteField(JNIEnv* env, jclass cls, jfieldID fld, jbyte val);
void     KaffeJNI_SetStaticCharField(JNIEnv* env, jclass cls, jfieldID fld, jchar val);
void     KaffeJNI_SetStaticShortField(JNIEnv* env, jclass cls, jfieldID fld, jshort val);
void     KaffeJNI_SetStaticIntField(JNIEnv* env, jclass cls, jfieldID fld, jint val);
void     KaffeJNI_SetStaticLongField(JNIEnv* env, jclass cls, jfieldID fld, jlong val);
void     KaffeJNI_SetStaticFloatField(JNIEnv* env, jclass cls, jfieldID fld, jfloat val);
void     KaffeJNI_SetStaticDoubleField(JNIEnv* env, jclass cls, jfieldID fld, jdouble val);

/* ====== Functions to call methods ================================================= */

jobject  KaffeJNI_CallNonvirtualObjectMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jobject  KaffeJNI_CallNonvirtualObjectMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jobject  KaffeJNI_CallNonvirtualObjectMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

jboolean KaffeJNI_CallNonvirtualBooleanMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jboolean KaffeJNI_CallNonvirtualBooleanMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jboolean KaffeJNI_CallNonvirtualBooleanMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

jbyte    KaffeJNI_CallNonvirtualByteMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jbyte    KaffeJNI_CallNonvirtualByteMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jbyte    KaffeJNI_CallNonvirtualByteMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

jchar    KaffeJNI_CallNonvirtualCharMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jchar    KaffeJNI_CallNonvirtualCharMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jchar    KaffeJNI_CallNonvirtualCharMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

jshort   KaffeJNI_CallNonvirtualShortMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jshort   KaffeJNI_CallNonvirtualShortMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jshort   KaffeJNI_CallNonvirtualShortMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

jint     KaffeJNI_CallNonvirtualIntMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jint     KaffeJNI_CallNonvirtualIntMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jint     KaffeJNI_CallNonvirtualIntMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

void     KaffeJNI_CallVoidMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
void     KaffeJNI_CallVoidMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
void     KaffeJNI_CallVoidMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jobject  KaffeJNI_CallObjectMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jobject  KaffeJNI_CallObjectMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jobject  KaffeJNI_CallObjectMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jboolean KaffeJNI_CallBooleanMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jboolean KaffeJNI_CallBooleanMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jboolean KaffeJNI_CallBooleanMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jbyte    KaffeJNI_CallByteMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jbyte    KaffeJNI_CallByteMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jbyte    KaffeJNI_CallByteMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jchar    KaffeJNI_CallCharMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jchar    KaffeJNI_CallCharMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jchar    KaffeJNI_CallCharMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jshort   KaffeJNI_CallShortMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jshort   KaffeJNI_CallShortMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jshort   KaffeJNI_CallShortMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jint     KaffeJNI_CallIntMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jint     KaffeJNI_CallIntMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jint     KaffeJNI_CallIntMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jlong    KaffeJNI_CallLongMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jlong    KaffeJNI_CallLongMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jlong    KaffeJNI_CallLongMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jfloat   KaffeJNI_CallFloatMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jfloat   KaffeJNI_CallFloatMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jfloat   KaffeJNI_CallFloatMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jdouble  KaffeJNI_CallDoubleMethodV(JNIEnv* env, jobject obj, jmethodID meth, va_list args);
jdouble  KaffeJNI_CallDoubleMethod(JNIEnv* env, jobject obj, jmethodID meth, ...);
jdouble  KaffeJNI_CallDoubleMethodA(JNIEnv* env, jobject obj, jmethodID meth, jvalue* args);

jlong    KaffeJNI_CallNonvirtualLongMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jlong    KaffeJNI_CallNonvirtualLongMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jlong    KaffeJNI_CallNonvirtualLongMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

jfloat   KaffeJNI_CallNonvirtualFloatMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jfloat   KaffeJNI_CallNonvirtualFloatMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jfloat   KaffeJNI_CallNonvirtualFloatMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

jdouble  KaffeJNI_CallNonvirtualDoubleMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
jdouble  KaffeJNI_CallNonvirtualDoubleMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
jdouble  KaffeJNI_CallNonvirtualDoubleMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

void     KaffeJNI_CallNonvirtualVoidMethodV(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, va_list args);
void     KaffeJNI_CallNonvirtualVoidMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...);
void     KaffeJNI_CallNonvirtualVoidMethodA(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, jvalue* args);

void     KaffeJNI_CallStaticVoidMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
void     KaffeJNI_CallStaticVoidMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
void     KaffeJNI_CallStaticVoidMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jchar    KaffeJNI_CallStaticCharMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jchar    KaffeJNI_CallStaticCharMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jchar    KaffeJNI_CallStaticCharMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jbyte    KaffeJNI_CallStaticByteMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jbyte    KaffeJNI_CallStaticByteMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jbyte    KaffeJNI_CallStaticByteMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jshort   KaffeJNI_CallStaticShortMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jshort   KaffeJNI_CallStaticShortMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jshort   KaffeJNI_CallStaticShortMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jint     KaffeJNI_CallStaticIntMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jint     KaffeJNI_CallStaticIntMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jint     KaffeJNI_CallStaticIntMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jlong    KaffeJNI_CallStaticLongMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jlong    KaffeJNI_CallStaticLongMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jlong    KaffeJNI_CallStaticLongMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jfloat   KaffeJNI_CallStaticFloatMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jfloat   KaffeJNI_CallStaticFloatMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jfloat   KaffeJNI_CallStaticFloatMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jdouble  KaffeJNI_CallStaticDoubleMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jdouble  KaffeJNI_CallStaticDoubleMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jdouble  KaffeJNI_CallStaticDoubleMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jboolean KaffeJNI_CallStaticBooleanMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jboolean KaffeJNI_CallStaticBooleanMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jboolean KaffeJNI_CallStaticBooleanMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

jobject  KaffeJNI_CallStaticObjectMethodV(JNIEnv* env, jclass cls, jmethodID meth, va_list args);
jobject  KaffeJNI_CallStaticObjectMethod(JNIEnv* env, jclass cls, jmethodID meth, ...);
jobject  KaffeJNI_CallStaticObjectMethodA(JNIEnv* env, jclass cls, jmethodID meth, jvalue* args);

/* ====== Functions to handle java references ======================================= */

void     KaffeJNI_DeleteGlobalRef(JNIEnv* env, jref obj);
void     KaffeJNI_DeleteLocalRef(JNIEnv* env, jref obj);
jboolean KaffeJNI_IsSameObject(JNIEnv* env, jobject obj1, jobject obj2);
jref     KaffeJNI_NewGlobalRef(JNIEnv* env, jref obj);
jint     KaffeJNI_EnsureLocalCapacity(JNIEnv* env, jint capacity);
jobject  KaffeJNI_PopLocalFrame(JNIEnv* env, jobject obj);
jobject  KaffeJNI_NewLocalRef(JNIEnv* env, jobject ref);
jweak    KaffeJNI_NewWeakGlobalRef(JNIEnv *env, jobject obj);
void     KaffeJNI_DeleteWeakGlobalRef(JNIEnv *env, jweak obj);
jint     KaffeJNI_PushLocalFrame(JNIEnv* env, jint capacity);

#endif
