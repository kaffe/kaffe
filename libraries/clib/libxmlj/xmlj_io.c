/* 
 * $Id: xmlj_io.c,v 1.1 2004/04/14 19:40:14 dalibor Exp $
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

#include "xmlj_io.h"
#include "xmlj_error.h"

#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#include <libxml/xmlIO.h>
#include <libxml/parserInternals.h>

#include <pthread.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define DETECT_BUFFER_SIZE 50

typedef struct _OutputStreamContext
{

  JNIEnv *env;
  jobject outputStream;
  jmethodID outputStreamWriteFunc;
  jmethodID outputStreamCloseFunc;

}
OutputStreamContext;

typedef struct _InputStreamContext
{

  JNIEnv *env;
  jobject inputStream;
  jmethodID inputStreamReadFunc;
  jmethodID inputStreamCloseFunc;
  jobject bufferByteArray;
  jint bufferLength;

}
InputStreamContext;

InputStreamContext *
xmljNewInputStreamContext (JNIEnv * env,
                           jobject inputStream);

void 
xmljFreeInputStreamContext (InputStreamContext * inContext);

int 
xmljInputReadCallback (void *context, char *buffer, int len);

int 
xmljInputCloseCallback (void *context);

int 
xmljOutputWriteCallback (void *context, const char *buffer, int len);

int 
xmljOutputCloseCallback (void *context);

OutputStreamContext *
xmljNewOutputStreamContext (JNIEnv * env,
                            jobject outputStream);


int
xmljOutputWriteCallback (void *context, const char *buffer, int len)
{
  OutputStreamContext *outContext = (OutputStreamContext *) context;
  JNIEnv *env = outContext->env;
  jbyteArray byteArray = (*env)->NewByteArray (env, len);

  if (0 != byteArray)
    {
      (*env)->SetByteArrayRegion (env, byteArray, 0, len, (jbyte *) buffer);

      (*env)->CallVoidMethod (env,
			      outContext->outputStream,
			      outContext->outputStreamWriteFunc, byteArray);

      (*env)->DeleteLocalRef (env, byteArray);

      return (*env)->ExceptionOccurred (env) ? -1 : len;
    }
  else
    {
      /* Out of memory, signal error */
      return -1;
    }
}

int
xmljOutputCloseCallback (void *context)
{
  OutputStreamContext *outContext = (OutputStreamContext *) context;
  JNIEnv *env = outContext->env;
  (*env)->CallVoidMethod (env,
			  outContext->outputStream,
			  outContext->outputStreamCloseFunc);

  return (*env)->ExceptionOccurred (env) ? -1 : 0;
}

int
xmljInputReadCallback (void *context, char *buffer, int len)
{
  InputStreamContext *inContext = (InputStreamContext *) context;
  JNIEnv *env = inContext->env;

  jint nread = 0;
  int offset = 0;

  while (offset < len && nread >= 0)
    {
      nread = (*env)->CallIntMethod (env,
				     inContext->inputStream,
				     inContext->inputStreamReadFunc,
				     inContext->bufferByteArray,
				     0, MIN (len - offset,
					     inContext->bufferLength));

      if (nread > 0)
	{
	  (*env)->GetByteArrayRegion (env,
				      inContext->bufferByteArray,
				      0, nread, ((jbyte *) buffer) + offset);

	  offset += nread;
	}
    }

  return (*env)->ExceptionOccurred (env) ? -1 : offset;
}

int
xmljInputCloseCallback (void *context)
{
  InputStreamContext *inContext = (InputStreamContext *) context;
  JNIEnv *env = inContext->env;
  (*env)->CallVoidMethod (env, inContext->inputStream,
			  inContext->inputStreamCloseFunc);

  return (*env)->ExceptionOccurred (env) ? -1 : 0;
}

