/* 
 * Copyright (C) 2003, 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Classpathx/jaxp.
 * 
 * GNU Classpath is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *  
 * GNU Classpath is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Classpath; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
 * 
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 * 
 * As a special exception, the copyright holders of this library give you
 * permission to link this library with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this library, you may extend
 * this exception to your version of the library, but you are not
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 */

#include "xmlj_error.h"
#include "xmlj_io.h"
#include "xmlj_util.h"

/*
static void xmljErrorSAXFunc (void *ctx, const char *msg, ...);
static void xmljFatalErrorSAXFunc (void *ctx, const char *msg, ...);
static void xmljWarningSAXFunc (void *ctx, const char *msg, ...);
static jobject xmljMakeJaxpSourceLocator (void *ctx);

static jobject
xmljMakeJaxpSourceLocator (void *ctx)
{
  xmlParserCtxtPtr parseContext;
  SaxErrorContext *saxErrorContext;
  JNIEnv *env;
  jobject result;
  const char *systemId;
  const char *publicId;

  parseContext = (xmlParserCtxtPtr) ctx;
  saxErrorContext =
    (SaxErrorContext *) parseContext->_private;
  if (saxErrorContext == NULL)
    return NULL;
  
  env = saxErrorContext->env;
  result = (*env)->AllocObject (env,
					saxErrorContext->sourceLocatorClass);
  systemId = (const char *) saxErrorContext->locator->getSystemId (ctx);
  publicId = (const char *) saxErrorContext->locator->getPublicId (ctx);

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
  xmlParserCtxtPtr parseContext;
  SaxErrorContext *saxErrorContext;
  JNIEnv *env;
  
  parseContext = (xmlParserCtxtPtr) ctx;
  saxErrorContext = (SaxErrorContext *) parseContext->_private;
  if (saxErrorContext == NULL)
    return;
  
  env = saxErrorContext->env;

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
  xmlParserCtxtPtr parseContext;
  SaxErrorContext *saxErrorContext;
  va_list va;
  
  parseContext = (xmlParserCtxtPtr) ctx;
  saxErrorContext = (SaxErrorContext *) parseContext->_private;
  if (saxErrorContext == NULL)
    return;
  
  va_start (va, msg);
  callErrorAdapterMethod (ctx, msg, va, saxErrorContext->saxErrorMethodID);
  va_end (va);
}

static void
xmljFatalErrorSAXFunc (void *ctx, const char *msg, ...)
{
  xmlParserCtxtPtr parseContext;
  SaxErrorContext *saxErrorContext;
  va_list va;
  
  parseContext = (xmlParserCtxtPtr) ctx;
  saxErrorContext = (SaxErrorContext *) parseContext->_private;
  if (saxErrorContext == NULL)
    return;
  
  va_start (va, msg);
  callErrorAdapterMethod (ctx, msg, va,
			  saxErrorContext->saxFatalErrorMethodID);
  va_end (va);
}

static void
xmljWarningSAXFunc (void *ctx, const char *msg, ...)
{
  xmlParserCtxtPtr parseContext;
  SaxErrorContext *saxErrorContext;
  va_list va;
  
  parseContext = (xmlParserCtxtPtr) ctx;
  saxErrorContext = (SaxErrorContext *) parseContext->_private;
  if (saxErrorContext == NULL)
    return;
  
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
  xmlParserCtxtPtr parseContext;
  SaxErrorContext *saxErrorContext;
  
  parseContext = (xmlParserCtxtPtr) ctx;
  saxErrorContext = (SaxErrorContext *) parseContext->_private;
  if (saxErrorContext == NULL)
    return;
  
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
  JNIEnv *env;
  
  if (errorContext == NULL)
    return;
  
  env = errorContext->env;
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
*/

void
xmljXsltErrorFunc (void *ctx, const char *msg, ...)
{
  if (NULL != ctx)
    {
      SAXParseContext *sax = ((SAXParseContext *) ctx);
      
      if (NULL != sax)
        {
          JNIEnv *env = sax->env;
          
          if (!(*env)->ExceptionOccurred (env))
            {
              jobject target = sax->obj;
              xmlChar *x_msg;
              jstring j_msg;
              va_list args;
              
              if (sax->error == NULL)
                {
                  sax->error =
                    xmljGetMethodID (env,
                                     target,
                                     "error",
                                 "(Ljava/lang/String;IILjava/lang/String;Ljava/lang/String;)V");
                  if (sax->error == NULL)
                    {
                      return;
                    }
                }
              
              va_start (args, msg);
              x_msg = (msg == NULL) ? NULL : xmlCharStrdup (msg);
              va_end (args);
              j_msg = xmljNewString (env, x_msg);
              
              (*env)->CallVoidMethod (env,
                                      target,
                                      sax->error,
                                      j_msg,
                                      -1,
                                      -1,
                                      NULL,
                                      NULL);
            }
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

void
xmljThrowException (JNIEnv *env,
                    const char *classname,
                    const char *message)
{
  jclass cls;
  jmethodID method;
  jthrowable ex;
  jstring jmsg;

  cls = (*env)->FindClass (env, classname);
  if (cls == NULL)
    {
      fprintf (stderr, "Can't find class %s\n", classname);
      return;
    }
  method = (*env)->GetMethodID (env, cls, "<init>", "(Ljava/lang/String;)V");
  if (method == NULL)
    {
      printf ("Can't find method %s.<init>\n", classname);
      return;
    }
  jmsg = (message == NULL) ? NULL : (*env)->NewStringUTF (env, message);
  ex = (jthrowable) (*env)->NewObject (env, cls, method, jmsg);
  if (ex == NULL)
    {
      fprintf (stderr, "Can't instantiate new %s\n", classname);
      return;
    }
  (*env)->Throw (env, ex);
}

void
xmljThrowDOMException (JNIEnv* env,
                       int code,
                       const char *message)
{
  jclass cls;
  jmethodID method;
  jthrowable ex;
  jstring jmsg;

  cls = (*env)->FindClass (env, "org/w3c/dom/DOMException");
  if (cls == NULL)
    {
      printf ("Can't find class org.w3c.dom.DOMException\n");
      return;
    }
  method = (*env)->GetMethodID (env, cls, "<init>", "(SLjava/lang/String;)V");
  if (method == NULL)
    {
      printf ("Can't find method org.w3c.dom.DOMException.<init>\n");
      return;
    }
  jmsg = (message == NULL) ? NULL : (*env)->NewStringUTF (env, message);
  ex = (jthrowable) (*env)->NewObject (env, cls, method, code, jmsg);
  (*env)->Throw (env, ex);
}

