/*
 * xmlj_util.c
 * Copyright (C) 2004 The Free Software Foundation
 * 
 * This file is part of GNU JAXP, a library.
 * 
 * GNU JAXP is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * GNU JAXP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License.
 */
#include "xmlj_util.h"
#include "xmlj_error.h"
#include <libxml/tree.h>
#include <unistd.h>

/* xmlChar->jstring cache */
#ifdef XMLJ_STRING_CACHE
#define XMLJ_STRING_CACHE_SIZE 1024
xmlHashTablePtr xmljStringCache = NULL;

void
xmljHashDeallocate (void *data, xmlChar *name);

void
xmljHashDeallocate (void *data, xmlChar *name)
{
  /* NOOP */
}
#endif /* XMLJ_STRING_CACHE */

jstring
xmljNewString (JNIEnv * env, const xmlChar * text)
{
  jstring ret;

  if (text == NULL || (*env)->ExceptionOccurred (env))
    {
      return NULL;
    }
#ifdef XMLJ_STRING_CACHE
  if (xmljStringCache == NULL) /* Init cache */
    {
      xmljStringCache = xmlHashCreate (XMLJ_STRING_CACHE_SIZE);
    }
  ret = (jstring) xmlHashLookup (xmljStringCache, text);
  if (ret == NULL)
    {
      ret = (*env)->NewStringUTF (env, (char *) text);
      if (ret == NULL) /* Why? */
        {
          fprintf(stderr, "xmljNewString: ERROR: NewStringUTF returned null for \"%s\"\n", text);
          fflush (stderr);
        }
      else
        {
          xmlHashAddEntry (xmljStringCache, text, ret);
        }
    }
#else
  ret = (*env)->NewStringUTF (env, (char *) text);
  if (ret == NULL) /* Why? */
    {
      printf("xmljNewString: ERROR: NewStringUTF returned null for \"%s\"\n", text);
    }
#endif /* XMLJ_STRING_CACHE */
  return ret;
}

void
xmljClearStringCache ()
{
#ifdef XMLJ_STRING_CACHE
  if (xmljStringCache != NULL)
    {
      xmlHashFree (xmljStringCache, &xmljHashDeallocate);
    }
#endif /* XMLJ_STRING_CACHE */
}

const xmlChar *
xmljGetStringChars (JNIEnv * env, jstring text)
{
  const char *s_text;
  xmlChar *x_text;

  if (text == NULL)
    {
      return NULL;
    }

  s_text = (*env)->GetStringUTFChars (env, text, 0);
  x_text = (s_text == NULL) ? NULL : xmlCharStrdup (s_text);
  if (s_text != NULL && x_text == NULL)
    {
      /* TODO raise exception */
    }
  (*env)->ReleaseStringUTFChars (env, text, s_text);
  return x_text;
}

const xmlChar *
xmljGetPrefix (const xmlChar * qName)
{
  const xmlChar *localName;
  const xmlChar *ret;
  xmlChar **prefix;

  prefix = (xmlChar **) malloc (sizeof (xmlChar *));
  localName = xmlSplitQName2 (qName, prefix);
  if (localName == NULL)
    {
      return NULL;
    }
  ret = *prefix;
  free (prefix);
  return ret;
}

const xmlChar *
xmljGetLocalName (const xmlChar * qName)
{
  const xmlChar *localName;
  xmlChar **prefix;

  prefix = (xmlChar **) malloc (sizeof (xmlChar *));
  localName = xmlSplitQName2 (qName, prefix);
  if (localName == NULL)
    {
      return qName;
    }
  free (prefix);
  return localName;
}

jmethodID xmljGetMethodID (JNIEnv *env,
                           jobject target,
                           const char *name,
                           const char *signature)
{
  jclass cls;
  jmethodID ret;

  cls = (*env)->GetObjectClass (env, target);
  if (cls == NULL)
    {
      xmljThrowException (env,
                          "java/lang/ClassNotFoundException",
                          NULL);
      return NULL;
    }
  ret = (*env)->GetMethodID (env,
                             cls,
                             name,
                             signature);
  if (ret == NULL)
    {
      jclass clscls;
      jmethodID nm;
      jstring clsname;
      const char *c_clsName;
      char cat[512] = "[method signature too long]";
      
      clscls = (*env)->FindClass (env, "java/lang/Class");
      if (clscls == NULL)
        {
          return NULL;
        }
      nm = (*env)->GetMethodID (env, clscls, "getName",
                                "()Ljava/lang/String;");
      if (nm == NULL)
        {
          return NULL;
        }
      clsname = (jstring) (*env)->CallObjectMethod (env,
                                                    (jobject)cls,
                                                    nm);
      if (clsname == NULL)
        {
          return NULL;
        }
      c_clsName = (*env)->GetStringUTFChars (env, clsname, 0);
      sprintf (cat, "%s.%s %s", c_clsName, name, signature);
      xmljThrowException (env,
                          "java/lang/NoSuchMethodException",
                          cat);
      (*env)->ReleaseStringUTFChars (env, clsname, c_clsName);
    }
  return ret;
}

void *
xmljAsPointer (JNIEnv *env, jobject ptr)
{
  jclass cls;
  jfieldID field;
 
#if defined XMLJ_64BIT_POINTER
  cls = (*env)->FindClass (env, "gnu/xml/libxmlj/RawData64");
  field = (*env)->GetFieldID (env, cls, "data", "J");
  return (void *) (*env)->GetLongField (env, ptr, field);
#else
  cls = (*env)->FindClass (env, "gnu/xml/libxmlj/RawData32");
  field = (*env)->GetFieldID (env, cls, "data", "I");
  return (void *) (*env)->GetIntField (env, ptr, field);
#endif
}

jobject
xmljAsField (JNIEnv *env, void * ptr)
{
  jclass cls;
  jmethodID method;

#if defined XMLJ_64BIT_POINTER
  cls = (*env)->FindClass (env, "gnu/xml/libxmlj/RawData64");
  method = (*env)->GetMethodID (env, cls, "<init>", "(J)V");
  return (*env)->NewObject (env, cls, method, (jlong) ptr);
#else
  cls = (*env)->FindClass (env, "gnu/xml/libxmlj/RawData32");
  method = (*env)->GetMethodID (env, cls, "<init>", "(I)V");
  return (*env)->NewObject (env, cls, method, (jint) ptr);
#endif
}

JNIEnv *
xmljGetJNIEnv ()
{
  JavaVM **jvms;
  jsize *jvm_count;
  JavaVM *jvm;
  JNIEnv **envs;
  JNIEnv *env;

  jvms = (JavaVM **) malloc (sizeof (JavaVM *));
  if (!jvms)
    {
      return NULL;
    }
  jvm_count = (jsize *) malloc (sizeof (jsize));
  if (!jvm_count)
    {
      free (jvms);
      return NULL;
    }
  if (JNI_GetCreatedJavaVMs (jvms, 1, jvm_count))
    {
      free (jvms);
      free (jvm_count);
      return NULL;
    }
  jvm = *jvms;
  envs = (JNIEnv **) malloc (sizeof (JNIEnv *));
  if (!envs)
    {
      free (jvms);
      free (jvm_count);
      return NULL;
    }
  (*jvm)->AttachCurrentThread (jvm, (void **) envs, NULL);
  (*jvm)->GetEnv (jvm, (void **) envs, JNI_VERSION_1_2);
  if (envs)
    {
      env = *envs;
      free (envs);
    }
  else
    {
      env = NULL;
    }
  free (jvms);
  free (jvm_count);
  return env;
}

