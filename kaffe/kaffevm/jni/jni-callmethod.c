/*
 * jni-callmethod.c
 * Java Native Interface - CallXMethod JNI functions.
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
#include "baseClasses.h"
#include "threadData.h"
#include "classMethod.h"
#include "exception.h"
#include "jni.h"
#include "jni_i.h"
#include "jnirefs.h"
#include "jni_funcs.h"

/*
 * Find the function to be called when meth is invoked on obj
 */
static inline void*
getMethodFunc (Method* meth, Hjava_lang_Object *obj)
{
  if (obj && CLASS_IS_INTERFACE (meth->class)) {
    register void ***implementors;
    register Hjava_lang_Class *clazz;
		
    assert (meth->idx >= 0);

    implementors = meth->class->implementors;
    clazz = OBJECT_CLASS(obj);

    assert (implementors != NULL && clazz->impl_index <= (int)implementors[0]);

    return implementors[clazz->impl_index][meth->idx + 1]; 	
  } else {
    return meth->idx >= 0 ? obj->vtable->method[meth->idx] : METHOD_NATIVECODE (meth);
  }
}

/*
 * The JNI functions.
 */


/* ------------------------------------------------------------------------------
 * Virtual method calls returning VOID
 * ------------------------------------------------------------------------------
 */

void
KaffeJNI_CallVoidMethodV(JNIEnv* env UNUSED, jobject obj, jmethodID meth, va_list args)
{
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING_VOID();
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  /*
   * callMethodA will have to unveil the objects contained in the argument lists.
   * This is not really pretty but more efficient because we do not parse the signature
   * twice.
   */
  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, args, NULL);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_CallVoidMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
  va_list args;

  BEGIN_EXCEPTION_HANDLING_VOID();

  va_start(args, meth);
  KaffeJNI_CallVoidMethodV(env, obj, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_CallVoidMethodA(JNIEnv* env UNUSED, jobject obj, jmethodID meth, jvalue* args)
{
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING_VOID();
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  /*
   * callMethodA will have to unveil the objects contained in the argument lists.
   * This is not really pretty but more efficient because we do not parse the signature
   * twice.
   */
  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, args, NULL, 0);

  END_EXCEPTION_HANDLING();
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning Object
 * ------------------------------------------------------------------------------
 */

jobject
KaffeJNI_CallNonvirtualObjectMethodV(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(NULL);

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  obj_local = unveil(obj);
  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, args, &retval);

  ADD_REF(retval.l);
  END_EXCEPTION_HANDLING();
  return (retval.l);
}

jobject
KaffeJNI_CallNonvirtualObjectMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jobject r;

  BEGIN_EXCEPTION_HANDLING(NULL);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualObjectMethodV(env, obj, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jobject
KaffeJNI_CallNonvirtualObjectMethodA(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(NULL);
  
  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, args, &retval, 0);

  ADD_REF(retval.l);
  END_EXCEPTION_HANDLING();
  return (retval.l);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning boolean
 * ------------------------------------------------------------------------------
 */

jboolean
KaffeJNI_CallNonvirtualBooleanMethodV(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jboolean) retval.i);
}

jboolean
KaffeJNI_CallNonvirtualBooleanMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jboolean r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualBooleanMethodV(env, obj, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jboolean
KaffeJNI_CallNonvirtualBooleanMethodA(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jboolean) retval.i);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning byte
 * ------------------------------------------------------------------------------
 */
jbyte
KaffeJNI_CallNonvirtualByteMethodV(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jbyte) retval.i);
}

jbyte
KaffeJNI_CallNonvirtualByteMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jbyte r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualByteMethodV(env, obj, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jbyte
KaffeJNI_CallNonvirtualByteMethodA(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jbyte) retval.i);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning char
 * ------------------------------------------------------------------------------
 */

jchar
KaffeJNI_CallNonvirtualCharMethodV(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jchar) retval.i);
}

jchar
KaffeJNI_CallNonvirtualCharMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jchar r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualCharMethodV(env, obj, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jchar
KaffeJNI_CallNonvirtualCharMethodA(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jchar) retval.i);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning short
 * ------------------------------------------------------------------------------
 */

jshort
KaffeJNI_CallNonvirtualShortMethodV(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jshort) retval.i);
}

