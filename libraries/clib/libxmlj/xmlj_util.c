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

jstring
xmljNewString (JNIEnv * env, const xmlChar * text)
{
  char *s_text;
  jstring ret;

  if (text == NULL)
    return NULL;
  /*s_text = (char *)malloc(sizeof(char *));
    sprintf(s_text, "%s", text); */
  s_text = (char *) text;	/* TODO signedness? */
  ret = (*env)->NewStringUTF (env, s_text);
  /*free(s_text); */
  return ret;
}

const xmlChar *
xmljGetStringChars (JNIEnv * env, jstring text)
{
  const char *s_text;
  xmlChar *x_text;

  if (text == NULL)
    return NULL;

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
    return NULL;
  else
    {
      ret = *prefix;
      free (prefix);
      return ret;
    }
}

const xmlChar *
xmljGetLocalName (const xmlChar * qName)
{
  const xmlChar *localName;
  xmlChar **prefix;

  prefix = (xmlChar **) malloc (sizeof (xmlChar *));
  localName = xmlSplitQName2 (qName, prefix);
  if (localName == NULL)
    return qName;
  else
    {
      free (prefix);
      return localName;
    }
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
      jclass clscls = (*env)->FindClass(env, "java/lang/Class");
      jmethodID nm = (*env)->GetMethodID(env, clscls, "getName", "()Ljava/lang/String;");
      jstring clsname = (jstring)(*env)->CallObjectMethod(env,
                                                          (jobject)cls,
                                                          nm);
      const char * c_clsName = (*env)->GetStringUTFChars(env, clsname, 0);
      char * cat = (char *)malloc(sizeof(char));
      sprintf(cat, "%s.%s %s", c_clsName, name, signature);
      xmljThrowException (env,
                          "java/lang/NoSuchMethodException",
                          cat);
      free (cat);
      (*env)->ReleaseStringUTFChars(env, clsname, c_clsName);
    }
  return ret;
}
