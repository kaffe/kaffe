/*
 * jni-fields.c
 * Java Native Interface - Get/SetXField JNI functions.
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

/*
 * Get and set fields.
 */
#include "config.h"
#include "jni_i.h"
#include "jni.h"
#include "classMethod.h"
#include "jni_funcs.h"

#define	GET_FIELD(T,O,F)	*(T*)((char*)(O) + FIELD_BOFFSET((Field*)(F)))
#define	SET_FIELD(T,O,F,V)	*(T*)((char*)(O) + FIELD_BOFFSET((Field*)(F))) = (V)
#define	GET_STATIC_FIELD(T,F)	*(T*)FIELD_ADDRESS((Field*)F)
#define	SET_STATIC_FIELD(T,F,V)	*(T*)FIELD_ADDRESS((Field*)F) = (V)

/* ------------------------------------------------------------------------------
 * GetField functions
 * ------------------------------------------------------------------------------
 */

jobject
KaffeJNI_GetObjectField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
  jobject nobj;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(NULL);

  obj_local = unveil(obj);
  nobj = GET_FIELD(jobject, obj_local, fld);

  ADD_REF(nobj);
  END_EXCEPTION_HANDLING();
  return (nobj);
}

jboolean
KaffeJNI_GetBooleanField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
  jboolean r;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  r = GET_FIELD(jboolean, obj_local, fld);

  END_EXCEPTION_HANDLING();
  return (r);
}

jbyte
KaffeJNI_GetByteField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
  jbyte r;
  jobject obj_local;
  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  r = GET_FIELD(jbyte, obj_local, fld);

  END_EXCEPTION_HANDLING();
  return (r);
}

jchar
KaffeJNI_GetCharField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
  jchar r;
  jobject obj_local;
  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  r = GET_FIELD(jchar, obj_local, fld);

  END_EXCEPTION_HANDLING();
  return (r);
}

jshort
KaffeJNI_GetShortField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
  jshort r;
  jobject obj_local;
  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  r = GET_FIELD(jshort, obj_local, fld);

  END_EXCEPTION_HANDLING();
  return (r);
}

jint
KaffeJNI_GetIntField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
  jint r;
  jobject obj_local;
  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  r = GET_FIELD(jint, obj_local, fld);

  END_EXCEPTION_HANDLING();
  return (r);
}

jlong
KaffeJNI_GetLongField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
  jlong r;
  jobject obj_local;
  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  r = GET_FIELD(jlong, obj_local, fld);

  END_EXCEPTION_HANDLING();
  return (r);
}

jfloat
KaffeJNI_GetFloatField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
  jfloat r;
  jobject obj_local;
  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  r = GET_FIELD(jfloat, obj_local, fld);

  END_EXCEPTION_HANDLING();
  return (r);
}

jdouble
KaffeJNI_GetDoubleField(JNIEnv* env UNUSED, jobject obj, jfieldID fld)
{
  jdouble r;
  jobject obj_local;
  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  r = GET_FIELD(jdouble, obj_local, fld);

  END_EXCEPTION_HANDLING();
  return (r);
}

/* ------------------------------------------------------------------------------
 * SetField functions
 * ------------------------------------------------------------------------------
 */

void
KaffeJNI_SetObjectField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jobject val)
{
  jobject obj_local, val_local;

  BEGIN_EXCEPTION_HANDLING_VOID();

  obj_local = unveil(obj);
  val_local = unveil(val);
  SET_FIELD(jobject, obj_local, fld, val_local);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetBooleanField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jboolean val)
{
  jobject obj_local;
  BEGIN_EXCEPTION_HANDLING_VOID();

  obj_local = unveil(obj);
  SET_FIELD(jboolean, obj_local, fld, val);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetByteField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jbyte val)
{
  jobject obj_local;
  BEGIN_EXCEPTION_HANDLING_VOID();

  obj_local = unveil(obj);
  SET_FIELD(jbyte, obj_local, fld, val);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetCharField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jchar val)
{
  jobject obj_local;
  BEGIN_EXCEPTION_HANDLING_VOID();

  obj_local = unveil(obj);
  SET_FIELD(jchar, obj_local, fld, val);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetShortField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jshort val)
{
  jobject obj_local;
  BEGIN_EXCEPTION_HANDLING_VOID();

  obj_local = unveil(obj);
  SET_FIELD(jshort, obj_local, fld, val);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetIntField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jint val)
{
  jobject obj_local;
  BEGIN_EXCEPTION_HANDLING_VOID();

  obj_local = unveil(obj);
  SET_FIELD(jint, obj_local, fld, val);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetLongField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jlong val)
{
  jobject obj_local;
  BEGIN_EXCEPTION_HANDLING_VOID();

  obj_local = unveil(obj);
  SET_FIELD(jlong, obj_local, fld, val);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetFloatField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jfloat val)
{
  jobject obj_local;
  BEGIN_EXCEPTION_HANDLING_VOID();

  obj_local = unveil(obj);
  SET_FIELD(jfloat, obj_local, fld, val);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetDoubleField(JNIEnv* env UNUSED, jobject obj, jfieldID fld, jdouble val)
{
  jobject obj_local;
  BEGIN_EXCEPTION_HANDLING_VOID();

  obj_local = unveil(obj);
  SET_FIELD(jdouble, obj_local, fld, val);

  END_EXCEPTION_HANDLING();
}


/* ------------------------------------------------------------------------------
 * GetStaticField functions
 * ------------------------------------------------------------------------------
 */

jobject
KaffeJNI_GetStaticObjectField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
  jobject obj;

  BEGIN_EXCEPTION_HANDLING(NULL);

  obj = GET_STATIC_FIELD(jobject, fld);

  ADD_REF(obj);
  END_EXCEPTION_HANDLING();
  return (obj);
}

