/*
 * xmlj_util.h
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

#ifndef XMLJ_UTIL_H
#define XMLJ_UTIL_H

#if defined __64BIT__ || defined __LP64 || defined _LP64 || defined __LP64__ || defined _ADDR64
#  define XMLJ_64BIT_POINTER 1
#endif 

#include <jni.h>
#include <libxml/xmlstring.h>

jstring xmljNewString (JNIEnv *, const xmlChar *);

const xmlChar *xmljGetStringChars (JNIEnv *, jstring);

const xmlChar *xmljGetPrefix (const xmlChar * qName);

const xmlChar *xmljGetLocalName (const xmlChar * qName);

jmethodID xmljGetMethodID (JNIEnv *env,
                           jobject target,
                           const char *name,
                           const char *signature);

void * xmljAsPointer (JNIEnv *env, jobject field);

jobject xmljAsField (JNIEnv *env, void * ptr);

#endif /* !defined XMLJ_UTIL_H */
