/* 
 * $Id: xmlj_error.h,v 1.1 2004/04/14 19:40:14 dalibor Exp $
 * Copyright (C) 2003 Julian Scheid
 * 
 * This file is part of GNU LibxmlJ, a JAXP-compliant Java wrapper for
 * the XML and XSLT C libraries for Gnome (libxml2/libxslt).
 * 
 * GNU LibxmlJ is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *  
 * GNU LibxmlJ is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU LibxmlJ; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA. 
 */

#ifndef XMLJ_ERROR_H
#define XMLJ_ERROR_H

#include <jni.h>  
#include <libxml/xmlIO.h>

typedef struct SaxErrorContext_ 
{
  JNIEnv * env;
  jobject saxErrorAdapter;
  jmethodID saxCommentMethodID;
  jmethodID saxWarningMethodID;
  jmethodID saxErrorMethodID;
  jmethodID saxFatalErrorMethodID;
  jclass sourceLocatorClass;
  jmethodID sourceLocatorConstructor;
  xmlSAXLocatorPtr locator;
  jstring publicId;
  jstring systemId;
  const char *publicIdCstr;
  const char *systemIdCstr;
  jmethodID resolveURIMethodID;
  jmethodID resolveURIAndOpenMethodID;
  jmethodID getInputStreamMethodID;
  jmethodID xsltGenericErrorMethodID;
  jobject theTransformerException;
  jmethodID getNativeHandleMethodID;
} SaxErrorContext;

SaxErrorContext * xmljCreateSaxErrorContext (JNIEnv * env,
					       jobject errorContext,
					       jstring systemId,
					       jstring publicId);

void xmljFreeSaxErrorContext (SaxErrorContext * errorContext);

void xmljInitErrorHandling (xmlSAXHandler * saxHandler);

void xmljXsltErrorFunc (void *ctx, const char *msg, ...);

#endif	/* !defined XMLJ_ERROR_H */