InputStreamContext *
xmljNewInputStreamContext (JNIEnv * env, jobject inputStream)
{
  jclass inputStreamClass = (*env)->FindClass (env, "java/io/InputStream");
  InputStreamContext *result =
    (InputStreamContext *) malloc (sizeof (InputStreamContext));

  result->env = env;
  result->inputStream = inputStream;
  result->inputStreamReadFunc =
    (*env)->GetMethodID (env, inputStreamClass, "read", "([BII)I");
  result->inputStreamCloseFunc =
    (*env)->GetMethodID (env, inputStreamClass, "close", "()V");
  result->bufferLength = 4000;
  result->bufferByteArray = (*env)->NewByteArray (env, result->bufferLength);
  return result;
}

void
xmljFreeInputStreamContext (InputStreamContext * inContext)
{
  JNIEnv *env = inContext->env;

  (*env)->DeleteLocalRef (env, inContext->bufferByteArray);
  free (inContext);
}

OutputStreamContext *
xmljNewOutputStreamContext (JNIEnv * env, jobject outputStream)
{
  jclass outputStreamClass = (*env)->FindClass (env, "java/io/OutputStream");
  OutputStreamContext *result =
    (OutputStreamContext *) malloc (sizeof (OutputStreamContext));
  result->env = env;
  result->outputStream = outputStream;
  result->outputStreamWriteFunc =
    (*env)->GetMethodID (env, outputStreamClass, "write", "([B)V");
  result->outputStreamCloseFunc =
    (*env)->GetMethodID (env, outputStreamClass, "close", "()V");
  return result;
}


void
xmljFreeOutputStreamContext (OutputStreamContext * outContext)
{
  free (outContext);
}

xmlCharEncoding
xmljDetectCharEncoding (JNIEnv * env, jobject pushbackInputStream)
{
  jclass pushbackInputStreamClass
    = (*env)->FindClass (env, "java/io/PushbackInputStream");

  jbyteArray buffer = (*env)->NewByteArray (env, DETECT_BUFFER_SIZE);

  jmethodID readMethod
    = (*env)->GetMethodID (env, pushbackInputStreamClass, "read", "([B)I");

  jmethodID unreadMethod
    =
    (*env)->GetMethodID (env, pushbackInputStreamClass, "unread", "([BII)V");

  jint nread =
    (*env)->CallIntMethod (env, pushbackInputStream, readMethod, buffer);

  if (nread > 0)
    {
      jbyte nativeBuffer[DETECT_BUFFER_SIZE + 1];


      (*env)->CallVoidMethod (env, pushbackInputStream, unreadMethod,
			      buffer, 0, nread);

      memset (nativeBuffer, 0, DETECT_BUFFER_SIZE + 1);

      (*env)->GetByteArrayRegion (env, buffer, 0, nread, nativeBuffer);

      (*env)->DeleteLocalRef (env, buffer);

      if (nread >= 5)
	{
	  return xmlDetectCharEncoding ((unsigned char *) nativeBuffer,
					nread);
	}
      else
	{
	  return XML_CHAR_ENCODING_NONE;
	}
    }
  else
    {
      return XML_CHAR_ENCODING_ERROR;
    }
}

xmlParserCtxtPtr
xmljEstablishParserContext (JNIEnv * env,
			    jobject inputStream,
			    jstring inSystemId,
			    jstring inPublicId, jobject saxErrorAdapter)
{
  xmlCharEncoding encoding = xmljDetectCharEncoding (env, inputStream);

  InputStreamContext *inputContext =
    xmljNewInputStreamContext (env, inputStream);

  if (NULL != inputContext)
    {
      xmlParserCtxtPtr inputParserCtx 
        = xmlCreateIOParserCtxt (NULL, NULL,        
                                 /* NOTE: userdata must be NULL for DOM to work */
                                 xmljInputReadCallback,
                                 xmljInputCloseCallback,
                                 inputContext, encoding);

      xmljInitErrorHandling (inputParserCtx->sax);

      inputParserCtx->userData = inputParserCtx;

      if (NULL != inputParserCtx)
        {
          SaxErrorContext *saxErrorContext
            = xmljCreateSaxErrorContext (env, saxErrorAdapter,
                                         inSystemId, inPublicId);
          
          if (NULL != saxErrorContext)
            {
              inputParserCtx->_private = saxErrorContext;
              
              return inputParserCtx;
            }
          xmlFreeParserCtxt (inputParserCtx);
          xmljFreeSaxErrorContext (saxErrorContext);
        }
    }

  xmljFreeInputStreamContext (inputContext);
  return NULL;
}

