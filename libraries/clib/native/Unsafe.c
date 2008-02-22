/* Unsafe.c - Unsafe operations needed for concurrency
   Copyright (C) 2008 Dalibor Topic  <robilad@kaffe.org>

This file is part of Kaffe.

Kaffe is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

Kaffe is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with Kaffe; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */


#include <glib.h>
#include <jni.h>

#include "object.h"
#include "support.h"

/**
 * Get the offset of a field.
 *
 * @param env JNI interface pointer
 * @param unsafe unused
 * @param field an java.lang.reflect.Field instance 
 *
 * @return offset of the field.
 */
JNIEXPORT jlong JNICALL Java_sun_misc_Unsafe_objectFieldOffset(JNIEnv* env, jobject unsafe UNUSED, jobject field)
{
  const jlong FAILED = -1;

  /* First get the Field class. */
  jclass fieldClass = (*env)->FindClass (env, "java/reflect/Field");
  if (NULL != fieldClass) {
    /* Then get the slot field ID */
    jfieldID slotID = (*env)->GetFieldID(env, fieldClass, "slot", "I");
    if (NULL != slotID)
      /* Return the slot for the given field. */
      return (*env)->GetIntField(env, field, slotID);
      
  }
  /* If for some reason I we can get the Field class or the ID of the slot field,
     return and let the exception handling deal with the reason.
  */
  return FAILED;
}

/**
 * Find the address of a field.
 *
 * @param env pointer to the JNI environment.
 * @param obj object containing the field.
 * @param offset slot of the field
 *
 * @return field's address.
 */
static volatile void * getFieldAddress(JNIEnv *env, jobject obj, jlong offset)
{
  jclass clazz = (*env)->GetObjectClass(env, obj);
  return KaffeVM_GetFieldAddress(clazz, obj, offset);
}


/**
 * Helper macro, defining a sun.misc.Unsafe compare and swap function 
 * with a given NAME tail and TYPE of arguments.
 */
#define KAFFE_UNSAFE_COMPARE_AND_SWAP(NAME, TYPE)					\
JNIEXPORT jboolean JNICALL Java_sun_misc_Unsafe_compareAndSwap ## NAME(JNIEnv* env, jobject unsafe UNUSED, jobject obj, jlong offset, TYPE expect, TYPE update) \
{ \
  volatile TYPE * address = getFieldAddress(env, obj, offset); \
  if (sizeof(TYPE) == sizeof(gint)) \
    return g_atomic_int_compare_and_exchange((volatile gint *) address, (gint) expect, (gint) update); \
  else if (sizeof(TYPE) == sizeof(gpointer)) \
    return g_atomic_pointer_compare_and_exchange((volatile gpointer *) address, (gpointer) expect, (gpointer) update); \
  else \
    if (*address == expect) { \
      *address = update; \
      return JNI_TRUE; \
    } \
    else \
      return JNI_FALSE; \
} \

KAFFE_UNSAFE_COMPARE_AND_SWAP(Int, jint)
KAFFE_UNSAFE_COMPARE_AND_SWAP(Long, jlong)
KAFFE_UNSAFE_COMPARE_AND_SWAP(Object, jobject)

#define KAFFE_UNSAFE_PUT(ACCESS, NAME, TYPE)				\
  ACCESS void JNICALL Java_sun_misc_Unsafe_put ## NAME (JNIEnv* env, jobject unsafe UNUSED, jobject obj, jlong offset, TYPE value) \
  { \
    volatile TYPE * address = getFieldAddress(env, obj, offset); \
    *address = value; \
  }

KAFFE_UNSAFE_PUT(static, Int, jint)
KAFFE_UNSAFE_PUT(JNIEXPORT, Long, jlong)
KAFFE_UNSAFE_PUT(JNIEXPORT, Object, jobject)

#define KAFFE_UNSAFE_PUT_BODY(NAME) \
  { \
    /* Delegate this to the default implementation. */ \
    return Java_sun_misc_Unsafe_put ## NAME (env, unsafe, obj, offset, value); \
  } 

#define KAFFE_UNSAFE_PUT_ORDERED(NAME, TYPE) \
  JNIEXPORT void JNICALL Java_sun_misc_Unsafe_putOrdered ## NAME(JNIEnv* env, jobject unsafe, jobject obj, jlong offset, TYPE value) \
       KAFFE_UNSAFE_PUT_BODY(NAME)

#define KAFFE_UNSAFE_PUT_VOLATILE(NAME, TYPE) \
  JNIEXPORT void JNICALL Java_sun_misc_Unsafe_put ## NAME ## Volatile (JNIEnv* env, jobject unsafe, jobject obj, jlong offset, TYPE value) \
       KAFFE_UNSAFE_PUT_BODY(NAME)

#define KAFFE_UNSAFE_PUT_FUNCTIONS(TYPE) \
  KAFFE_UNSAFE_PUT_ ## TYPE (Int, jint) \
  KAFFE_UNSAFE_PUT_ ## TYPE (Long, jlong)	\
  KAFFE_UNSAFE_PUT_ ## TYPE (Object, jobject)

KAFFE_UNSAFE_PUT_FUNCTIONS(ORDERED)
KAFFE_UNSAFE_PUT_FUNCTIONS(VOLATILE)

#define KAFFE_UNSAFE_GET(ACCESS, NAME, TYPE) \
  ACCESS TYPE JNICALL Java_sun_misc_Unsafe_get ## NAME(JNIEnv* env, jobject unsafe UNUSED, jobject obj, jlong offset) \
  { \
    volatile TYPE * address = getFieldAddress(env, obj, offset); \
    return *address; \
  }

KAFFE_UNSAFE_GET(static, Int, jint)
KAFFE_UNSAFE_GET(JNIEXPORT, Long, jlong)
KAFFE_UNSAFE_GET(static, Object, jobject)

#define KAFFE_UNSAFE_GET_VOLATILE(NAME, TYPE) \
  JNIEXPORT TYPE JNICALL Java_sun_misc_Unsafe_get ## NAME ## Volatile(JNIEnv* env, jobject unsafe, jobject obj, jlong offset) \
  { \
    return Java_sun_misc_Unsafe_get ## NAME (env, unsafe, obj, offset); \
  }

KAFFE_UNSAFE_GET_VOLATILE(Int, jint)
KAFFE_UNSAFE_GET_VOLATILE(Long, jlong)
KAFFE_UNSAFE_GET_VOLATILE(Object, jobject)

JNIEXPORT jint JNICALL Java_sun_misc_Unsafe_arrayBaseOffset(JNIEnv* env UNUSED, jobject unsafe UNUSED, jclass arrayClass UNUSED)
{
  return ARRAY_DATA_OFFSET;
}

JNIEXPORT jint JNICALL Java_sun_misc_Unsafe_arrayIndexScale(JNIEnv* env UNUSED, jobject unsafe UNUSED, jclass arrayClass UNUSED)
{
  return sizeof(double);
}
  
JNIEXPORT void JNICALL Java_sun_misc_Unsafe_unpark(JNIEnv* env UNUSED, jobject unsafe UNUSED, jobject park UNUSED)
{
  /** FIXME */
}

JNIEXPORT void JNICALL Java_sun_misc_Unsafe_park(JNIEnv* env UNUSED, jobject unsafe UNUSED, jboolean flag UNUSED, jlong duration UNUSED)
{
  /** FIXME */
}

