/* 
 * $Id: xmlj_error.c,v 1.1 2004/04/14 19:40:14 dalibor Exp $
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

#include "xmlj_error.h"

static void xmljErrorSAXFunc (void *ctx, const char *msg, ...);
static void xmljFatalErrorSAXFunc (void *ctx, const char *msg, ...);
static void xmljWarningSAXFunc (void *ctx, const char *msg, ...);
static jobject xmljMakeJaxpSourceLocator (void *ctx);

static jobject
xmljMakeJaxpSourceLocator (void *ctx)
{
  xmlParserCtxtPtr parseContext = (xmlParserCtxtPtr) ctx;
  SaxErrorContext *saxErrorContext =
    (SaxErrorContext *) parseContext->_private;
  JNIEnv *env = saxErrorContext->env;

  jobject result = (*env)->AllocObject (env,
					saxErrorContext->sourceLocatorClass);

  const char *systemId =
    (const char *) saxErrorContext->locator->getSystemId (ctx);
  const char *publicId =
    (const char *) saxErrorContext->locator->getPublicId (ctx);

  (*env)->CallVoidMethod (env,
			  result,
			  saxErrorContext->sourceLocatorConstructor,
			  (*env)->NewStringUTF (env,
						(NULL !=
						 systemId) ? systemId : ""),
			  (*env)->NewStringUTF (env,
						(NULL !=
						 publicId) ? publicId : ""),
			  saxErrorContext->locator->getLineNumber (ctx),
			  saxErrorContext->locator->getColumnNumber (ctx));

  return result;
}

static void
callErrorAdapterMethod (void *ctx, const char *msg, va_list va,
			jmethodID methodID)
{
  xmlParserCtxtPtr parseContext = (xmlParserCtxtPtr) ctx;
  SaxErrorContext *saxErrorContext =
    (SaxErrorContext *) parseContext->_private;
  JNIEnv *env = saxErrorContext->env;

  if (!(*env)->ExceptionOccurred (env))
    {
      jobject jaxpSourceLocator = xmljMakeJaxpSourceLocator (ctx);

      char buffer[2048] = "[Error message too long]";
      vsnprintf (buffer, sizeof buffer, msg, va);

      (*env)->CallVoidMethod (env,
			      saxErrorContext->saxErrorAdapter,
			      methodID,
			      (*env)->NewStringUTF (env, buffer),
			      jaxpSourceLocator);

      if ((*env)->ExceptionOccurred (env))
	{
	  xmlStopParser (parseContext);
	}
    }
}

static void
xmljErrorSAXFunc (void *ctx, const char *msg, ...)
{
  xmlParserCtxtPtr parseContext = (xmlParserCtxtPtr) ctx;
  SaxErrorContext *saxErrorContext =
    (SaxErrorContext *) parseContext->_private;

  va_list va;
  va_start (va, msg);

  callErrorAdapterMethod (ctx, msg, va, saxErrorContext->saxErrorMethodID);
  va_end (va);
}

static void
xmljFatalErrorSAXFunc (void *ctx, const char *msg, ...)
{
  xmlParserCtxtPtr parseContext = (xmlParserCtxtPtr) ctx;
  SaxErrorContext *saxErrorContext =
    (SaxErrorContext *) parseContext->_private;
  va_list va;
  va_start (va, msg);
  callErrorAdapterMethod (ctx, msg, va,
			  saxErrorContext->saxFatalErrorMethodID);
  va_end (va);
}

static void
xmljWarningSAXFunc (void *ctx, const char *msg, ...)
{
  xmlParserCtxtPtr parseContext = (xmlParserCtxtPtr) ctx;
  SaxErrorContext *saxErrorContext =
    (SaxErrorContext *) parseContext->_private;
  va_list va;
  va_start (va, msg);
  callErrorAdapterMethod (ctx, msg, va, saxErrorContext->saxWarningMethodID);
  va_end (va);
}

const xmlChar *
xmljGetSystemId (void *ctx)
{
  xmlParserCtxtPtr parseContext = (xmlParserCtxtPtr) ctx;
  SaxErrorContext *saxErrorContext =
    (SaxErrorContext *) parseContext->_private;
  return (xmlChar *) saxErrorContext->systemIdCstr;
}


const xmlChar *
xmljGetPublicId (void *ctx)
{
  xmlParserCtxtPtr parseContext = (xmlParserCtxtPtr) ctx;
  SaxErrorContext *saxErrorContext =
    (SaxErrorContext *) parseContext->_private;
  return (xmlChar *) saxErrorContext->publicIdCstr;
}

static void
xmljSetDocumentLocator (void *ctx, xmlSAXLocatorPtr loc)
{
  xmlParserCtxtPtr parseContext = (xmlParserCtxtPtr) ctx;
  SaxErrorContext *saxErrorContext =
    (SaxErrorContext *) parseContext->_private;
  saxErrorContext->locator = loc;

  loc->getPublicId = xmljGetPublicId;
  loc->getSystemId = xmljGetSystemId;
}


void
xmljInitErrorHandling (xmlSAXHandler * saxHandler)
{
  saxHandler->warning = &xmljWarningSAXFunc;
  saxHandler->error = &xmljErrorSAXFunc;
  saxHandler->fatalError = &xmljFatalErrorSAXFunc;
  saxHandler->setDocumentLocator = &xmljSetDocumentLocator;
}

SaxErrorContext *
xmljCreateSaxErrorContext (JNIEnv * env, jobject saxErrorAdapter,
			   jstring systemId, jstring publicId)
{
  SaxErrorContext *saxErrorContext
    = (SaxErrorContext *) malloc (sizeof (SaxErrorContext));

  jclass saxErrorAdapterClass 
    = (*env)->FindClass (env,
                         "gnu/xml/libxmlj/transform/JavaContext");
  jclass sourceWrapperClass 
    = (*env)->FindClass (env,
                         "gnu/xml/libxmlj/transform/SourceWrapper");
  jclass libxmlDocumentClassID 
    = (*env)->FindClass (env, 
                         "gnu/xml/libxmlj/transform/LibxmlDocument");
  
  saxErrorContext->saxWarningMethodID
    = (*env)->GetMethodID (env,
			   saxErrorAdapterClass,
			   "saxWarning",
			   "(Ljava/lang/String;Ljavax/xml/transform/SourceLocator;)V");
  saxErrorContext->saxErrorMethodID
    = (*env)->GetMethodID (env,
			   saxErrorAdapterClass,
			   "saxError",
			   "(Ljava/lang/String;Ljavax/xml/transform/SourceLocator;)V");
  saxErrorContext->saxFatalErrorMethodID
    = (*env)->GetMethodID (env,
			   saxErrorAdapterClass,
			   "saxFatalError",
			   "(Ljava/lang/String;Ljavax/xml/transform/SourceLocator;)V");
  saxErrorContext->resolveURIMethodID
    = (*env)->GetMethodID (env,
			   saxErrorAdapterClass,
			   "resolveURI",
			   "(Ljava/lang/String;Ljava/lang/String;)Lgnu/xml/libxmlj/transform/SourceWrapper;");
  saxErrorContext->resolveURIAndOpenMethodID
    = (*env)->GetMethodID (env,
			   saxErrorAdapterClass,
			   "resolveURIAndOpen",
			   "(Ljava/lang/String;Ljava/lang/String;)Lgnu/xml/libxmlj/transform/LibxmlDocument;");
  saxErrorContext->getInputStreamMethodID
    = (*env)->GetMethodID (env,
			   sourceWrapperClass,
			   "getInputStream",
			   "()Ljava/io/PushbackInputStream;");

  saxErrorContext->xsltGenericErrorMethodID
    = (*env)->GetMethodID (env,
			   saxErrorAdapterClass,
			   "xsltGenericError", "(Ljava/lang/String;)V");

  saxErrorContext->sourceLocatorClass
    = (*env)->FindClass (env, "gnu/xml/libxmlj/transform/SourceLocatorImpl");
  saxErrorContext->sourceLocatorConstructor
    = (*env)->GetMethodID (env,
			   saxErrorContext->sourceLocatorClass,
			   "<init>",
			   "(Ljava/lang/String;Ljava/lang/String;II)V");

  saxErrorContext->getNativeHandleMethodID
    = (*env)->GetMethodID (env,
			   libxmlDocumentClassID,
			   "getNativeHandle",
			   "()J");

  saxErrorContext->env = env;
  saxErrorContext->saxErrorAdapter = saxErrorAdapter;

  if (NULL != systemId)
    {
      saxErrorContext->systemIdCstr =
	(*env)->GetStringUTFChars (env, systemId, NULL);
      saxErrorContext->systemId = systemId;
    }
  else
    {
      saxErrorContext->systemIdCstr = 0;
      saxErrorContext->systemId = 0;
    }

  if (NULL != publicId)
    {
      saxErrorContext->publicIdCstr =
	(*env)->GetStringUTFChars (env, publicId, NULL);
      saxErrorContext->publicId = publicId;
    }
  else
    {
      saxErrorContext->publicIdCstr = 0;
      saxErrorContext->publicId = 0;
    }

  return saxErrorContext;
}


void
xmljFreeSaxErrorContext (SaxErrorContext * errorContext)
{
  JNIEnv *env = errorContext->env;
  if (NULL != errorContext->systemId && NULL != errorContext->systemIdCstr)
    {
      (*env)->ReleaseStringUTFChars (env, errorContext->systemId,
				     errorContext->systemIdCstr);
    }
  if (NULL != errorContext->publicId && NULL != errorContext->publicIdCstr)
    {
      (*env)->ReleaseStringUTFChars (env, errorContext->publicId,
				     errorContext->publicIdCstr);
    }
  free (errorContext);
}


void
xmljXsltErrorFunc (void *ctx, const char *msg, ...)
{
  if (NULL != ctx)
    {
      SaxErrorContext *errorContext = ((SaxErrorContext *) ctx);
      JNIEnv *env = errorContext->env;

      if (NULL != errorContext && !(*env)->ExceptionOccurred (env))
	{
	  char buffer[2048] = "[Error message too long]";
	  va_list va;
	  va_start (va, msg);
	  vsnprintf (buffer, sizeof buffer, msg, va);
	  va_end (va);

	  (*env)->CallVoidMethod (env,
				  errorContext->saxErrorAdapter,
				  errorContext->xsltGenericErrorMethodID,
				  (*env)->NewStringUTF (env, buffer));
	}
    }
  else
    {
      va_list va;
      va_start (va, msg);
      printf ("libxslt error: ");
      vprintf (msg, va);
      va_end (va);
    }
}