void
xmljReleaseParserContext (xmlParserCtxtPtr inputParserCtx)
{
  SaxErrorContext *saxErrorContext
    = (SaxErrorContext *) inputParserCtx->_private;

  InputStreamContext *inputStreamContext
    = (InputStreamContext *) inputParserCtx->input->buf->context;

  xmljFreeSaxErrorContext (saxErrorContext);

  xmlFreeParserCtxt (inputParserCtx);

  xmljFreeInputStreamContext (inputStreamContext);
}

xmlDocPtr
xmljParseJavaInputStream (JNIEnv * env,
			  jobject inputStream,
			  jstring inSystemId,
			  jstring inPublicId, jobject saxErrorAdapter)
{
  xmlDocPtr tree = NULL;

  xmlParserCtxtPtr inputParserCtx
    = xmljEstablishParserContext (env, inputStream,
				  inSystemId,
				  inPublicId,
				  saxErrorAdapter);

  if (NULL != inputParserCtx)
    {
      xmljSetThreadContext ((SaxErrorContext *) inputParserCtx->_private);

      if (0 == xmlParseDocument (inputParserCtx))
	{
	  tree = inputParserCtx->myDoc;
	}
      else
	{
	  /* ... */
	}

      xmljClearThreadContext ();

      xmljReleaseParserContext (inputParserCtx);
    }


  return tree;
}


void
xmljSaveFileToJavaOutputStream (JNIEnv * env, jobject outputStream,
				xmlDocPtr tree,
				const char *outputEncodingName)
{
  OutputStreamContext *outputContext =
    xmljNewOutputStreamContext (env, outputStream);

  xmlCharEncoding outputEncoding = xmlParseCharEncoding (outputEncodingName);

  xmlOutputBufferPtr outputBuffer =
    xmlOutputBufferCreateIO (xmljOutputWriteCallback,
			     xmljOutputCloseCallback,
			     outputContext,
			     xmlGetCharEncodingHandler (outputEncoding));

  /* Write result to output stream */

  xmlSaveFileTo (outputBuffer, tree, outputEncodingName);

  xmljFreeOutputStreamContext (outputContext);
}

jobject
xmljResolveURI (SaxErrorContext * saxErrorContext,
		const char *URL, const char *ID)
{
  JNIEnv *env = saxErrorContext->env;

  jstring hrefString = (*env)->NewStringUTF (env, URL);
  jstring baseString = saxErrorContext->systemId;

  jobject sourceWrapper = (*env)->CallObjectMethod (env,
						    saxErrorContext->
						    saxErrorAdapter,
						    saxErrorContext->
						    resolveURIMethodID,
						    hrefString,
						    baseString);
  (*env)->DeleteLocalRef (env, hrefString);

  if (NULL == sourceWrapper)
    {
      return NULL;
    }
  else
    {
      jobject sourceInputStream = (*env)->CallObjectMethod (env,
							    sourceWrapper,
							    saxErrorContext->
							    getInputStreamMethodID);

      (*env)->DeleteLocalRef (env, sourceWrapper);

      if ((*env)->ExceptionOccurred (env))
	{
	  /* Report to ErrorAdapter here? */
	  return NULL;
	}

      return sourceInputStream;
    }
}

