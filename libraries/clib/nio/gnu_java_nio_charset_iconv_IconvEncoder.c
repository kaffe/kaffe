/* gnu_java_nio_charset_iconv_IconvEncoder.c --
   Copyright (C) 2005 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.

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

#include <stdio.h>
#include <assert.h>
#include <errno.h>
#include <iconv.h>
#include "gnu_java_nio_charset_iconv_IconvEncoder.h"

static void createRawData (JNIEnv * env, jobject obj, void *ptr);
static void *getData (JNIEnv * env, jobject obj);

JNIEXPORT void JNICALL
Java_gnu_java_nio_charset_iconv_IconvEncoder_openIconv (JNIEnv * env,
							jobject obj,
							jstring jname)
{
  jclass exception;
  iconv_t iconv_object;

  const char *name = (*env)->GetStringUTFChars (env, jname, 0);

  /* to "name" from java, native java format depends on endianness */
#ifdef WORDS_BIGENDIAN
  iconv_object = iconv_open (name, "UTF-16BE");
#else
  iconv_object = iconv_open (name, "UTF-16LE");
#endif

  (*env)->ReleaseStringUTFChars (env, jname, name);
  if ((long) iconv_object == -1L)
    {
      /* Throw an exception if charset not available */
      (*env)->ExceptionDescribe (env);
      (*env)->ExceptionClear (env);
      exception =
	(*env)->FindClass (env, "java/lang/IllegalArgumentException");
      assert (exception != 0);
      (*env)->ThrowNew (env, exception, "Charset not available.");
      return;
    }
  createRawData (env, obj, (void *) iconv_object);
}

JNIEXPORT jint JNICALL
Java_gnu_java_nio_charset_iconv_IconvEncoder_encode (JNIEnv * env,
						     jobject obj,
						     jcharArray inArr,
						     jbyteArray outArr,
						     jint posIn, jint remIn,
						     jint posOut, jint remOut)
{
  iconv_t iconv_object = getData (env, obj);
  jclass cls;
  jfieldID fid;
  size_t retval;
  char **in, **out;
  jchar *input, *inputcopy;
  jbyte *output, *outputcopy;
  size_t lenIn = (size_t) remIn * 2;
  size_t lenOut = (size_t) remOut;

  inputcopy = input = (*env)->GetCharArrayElements (env, inArr, 0);
  outputcopy = output = (*env)->GetByteArrayElements (env, outArr, 0);

  input += posIn;
  output += posOut;

  in = (char **) &input;
  out = (char **) &output;
  retval = iconv (iconv_object, in, &lenIn, out, &lenOut);

  /* XXX: Do we need to relase the input array? It's not modified. */
  (*env)->ReleaseCharArrayElements (env, inArr, inputcopy, 0);
  (*env)->ReleaseByteArrayElements (env, outArr, outputcopy, 0);

  if (retval == (size_t) (-1))
    {
      if (errno == EILSEQ || errno == EINVAL)
	retval = 1;
      else
	retval = 0;
    }
  else
    retval = 0;

  cls = (*env)->GetObjectClass (env, obj);
  fid = (*env)->GetFieldID (env, cls, "inremaining", "I");
  assert (fid != 0);
  (*env)->SetIntField (env, obj, fid, (jint) (lenIn >> 1));
  fid = (*env)->GetFieldID (env, cls, "outremaining", "I");
  assert (fid != 0);
  (*env)->SetIntField (env, obj, fid, (jint) lenOut);

  return (jint) retval;
}

JNIEXPORT void JNICALL
Java_gnu_java_nio_charset_iconv_IconvEncoder_closeIconv (JNIEnv * env,
							 jobject obj)
{
  iconv_t iconv_object;
  iconv_object = getData (env, obj);
  iconv_close (iconv_object);
}


static void
createRawData (JNIEnv * env, jobject obj, void *ptr)
{
  jclass cls;
  jmethodID method;
  jobject data;
  jfieldID data_fid;

  cls = (*env)->GetObjectClass (env, obj);
  data_fid = (*env)->GetFieldID (env, cls, "data", "Lgnu/classpath/RawData;");
  assert (data_fid != 0);

#ifdef POINTERS_ARE_64BIT
  cls = (*env)->FindClass (env, "gnu/classpath/RawData64");
  method = (*env)->GetMethodID (env, cls, "<init>", "(J)V");
  data = (*env)->NewObject (env, cls, method, (jlong) ptr);
#else
  cls = (*env)->FindClass (env, "gnu/classpath/RawData32");
  method = (*env)->GetMethodID (env, cls, "<init>", "(I)V");
  data = (*env)->NewObject (env, cls, method, (jint) ptr);
#endif

  (*env)->SetObjectField (env, obj, data_fid, data);
}

static void *
getData (JNIEnv * env, jobject obj)
{
  jclass cls;
  jfieldID field;
  jfieldID data_fid;
  jobject data;

  cls = (*env)->GetObjectClass (env, obj);
  data_fid = (*env)->GetFieldID (env, cls, "data", "Lgnu/classpath/RawData;");
  assert (data_fid != 0);
  data = (*env)->GetObjectField (env, obj, data_fid);

#if SIZEOF_VOID_P == 8
  cls = (*env)->FindClass (env, "gnu/classpath/RawData64");
  field = (*env)->GetFieldID (env, cls, "data", "J");
  return (void *) (*env)->GetLongField (env, data, field);
#else
  cls = (*env)->FindClass (env, "gnu/classpath/RawData32");
  field = (*env)->GetFieldID (env, cls, "data", "I");
  return (void *) (*env)->GetIntField (env, data, field);
#endif
}