jshort
KaffeJNI_CallNonvirtualShortMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jshort r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualShortMethodV(env, obj, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jshort
KaffeJNI_CallNonvirtualShortMethodA(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jshort) retval.i);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning int
 * ------------------------------------------------------------------------------
 */

jint
KaffeJNI_CallNonvirtualIntMethodV(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.i);
}

jint
KaffeJNI_CallNonvirtualIntMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jint r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualIntMethodV(env, obj, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jint
KaffeJNI_CallNonvirtualIntMethodA(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  obj_local = unveil(obj);
  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.i);
}

/* ------------------------------------------------------------------------------
 * Virtual method calls returning Object
 * ------------------------------------------------------------------------------
 */

jobject

KaffeJNI_CallObjectMethodV(JNIEnv* env UNUSED, jobject obj, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(NULL);

  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, args, &retval);

  ADD_REF(retval.l);
  END_EXCEPTION_HANDLING();
  return (retval.l);
}


jobject
KaffeJNI_CallObjectMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
  va_list args;
  jobject r;

  BEGIN_EXCEPTION_HANDLING(NULL);

  va_start(args, meth);
  r = KaffeJNI_CallObjectMethodV(env, obj, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jobject
KaffeJNI_CallObjectMethodA(JNIEnv* env UNUSED, jobject obj, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(NULL);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.l);
}

/* ------------------------------------------------------------------------------
 * Virtual method calls returning boolean
 * ------------------------------------------------------------------------------
 */

jboolean
KaffeJNI_CallBooleanMethodV(JNIEnv* env UNUSED, jobject obj, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jboolean) retval.i);
}

jboolean
KaffeJNI_CallBooleanMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
  va_list args;
  jboolean r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallBooleanMethodV(env, obj, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jboolean
KaffeJNI_CallBooleanMethodA(JNIEnv* env UNUSED, jobject obj, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jboolean) retval.i);
}

/* ------------------------------------------------------------------------------
 * Virtual method calls returning byte
 * ------------------------------------------------------------------------------
 */

jbyte
KaffeJNI_CallByteMethodV(JNIEnv* env UNUSED, jobject obj, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
   o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jbyte) retval.i);
}

jbyte
KaffeJNI_CallByteMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
  va_list args;
  jbyte r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallByteMethodV(env, obj, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jbyte
KaffeJNI_CallByteMethodA(JNIEnv* env UNUSED, jobject obj, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), obj, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jbyte) retval.i);
}

/* ------------------------------------------------------------------------------
 * Virtual method calls returning char
 * ------------------------------------------------------------------------------
 */

jchar
KaffeJNI_CallCharMethodV(JNIEnv* env UNUSED, jobject obj, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jchar) retval.i);
}

jchar
KaffeJNI_CallCharMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
  va_list args;
  jchar r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallCharMethodV(env, obj, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jchar
KaffeJNI_CallCharMethodA(JNIEnv* env UNUSED, jobject obj, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jchar) retval.i);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning short
 * ------------------------------------------------------------------------------
 */

jshort
KaffeJNI_CallShortMethodV(JNIEnv* env UNUSED, jobject obj, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jshort) retval.i);
}

jshort
KaffeJNI_CallShortMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
  va_list args;
  jshort r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallShortMethodV(env, obj, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jshort
KaffeJNI_CallShortMethodA(JNIEnv* env UNUSED, jobject obj, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jshort) retval.i);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning int
 * ------------------------------------------------------------------------------
 */

jint
KaffeJNI_CallIntMethodV(JNIEnv* env UNUSED, jobject obj, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.i);
}

jint
KaffeJNI_CallIntMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
  va_list args;
  jint r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallIntMethodV(env, obj, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jint
KaffeJNI_CallIntMethodA(JNIEnv* env UNUSED, jobject obj, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.i);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning long
 * ------------------------------------------------------------------------------
 */

jlong
KaffeJNI_CallLongMethodV(JNIEnv* env UNUSED, jobject obj, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.j);
}

