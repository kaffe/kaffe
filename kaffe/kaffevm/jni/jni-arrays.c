/*
 * jni-arrays.c
 * Java Native Interface - Array handling JNI functions.
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
#include "jni_i.h"
#include "jni.h"
#include "jni_funcs.h"
#include "Arrays.h"
#include "jnirefs.h"
#include "exception.h"
#include "object.h"

jobject
KaffeJNI_GetObjectArrayElement(JNIEnv* env UNUSED, jobjectArray arr, jsize elem)
{
  jobject obj;

  BEGIN_EXCEPTION_HANDLING(NULL);

  if (elem >= (jsize)obj_length((HArrayOfObject*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  obj = unhand_array((HArrayOfObject*)arr)->body[elem];

  ADD_REF(obj);
  END_EXCEPTION_HANDLING();
  return (obj);
}

void
KaffeJNI_SetObjectArrayElement(JNIEnv* env UNUSED, jobjectArray arr, jsize elem, jobject val)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (elem >= (jsize)obj_length((HArrayOfObject*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  unhand_array((HArrayOfObject*)arr)->body[elem] = (Hjava_lang_Object*)val;

  END_EXCEPTION_HANDLING();
}

jobjectArray
KaffeJNI_NewObjectArray(JNIEnv* env UNUSED, jsize len, jclass cls, jobject init)
{
  HArrayOfObject* obj;
  jsize i;

  BEGIN_EXCEPTION_HANDLING(NULL);

  obj = (HArrayOfObject*)newArray((Hjava_lang_Class*)cls, len);

  /* I assume this is what init is for */
  for (i = 0; i < len; i++) {
    unhand_array(obj)->body[i] = (Hjava_lang_Object*)init;
  }

  ADD_REF(obj);
  END_EXCEPTION_HANDLING();
  return (obj);
}

jbooleanArray
KaffeJNI_NewBooleanArray(JNIEnv* env UNUSED, jsize len)
{
  jbooleanArray arr;

  BEGIN_EXCEPTION_HANDLING(NULL);

  arr = newArray(booleanClass, len);

  ADD_REF(arr);
  END_EXCEPTION_HANDLING();
  return (arr);
}

jbyteArray
KaffeJNI_NewByteArray(JNIEnv* env UNUSED, jsize len)
{
  jbyteArray arr;

  BEGIN_EXCEPTION_HANDLING(NULL);

  arr = newArray(byteClass, len);

  ADD_REF(arr);
  END_EXCEPTION_HANDLING();
  return (arr);
}

jcharArray
KaffeJNI_NewCharArray(JNIEnv* env UNUSED, jsize len)
{
  jcharArray arr;

  BEGIN_EXCEPTION_HANDLING(NULL);

  arr = newArray(charClass, len);

  ADD_REF(arr);
  END_EXCEPTION_HANDLING();
  return (arr);
}

jshortArray
KaffeJNI_NewShortArray(JNIEnv* env UNUSED, jsize len)
{
  jshortArray arr;

  BEGIN_EXCEPTION_HANDLING(NULL);

  arr = newArray(shortClass, len);

  ADD_REF(arr);
  END_EXCEPTION_HANDLING();
  return (arr);
}

jintArray
KaffeJNI_NewIntArray(JNIEnv* env UNUSED, jsize len)
{
  jintArray arr;

  BEGIN_EXCEPTION_HANDLING(NULL);

  arr = newArray(intClass, len);

  ADD_REF(arr);
  END_EXCEPTION_HANDLING();
  return (arr);
}

jlongArray
KaffeJNI_NewLongArray(JNIEnv* env UNUSED, jsize len)
{
  jlongArray arr;

  BEGIN_EXCEPTION_HANDLING(NULL);

  arr = newArray(longClass, len);

  ADD_REF(arr);
  END_EXCEPTION_HANDLING();
  return (arr);
}

jfloatArray
KaffeJNI_NewFloatArray(JNIEnv* env UNUSED, jsize len)
{
  jfloatArray arr;

  BEGIN_EXCEPTION_HANDLING(NULL);

  arr = newArray(floatClass, len);

  ADD_REF(arr);
  END_EXCEPTION_HANDLING();
  return (arr);
}