jboolean
KaffeJNI_GetStaticBooleanField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
  jboolean r;
  BEGIN_EXCEPTION_HANDLING(0);

  r = GET_STATIC_FIELD(jboolean, fld);

  END_EXCEPTION_HANDLING();
  return (r);
}

jbyte
KaffeJNI_GetStaticByteField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
  jbyte r;
  BEGIN_EXCEPTION_HANDLING(0);

  r = GET_STATIC_FIELD(jbyte, fld);

  END_EXCEPTION_HANDLING();
  return (r);
}

jchar
KaffeJNI_GetStaticCharField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
  jchar r;
  BEGIN_EXCEPTION_HANDLING(0);

  r = GET_STATIC_FIELD(jchar, fld);

  END_EXCEPTION_HANDLING();
  return (r);
}

jshort
KaffeJNI_GetStaticShortField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
  jint r;
  BEGIN_EXCEPTION_HANDLING(0);

  r = GET_STATIC_FIELD(jshort, fld);

  END_EXCEPTION_HANDLING();
  return (r);
}

jint
KaffeJNI_GetStaticIntField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
  jint r;
  BEGIN_EXCEPTION_HANDLING(0);

  r = GET_STATIC_FIELD(jint, fld);

  END_EXCEPTION_HANDLING();
  return (r);
}

jlong
KaffeJNI_GetStaticLongField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
  jlong r;
  BEGIN_EXCEPTION_HANDLING(0);

  r = GET_STATIC_FIELD(jlong, fld);

  END_EXCEPTION_HANDLING();
  return (r);
}

jfloat
KaffeJNI_GetStaticFloatField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
  jfloat r;
  BEGIN_EXCEPTION_HANDLING(0);

  r = GET_STATIC_FIELD(jfloat, fld);

  END_EXCEPTION_HANDLING();
  return (r);
}

jdouble
KaffeJNI_GetStaticDoubleField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld)
{
  jdouble r;
  BEGIN_EXCEPTION_HANDLING(0);

  r = GET_STATIC_FIELD(jdouble, fld);

  END_EXCEPTION_HANDLING();
  return (r);
}

/* ------------------------------------------------------------------------------
 * SetStaticField functions
 * ------------------------------------------------------------------------------
 */

void
KaffeJNI_SetStaticObjectField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jobject val)
{
  jobject val_local;
  BEGIN_EXCEPTION_HANDLING_VOID();

  val_local = unveil(val);
  SET_STATIC_FIELD(jobject, fld, val_local);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetStaticBooleanField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jboolean val)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  SET_STATIC_FIELD(jboolean, fld, val);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetStaticByteField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jbyte val)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  SET_STATIC_FIELD(jbyte, fld, val);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetStaticCharField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jchar val)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  SET_STATIC_FIELD(jchar, fld, val);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetStaticShortField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jshort val)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  SET_STATIC_FIELD(jshort, fld, val);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetStaticIntField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jint val)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  SET_STATIC_FIELD(jint, fld, val);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetStaticLongField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jlong val)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  SET_STATIC_FIELD(jlong, fld, val);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetStaticFloatField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jfloat val)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  SET_STATIC_FIELD(jfloat, fld, val);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_SetStaticDoubleField(JNIEnv* env UNUSED, jclass cls UNUSED, jfieldID fld, jdouble val)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  SET_STATIC_FIELD(jdouble, fld, val);

  END_EXCEPTION_HANDLING();
}