jlong
KaffeJNI_CallLongMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
  va_list args;
  jlong r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallLongMethodV(env, obj, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jlong
KaffeJNI_CallLongMethodA(JNIEnv* env UNUSED, jobject obj, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.j);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning float
 * ------------------------------------------------------------------------------
 */

jfloat
KaffeJNI_CallFloatMethodV(JNIEnv* env UNUSED, jobject obj, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.f);
}

jfloat
KaffeJNI_CallFloatMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
  va_list args;
  jfloat r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallFloatMethodV(env, obj, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jfloat
KaffeJNI_CallFloatMethodA(JNIEnv* env UNUSED, jobject obj, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.f);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning double
 * ------------------------------------------------------------------------------
 */

jdouble
KaffeJNI_CallDoubleMethodV(JNIEnv* env UNUSED, jobject obj, jmethodID meth, va_list args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, getMethodFunc (m, o), o, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.d);
}

jdouble
KaffeJNI_CallDoubleMethod(JNIEnv* env, jobject obj, jmethodID meth, ...)
{
  va_list args;
  jdouble r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallDoubleMethodV(env, obj, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jdouble
KaffeJNI_CallDoubleMethodA(JNIEnv* env UNUSED, jobject obj, jmethodID meth, jvalue* args)
{
  jvalue retval;
  jobject obj_local;
  Hjava_lang_Object* o;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);
  obj_local = unveil(obj);
  o = (Hjava_lang_Object*)obj_local;

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, getMethodFunc (m, o), o, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.d);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning long
 * ------------------------------------------------------------------------------
 */

jlong
KaffeJNI_CallNonvirtualLongMethodV(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }
  obj_local = unveil(obj);

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.j);
}

jlong
KaffeJNI_CallNonvirtualLongMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jlong r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualLongMethodV(env, obj, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jlong
KaffeJNI_CallNonvirtualLongMethodA(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }
  obj_local = unveil(obj);

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.j);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning float
 * ------------------------------------------------------------------------------
 */

jfloat
KaffeJNI_CallNonvirtualFloatMethodV(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }
  obj_local = unveil(obj);

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.f);
}

jfloat
KaffeJNI_CallNonvirtualFloatMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jfloat r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualFloatMethodV(env, obj, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jfloat
KaffeJNI_CallNonvirtualFloatMethodA(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }
  obj_local = unveil(obj);

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.f);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning double
 * ------------------------------------------------------------------------------
 */

jdouble
KaffeJNI_CallNonvirtualDoubleMethodV(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }
  obj_local = unveil(obj);

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.d);
}

jdouble
KaffeJNI_CallNonvirtualDoubleMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jdouble r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallNonvirtualDoubleMethodV(env, obj, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jdouble
KaffeJNI_CallNonvirtualDoubleMethodA(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING(0);

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }
  obj_local = unveil(obj);

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.d);
}

/* ------------------------------------------------------------------------------
 * Non-virtual method calls returning void
 * ------------------------------------------------------------------------------
 */

void
KaffeJNI_CallNonvirtualVoidMethodV(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, va_list args)
{
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING_VOID();

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }
  obj_local = unveil(obj);

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), obj_local, args, NULL);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_CallNonvirtualVoidMethod(JNIEnv* env, jobject obj, jclass cls, jmethodID meth, ...)
{
  va_list args;

  BEGIN_EXCEPTION_HANDLING_VOID();

  va_start(args, meth);
  KaffeJNI_CallNonvirtualVoidMethodV(env, obj, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_CallNonvirtualVoidMethodA(JNIEnv* env UNUSED, jobject obj, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  Method* m = (Method*)meth;
  jobject obj_local;

  BEGIN_EXCEPTION_HANDLING_VOID();

  if (METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }
  obj_local = unveil(obj);

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), obj_local, args, NULL, 0);

  END_EXCEPTION_HANDLING();
}

/* ------------------------------------------------------------------------------
 * Static method calls returning Object
 * ------------------------------------------------------------------------------
 */

jobject
KaffeJNI_CallStaticObjectMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(NULL);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, args, &retval);

  ADD_REF(retval.l);
  END_EXCEPTION_HANDLING();
  return (retval.l);
}

jobject
KaffeJNI_CallStaticObjectMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jobject r;

  BEGIN_EXCEPTION_HANDLING(NULL);

  va_start(args, meth);
  r = KaffeJNI_CallStaticObjectMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jobject