jdoubleArray
KaffeJNI_NewDoubleArray(JNIEnv* env UNUSED, jsize len)
{
  jdoubleArray arr;

  BEGIN_EXCEPTION_HANDLING(NULL);

  arr = newArray(doubleClass, len);

  ADD_REF(arr);
  END_EXCEPTION_HANDLING();
  return (arr);
}

jboolean*
KaffeJNI_GetBooleanArrayElements(JNIEnv* env UNUSED, jbooleanArray arr, jboolean* iscopy)
{
  jboolean* array;
  BEGIN_EXCEPTION_HANDLING(NULL);

  if (iscopy != NULL) {
    *iscopy = JNI_FALSE;
  }
  array = unhand_array((HArrayOfBoolean*)arr)->body;

  END_EXCEPTION_HANDLING();
  return (array);
}

jbyte*
KaffeJNI_GetByteArrayElements(JNIEnv* env UNUSED, jbyteArray arr, jboolean* iscopy)
{
  jbyte* array;
  BEGIN_EXCEPTION_HANDLING(NULL);

  if (iscopy != NULL) {
    *iscopy = JNI_FALSE;
  }
  array = unhand_array((HArrayOfByte*)arr)->body;

  END_EXCEPTION_HANDLING();
  return (array);
}

void*
KaffeJNI_GetPrimitiveArrayCritical(JNIEnv* env, jarray arr, jboolean* iscopy)
{
  return (KaffeJNI_GetByteArrayElements(env, (jbyteArray)arr, iscopy));
}

jchar*
KaffeJNI_GetCharArrayElements(JNIEnv* env UNUSED, jcharArray arr, jboolean* iscopy)
{
  jchar* array;
  BEGIN_EXCEPTION_HANDLING(NULL);

  if (iscopy != NULL) {
    *iscopy = JNI_FALSE;
  }
  array = unhand_array((HArrayOfChar*)arr)->body;

  END_EXCEPTION_HANDLING();
  return (array);
}

jshort*
KaffeJNI_GetShortArrayElements(JNIEnv* env UNUSED, jshortArray arr, jboolean* iscopy)
{
  jshort* array;
  BEGIN_EXCEPTION_HANDLING(NULL);

  if (iscopy != NULL) {
    *iscopy = JNI_FALSE;
  }
  array = unhand_array((HArrayOfShort*)arr)->body;

  END_EXCEPTION_HANDLING();
  return (array);
}

jint*
KaffeJNI_GetIntArrayElements(JNIEnv* env UNUSED, jintArray arr, jboolean* iscopy)
{
  jint* array;
  BEGIN_EXCEPTION_HANDLING(NULL);

  if (iscopy != NULL) {
    *iscopy = JNI_FALSE;
  }
  array = unhand_array((HArrayOfInt*)arr)->body;

  END_EXCEPTION_HANDLING();
  return (array);
}

jlong*
KaffeJNI_GetLongArrayElements(JNIEnv* env UNUSED, jlongArray arr, jboolean* iscopy)
{
  jlong* array;
  BEGIN_EXCEPTION_HANDLING(NULL);

  if (iscopy != NULL) {
    *iscopy = JNI_FALSE;
  }
  array = unhand_array((HArrayOfLong*)arr)->body;

  END_EXCEPTION_HANDLING();
  return (array);
}

jfloat*
KaffeJNI_GetFloatArrayElements(JNIEnv* env UNUSED, jfloatArray arr, jboolean* iscopy)
{
  jfloat* array;
  BEGIN_EXCEPTION_HANDLING(NULL);

  if (iscopy != NULL) {
    *iscopy = JNI_FALSE;
  }
  array = unhand_array((HArrayOfFloat*)arr)->body;

  END_EXCEPTION_HANDLING();
  return (array);
}