xmlDocPtr
xmljResolveURIAndOpen (SaxErrorContext * saxErrorContext,
		       const char *URL, const char *ID)
{
  JNIEnv *env = saxErrorContext->env;

  jstring hrefString = (*env)->NewStringUTF (env, URL);
  jstring baseString = saxErrorContext->systemId;

  jobject libxmlDocument
    = (*env)->CallObjectMethod (env,
                                saxErrorContext->saxErrorAdapter,
                                saxErrorContext->
                                resolveURIAndOpenMethodID,
                                hrefString,
                                baseString);

  jlong tree
    = (*env)->CallLongMethod (env,
                              libxmlDocument,
                              saxErrorContext->
                              getNativeHandleMethodID);

  (*env)->DeleteLocalRef(env, libxmlDocument);

  if ((*env)->ExceptionOccurred (env))
    {
      /* Report to ErrorAdapter here? */
      return NULL;
    }
  else
    {
      return (xmlDocPtr) (int) tree;
    }
}

xmlParserInputPtr
xmljLoadExternalEntity (const char *URL, const char *ID,
			xmlParserCtxtPtr ctxt)
{
  SaxErrorContext *saxErrorContext = xmljGetThreadContext ();

  JNIEnv *env = saxErrorContext->env;

  jstring hrefString = (*env)->NewStringUTF (env, URL);
  jstring baseString = saxErrorContext->systemId;

  jobject sourceWrapper = (*env)->CallObjectMethod (env,
						    saxErrorContext->
						    saxErrorAdapter,
						    saxErrorContext->
						    resolveURIMethodID,
						    hrefString,
						    baseString);

  (*env)->DeleteLocalRef (env, hrefString);

  if (NULL == sourceWrapper)
    {
      return NULL;
    }
  else
    {
      InputStreamContext *inputContext;
      xmlParserInputBufferPtr inputBuffer;
      xmlParserInputPtr inputStream;

      jobject sourceInputStream = (*env)->CallObjectMethod (env,
							    sourceWrapper,
							    saxErrorContext->
							    getInputStreamMethodID);

      (*env)->DeleteLocalRef (env, sourceWrapper);

      if ((*env)->ExceptionOccurred (env))
	{
	  /* Report to ErrorAdapter */
	  return NULL;
	}

      inputContext =
	xmljNewInputStreamContext (env, sourceInputStream);

      inputBuffer
	= xmlParserInputBufferCreateIO (xmljInputReadCallback,
					xmljInputCloseCallback,
					inputContext,
					XML_CHAR_ENCODING_NONE);

      inputStream = xmlNewInputStream (ctxt);
      if (inputStream == NULL)
	{
	  return (NULL);
	}

      inputStream->filename = NULL;
      inputStream->directory = NULL;
      inputStream->buf = inputBuffer;

      inputStream->base = inputStream->buf->buffer->content;
      inputStream->cur = inputStream->buf->buffer->content;
      inputStream->end = &inputStream->base[inputStream->buf->buffer->use];
      if ((ctxt->directory == NULL) && (inputStream->directory != NULL))
	ctxt->directory =
	  (char *) xmlStrdup ((const xmlChar *) inputStream->directory);
      return (inputStream);
    }
}

/* Key for the thread-specific buffer */
static pthread_key_t thread_context_key;

/* Once-only initialisation of the key */
static pthread_once_t thread_context_once = PTHREAD_ONCE_INIT;

/* Allocate the key */
static void 
thread_context_key_alloc()
{
  pthread_key_create(&thread_context_key, NULL);
}

void 
xmljSetThreadContext(SaxErrorContext *context)
{
  pthread_once(&thread_context_once, thread_context_key_alloc);
  pthread_setspecific(thread_context_key, context);
}

void 
xmljClearThreadContext(void)
{
  pthread_setspecific(thread_context_key, NULL);
}

/* Return the thread-specific buffer */
SaxErrorContext *
xmljGetThreadContext(void)
{
  return (SaxErrorContext *) pthread_getspecific(thread_context_key);
}