KaffeJNI_CallStaticObjectMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(NULL);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, args, &retval, 0);

  ADD_REF(retval.l);
  END_EXCEPTION_HANDLING();
  return (retval.l);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning boolean
 * ------------------------------------------------------------------------------
 */

jboolean
KaffeJNI_CallStaticBooleanMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jboolean) retval.i);
}

jboolean
KaffeJNI_CallStaticBooleanMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jboolean r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallStaticBooleanMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jboolean
KaffeJNI_CallStaticBooleanMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jboolean) retval.i);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning byte
 * ------------------------------------------------------------------------------
 */

jbyte
KaffeJNI_CallStaticByteMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jbyte) retval.i);
}

jbyte
KaffeJNI_CallStaticByteMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jbyte r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallStaticByteMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jbyte
KaffeJNI_CallStaticByteMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jbyte) retval.i);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning char
 * ------------------------------------------------------------------------------
 */

jchar
KaffeJNI_CallStaticCharMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jchar) retval.i);
}

jchar
KaffeJNI_CallStaticCharMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jchar r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallStaticCharMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jchar
KaffeJNI_CallStaticCharMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jchar) retval.i);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning short
 * ------------------------------------------------------------------------------
 */

jshort
KaffeJNI_CallStaticShortMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, args, &retval);

  END_EXCEPTION_HANDLING();
  return ((jshort) retval.i);
}

jshort
KaffeJNI_CallStaticShortMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jshort r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallStaticShortMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jshort
KaffeJNI_CallStaticShortMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return ((jshort) retval.i);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning int
 * ------------------------------------------------------------------------------
 */

jint
KaffeJNI_CallStaticIntMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.i);
}

jint
KaffeJNI_CallStaticIntMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jint r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallStaticIntMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jint
KaffeJNI_CallStaticIntMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.i);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning long
 * ------------------------------------------------------------------------------
 */

jlong
KaffeJNI_CallStaticLongMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.j);
}

jlong
KaffeJNI_CallStaticLongMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jlong r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallStaticLongMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jlong
KaffeJNI_CallStaticLongMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.j);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning float
 * ------------------------------------------------------------------------------
 */

jfloat
KaffeJNI_CallStaticFloatMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.f);
}

jfloat
KaffeJNI_CallStaticFloatMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jfloat r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallStaticFloatMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}

jfloat
KaffeJNI_CallStaticFloatMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.f);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning double
 * ------------------------------------------------------------------------------
 */

jdouble
KaffeJNI_CallStaticDoubleMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, args, &retval);

  END_EXCEPTION_HANDLING();
  return (retval.d);
}

jdouble
KaffeJNI_CallStaticDoubleMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;
  jdouble r;

  BEGIN_EXCEPTION_HANDLING(0);

  va_start(args, meth);
  r = KaffeJNI_CallStaticDoubleMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
  return (r);
}


jdouble
KaffeJNI_CallStaticDoubleMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING(0);

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, args, &retval, 0);

  END_EXCEPTION_HANDLING();
  return (retval.d);
}

/* ------------------------------------------------------------------------------
 * Static method calls returning void
 * ------------------------------------------------------------------------------
 */

void
KaffeJNI_CallStaticVoidMethodV(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, va_list args)
{
  jvalue retval;
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING_VOID();

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodV(m, METHOD_NATIVECODE(m), NULL, args, &retval);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_CallStaticVoidMethod(JNIEnv* env, jclass cls, jmethodID meth, ...)
{
  va_list args;

  BEGIN_EXCEPTION_HANDLING_VOID();

  va_start(args, meth);
  KaffeJNI_CallStaticVoidMethodV(env, cls, meth, args);
  va_end(args);

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_CallStaticVoidMethodA(JNIEnv* env UNUSED, jclass cls UNUSED, jmethodID meth, jvalue* args)
{
  Method* m = (Method*)meth;

  BEGIN_EXCEPTION_HANDLING_VOID();

  if (!METHOD_IS_STATIC(m)) {
    throwException(NoSuchMethodError(m->name->data));
  }

  KaffeVM_callMethodA(m, METHOD_NATIVECODE(m), NULL, args, NULL, 0);

  END_EXCEPTION_HANDLING();
}