jdouble*
KaffeJNI_GetDoubleArrayElements(JNIEnv* env UNUSED, jdoubleArray arr, jboolean* iscopy)
{
  jdouble* array;
  BEGIN_EXCEPTION_HANDLING(NULL);

  if (iscopy != NULL) {
    *iscopy = JNI_FALSE;
  }
  array = unhand_array((HArrayOfDouble*)arr)->body;

  END_EXCEPTION_HANDLING();
  return (array);
}

void
KaffeJNI_ReleaseBooleanArrayElements(JNIEnv* env UNUSED, jbooleanArray arr, jboolean* elems, jint mode)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (elems != unhand_array((HArrayOfBoolean*)arr)->body) {
    switch (mode) {
    case JNI_COMMIT:
      memcpy(unhand_array((HArrayOfBoolean*)arr)->body, elems, obj_length((HArrayOfBoolean*)arr) * sizeof(jboolean));
      break;
    case 0:
      memcpy(unhand_array((HArrayOfBoolean*)arr)->body, elems, obj_length((HArrayOfBoolean*)arr) * sizeof(jboolean));
      KFREE(elems);
      break;
    case JNI_ABORT:
      KFREE(elems);
      break;
    }
  }
  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_ReleaseByteArrayElements(JNIEnv* env UNUSED, jbyteArray arr, jbyte* elems, jint mode)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (elems != unhand_array((HArrayOfByte*)arr)->body) {
    switch (mode) {
    case JNI_COMMIT:
      memcpy(unhand_array((HArrayOfByte*)arr)->body, elems, obj_length((HArrayOfByte*)arr) * sizeof(jbyte));
      break;
    case 0:
      memcpy(unhand_array((HArrayOfByte*)arr)->body, elems, obj_length((HArrayOfByte*)arr) * sizeof(jbyte));
      KFREE(elems);
      break;
    case JNI_ABORT:
      KFREE(elems);
      break;
    }
  }
  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_ReleasePrimitiveArrayCritical(JNIEnv* env, jbyteArray arr, void* elems, jint mode)
{
  KaffeJNI_ReleaseByteArrayElements(env, (jbyteArray)arr, (jbyte*)elems, mode);
}

