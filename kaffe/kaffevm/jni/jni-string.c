/*
 * jni-string.c
 * Java Native Interface - String Handling JNI functions.
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
#include "jni.h"
#include "jni_i.h"
#include "kaffe/jmalloc.h"
#include "itypes.h"
#include "object.h"
#include "errors.h"
#include "exception.h"
#include "stringSupport.h"
#include "gc.h"
#include "jni_funcs.h"
#include "baseClasses.h"

void
KaffeJNI_ReleaseStringChars(JNIEnv* env UNUSED, jstring data UNUSED, const jchar* chars UNUSED)
{
  /* Does nothing */
}

jstring
KaffeJNI_NewString(JNIEnv* env UNUSED, const jchar* data, jsize len)
{
  Hjava_lang_String* str;

  BEGIN_EXCEPTION_HANDLING(NULL);

  str = (Hjava_lang_String*)newObject(StringClass);
  unhand(str)->offset = 0;
  unhand(str)->count = len;
  unhand(str)->value = (HArrayOfChar*)newArray(TYPE_CLASS(TYPE_Char), len);                   
  unhand(str)->interned = 0;
  memcpy(STRING_DATA(str), data, len * sizeof(jchar));

  END_EXCEPTION_HANDLING();
  return (str);
}

jsize
KaffeJNI_GetStringLength(JNIEnv* env UNUSED, jstring data)
{
  jsize len;
  BEGIN_EXCEPTION_HANDLING(0);

  data = unveil(data);
  len = STRING_SIZE((Hjava_lang_String*)data);
  END_EXCEPTION_HANDLING();
  return (len);
}

const jchar*
KaffeJNI_GetStringChars(JNIEnv* env UNUSED, jstring data, jboolean* copy)
{
  jchar* c;
  BEGIN_EXCEPTION_HANDLING(NULL);

  data = unveil(data);
  if (copy != NULL) {
    *copy = JNI_FALSE;
  }
  c = STRING_DATA(((Hjava_lang_String*)data));

  END_EXCEPTION_HANDLING();
  return (c);
}

jstring
KaffeJNI_NewStringUTF(JNIEnv* env UNUSED, const char* data)
{
  Hjava_lang_String* str;
  Utf8Const* utf8;
  size_t len;

  /* We cannot construct the string if data is NULL. So as JNI says
   * we return NULL.
   */

  BEGIN_EXCEPTION_HANDLING(NULL);

  if (data != NULL) {
    len = strlen(data);
    if (!utf8ConstIsValidUtf8(data, len)) {
      str = NULL;
    } else {
      utf8 = checkPtr(utf8ConstNew(data, (int)len));
      str = utf8Const2Java(utf8);
      utf8ConstRelease(utf8);
      if (!str) {
        errorInfo info;
        postOutOfMemory(&info);
        throwError(&info);
      }
    }
  } else
   str = NULL;

  END_EXCEPTION_HANDLING();
  return (str);
}

jsize
KaffeJNI_GetStringUTFLength(JNIEnv* env UNUSED, jstring data)
{
  Hjava_lang_String* const str = (Hjava_lang_String*)unveil(data);
  jchar* ptr;
  jsize len;
  jsize count;
  jsize i;

  BEGIN_EXCEPTION_HANDLING(0);

  ptr = STRING_DATA(str);
  len = STRING_SIZE(str);

  count = 0;
  for (i = 0; i < len; i++) {
    if (ptr[i] >= 0x0001 && ptr[i] <= 0x007F) {
      count += 1;
    }
    else if (ptr[i] >= 0x0080 && ptr[i] <= 0x07FF) {
      count += 2;
    }
    else {
      count += 3;
    }
  }

  END_EXCEPTION_HANDLING();
  return (count);
}

const jbyte*
KaffeJNI_GetStringUTFChars(JNIEnv* env, jstring data, jboolean* copy)
{
  Hjava_lang_String* const str = (Hjava_lang_String*)unveil(data);
  jchar* ptr;
  jbyte* buf;
  jsize len;
  jsize i;
  jsize j;

  BEGIN_EXCEPTION_HANDLING(NULL);

  /* We always copy data */
  if (copy != NULL) {
    *copy = JNI_TRUE;
  }

  buf = checkPtr(KMALLOC((size_t)KaffeJNI_GetStringUTFLength(env, data) + 1));

  ptr = STRING_DATA(str);
  len = STRING_SIZE(str);

  for (j = 0, i = 0; i < len; i++) {
    if (ptr[i] >= 0x0001 && ptr[i] <= 0x007F) {
      buf[j++] = ptr[i] & 0x7F;
    }
    else if (ptr[i] >= 0x0080 && ptr[i] <= 0x07FF) {
      buf[j++] = 0xC0 | ((ptr[i] >> 6) & 0x1F);
      buf[j++] = 0x80 | (ptr[i] & 0x3F);
    }
    else {
      buf[j++] = 0xE0 | ((ptr[i] >> 12) & 0x0F);
      buf[j++] = 0x80 | ((ptr[i] >> 6) & 0x3F);
      buf[j++] = 0x80 | (ptr[i] & 0x3F);
    }
  }

  END_EXCEPTION_HANDLING();
  return (buf);
}

void
KaffeJNI_ReleaseStringUTFChars(JNIEnv* env UNUSED, jstring data UNUSED, const char* chars)
{
  BEGIN_EXCEPTION_HANDLING_VOID();

  KFREE((void *) chars);
	
  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_GetStringRegion(JNIEnv UNUSED *env, jstring data, jsize start, jsize len, jchar *buf)
{
  Hjava_lang_String* const str = (Hjava_lang_String*)unveil(data);
  jchar *str_ptr;
  jsize str_len;
	
  BEGIN_EXCEPTION_HANDLING_VOID();

  str_ptr = STRING_DATA(str);
  str_len = STRING_SIZE(str);

  if (start >= len || start+len >= str_len) {
    errorInfo einfo;
	      
    postException(&einfo, "java.lang.StringIndexOutOfBoundsException");
    throwError(&einfo);
  }
	
  memcpy(buf, &str_ptr[start], len*sizeof(jchar));

  END_EXCEPTION_HANDLING();
}

void
KaffeJNI_GetStringUTFRegion(JNIEnv UNUSED *env, jstring data, jsize start, jsize len, char *buf)
{
  Hjava_lang_String* const str = (Hjava_lang_String*)unveil(data);
  jchar *str_ptr;
  jsize str_len;

  BEGIN_EXCEPTION_HANDLING_VOID();
	
  str_ptr = STRING_DATA(str);
  str_len = STRING_SIZE(str); 
  if (start >= len || start+len >= str_len) {
    errorInfo einfo;
	      
    postException(&einfo, "java.lang.StringIndexOutOfBoundsException");
    throwError(&einfo);
  }

  utf8ConstEncodeTo(&str_ptr[start], len, buf);

  END_EXCEPTION_HANDLING();
}