void
KaffeJNI_ReleaseCharArrayElements(JNIEnv* env UNUSED, jcharArray arr, jchar* elems, jint mode)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (elems != unhand_array((HArrayOfChar*)arr)->body) {
    switch (mode) {
    case JNI_COMMIT:
      memcpy(unhand_array((HArrayOfChar*)arr)->body, elems, obj_length((HArrayOfChar*)arr) * sizeof(jchar));
      break;
    case 0:
      memcpy(unhand_array((HArrayOfChar*)arr)->body, elems, obj_length((HArrayOfChar*)arr) * sizeof(jchar));
      KFREE(elems);
      break;
    case JNI_ABORT:
      KFREE(elems);
      break;
    }
  }
  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_ReleaseShortArrayElements(JNIEnv* env UNUSED, jshortArray arr, jshort* elems, jint mode)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (elems != unhand_array((HArrayOfShort*)arr)->body) {
    switch (mode) {
    case JNI_COMMIT:
      memcpy(unhand_array((HArrayOfShort*)arr)->body, elems, obj_length((HArrayOfShort*)arr) * sizeof(jshort));
      break;
    case 0:
      memcpy(unhand_array((HArrayOfShort*)arr)->body, elems, obj_length((HArrayOfShort*)arr) * sizeof(jshort));
      KFREE(elems);
      break;
    case JNI_ABORT:
      KFREE(elems);
      break;
    }
  }
  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_ReleaseIntArrayElements(JNIEnv* env UNUSED, jintArray arr, jint* elems, jint mode)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (elems != unhand_array((HArrayOfInt*)arr)->body) {
    switch (mode) {
    case JNI_COMMIT:
      memcpy(unhand_array((HArrayOfInt*)arr)->body, elems, obj_length((HArrayOfInt*)arr) * sizeof(jint));
      break;
    case 0:
      memcpy(unhand_array((HArrayOfInt*)arr)->body, elems, obj_length((HArrayOfInt*)arr) * sizeof(jint));
      KFREE(elems);
      break;
    case JNI_ABORT:
      KFREE(elems);
      break;
    }
  }
  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_ReleaseLongArrayElements(JNIEnv* env UNUSED, jlongArray arr, jlong* elems, jint mode)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (elems != unhand_array((HArrayOfLong*)arr)->body) {
    switch (mode) {
    case JNI_COMMIT:
      memcpy(unhand_array((HArrayOfLong*)arr)->body, elems, obj_length((HArrayOfLong*)arr) * sizeof(jlong));
      break;
    case 0:
      memcpy(unhand_array((HArrayOfLong*)arr)->body, elems, obj_length((HArrayOfLong*)arr) * sizeof(jlong));
      KFREE(elems);
      break;
    case JNI_ABORT:
      KFREE(elems);
      break;
    }
  }
  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_ReleaseFloatArrayElements(JNIEnv* env UNUSED, jfloatArray arr, jfloat* elems, jint mode)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (elems != unhand_array((HArrayOfFloat*)arr)->body) {
    switch (mode) {
    case JNI_COMMIT:
      memcpy(unhand_array((HArrayOfFloat*)arr)->body, elems, obj_length((HArrayOfFloat*)arr) * sizeof(jfloat));
      break;
    case 0:
      memcpy(unhand_array((HArrayOfFloat*)arr)->body, elems, obj_length((HArrayOfFloat*)arr) * sizeof(jfloat));
      KFREE(elems);
      break;
    case JNI_ABORT:
      KFREE(elems);
      break;
    }
  }
  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_ReleaseDoubleArrayElements(JNIEnv* env UNUSED, jdoubleArray arr, jdouble* elems, jint mode)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (elems != unhand_array((HArrayOfDouble*)arr)->body) {
    switch (mode) {
    case JNI_COMMIT:
      memcpy(unhand_array((HArrayOfDouble*)arr)->body, elems, obj_length((HArrayOfDouble*)arr) * sizeof(jdouble));
      break;
    case 0:
      memcpy(unhand_array((HArrayOfDouble*)arr)->body, elems, obj_length((HArrayOfDouble*)arr) * sizeof(jdouble));
      KFREE(elems);
      break;
    case JNI_ABORT:
      KFREE(elems);
      break;
    }
  }
  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_GetBooleanArrayRegion(JNIEnv* env UNUSED, jbooleanArray arr, jsize start, jsize len, jboolean* data)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (start >= obj_length((HArrayOfBoolean*)arr) || start + len > obj_length((HArrayOfBoolean*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  memcpy(data, &unhand_array((HArrayOfBoolean*)arr)->body[start], len * sizeof(jboolean));

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_GetByteArrayRegion(JNIEnv* env UNUSED, jbyteArray arr, jsize start, jsize len, jbyte* data)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (start >= obj_length((HArrayOfByte*)arr) || start + len > obj_length((HArrayOfByte*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  memcpy(data, &unhand_array((HArrayOfByte*)arr)->body[start], len * sizeof(jbyte));

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_GetCharArrayRegion(JNIEnv* env UNUSED, jcharArray arr, jsize start, jsize len, jchar* data)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (start >= obj_length((HArrayOfChar*)arr) || start + len > obj_length((HArrayOfChar*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  memcpy(data, &unhand_array((HArrayOfChar*)arr)->body[start], len * sizeof(jchar));

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_GetShortArrayRegion(JNIEnv* env UNUSED, jshortArray arr, jsize start, jsize len, jshort* data)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (start >= obj_length((HArrayOfShort*)arr) || start + len > obj_length((HArrayOfShort*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  memcpy(data, &unhand_array((HArrayOfShort*)arr)->body[start], len * sizeof(jshort));

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_GetIntArrayRegion(JNIEnv* env UNUSED, jintArray arr, jsize start, jsize len, jint* data)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (start >= obj_length((HArrayOfInt*)arr) || start + len > obj_length((HArrayOfInt*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  memcpy(data, &unhand_array((HArrayOfInt*)arr)->body[start], len * sizeof(jint));

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_GetLongArrayRegion(JNIEnv* env UNUSED, jlongArray arr, jsize start, jsize len, jlong* data)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (start >= obj_length((HArrayOfLong*)arr) || start + len > obj_length((HArrayOfLong*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  memcpy(data, &unhand_array((HArrayOfLong*)arr)->body[start], len * sizeof(jlong));

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_GetFloatArrayRegion(JNIEnv* env UNUSED, jfloatArray arr, jsize start, jsize len, jfloat* data)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (start >= obj_length((HArrayOfFloat*)arr) || start + len > obj_length((HArrayOfFloat*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  memcpy(data, &unhand_array((HArrayOfFloat*)arr)->body[start], len * sizeof(jfloat));

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_GetDoubleArrayRegion(JNIEnv* env UNUSED, jdoubleArray arr, jsize start, jsize len, jdouble* data)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (start >= obj_length((HArrayOfDouble*)arr) || start + len > obj_length((HArrayOfDouble*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  memcpy(data, &unhand_array((HArrayOfDouble*)arr)->body[start], len * sizeof(jdouble));

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetBooleanArrayRegion(JNIEnv* env UNUSED, jbooleanArray arr, jsize start, jsize len, jboolean* data)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (start >= obj_length((HArrayOfBoolean*)arr) || start+len > obj_length((HArrayOfBoolean*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  memcpy(&unhand_array((HArrayOfBoolean*)arr)->body[start], data, len * sizeof(jboolean));

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetByteArrayRegion(JNIEnv* env UNUSED, jbyteArray arr, jsize start, jsize len, jbyte* data)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (start >= obj_length((HArrayOfByte*)arr) || start+len > obj_length((HArrayOfByte*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  memcpy(&unhand_array((HArrayOfByte*)arr)->body[start], data, len * sizeof(jbyte));

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetCharArrayRegion(JNIEnv* env UNUSED, jcharArray arr, jsize start, jsize len, jchar* data)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (start >= obj_length((HArrayOfChar*)arr) || start+len > obj_length((HArrayOfChar*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  memcpy(&unhand_array((HArrayOfChar*)arr)->body[start], data, len * sizeof(jchar));

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetShortArrayRegion(JNIEnv* env UNUSED, jshortArray arr, jsize start, jsize len, jshort* data)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (start >= obj_length((HArrayOfShort*)arr) || start+len > obj_length((HArrayOfShort*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  memcpy(&unhand_array((HArrayOfShort*)arr)->body[start], data, len * sizeof(jshort));

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetIntArrayRegion(JNIEnv* env UNUSED, jintArray arr, jsize start, jsize len, jint* data)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (start >= obj_length((HArrayOfInt*)arr) || start+len > obj_length((HArrayOfInt*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  memcpy(&unhand_array((HArrayOfInt*)arr)->body[start], data, len * sizeof(jint));

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetLongArrayRegion(JNIEnv* env UNUSED, jlongArray arr, jsize start, jsize len, jlong* data)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (start >= obj_length((HArrayOfLong*)arr) || start+len > obj_length((HArrayOfLong*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  memcpy(&unhand_array((HArrayOfLong*)arr)->body[start], data, len * sizeof(jlong));

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetFloatArrayRegion(JNIEnv* env UNUSED, jfloatArray arr, jsize start, jsize len, jfloat* data)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (start >= obj_length((HArrayOfFloat*)arr) || start+len > obj_length((HArrayOfFloat*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  memcpy(&unhand_array((HArrayOfFloat*)arr)->body[start], data, len * sizeof(jfloat));

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetDoubleArrayRegion(JNIEnv* env UNUSED, jdoubleArray arr, jsize start, jsize len, jdouble* data)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  if (start >= obj_length((HArrayOfDouble*)arr) || start+len > obj_length((HArrayOfDouble*)arr)) {
    throwException(ArrayIndexOutOfBoundsException);
  }
  memcpy(&unhand_array((HArrayOfDouble*)arr)->body[start], data, len * sizeof(jdouble));

  END_EXCEPTION_HANDLING();
}
