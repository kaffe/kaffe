/* 
 * $Id: libxsltj.c,v 1.1 2004/04/14 19:40:14 dalibor Exp $
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

#include "gnu_xml_libxmlj_transform_LibxsltStylesheet.h"
#include "gnu_xml_libxmlj_transform_TransformerFactoryImpl.h"
#include "gnu_xml_libxmlj_transform_JavaContext.h"
#include "gnu_xml_libxmlj_transform_LibxmlDocument.h"

#include "xmlj_io.h"
#include "xmlj_error.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/HTMLtree.h>
#include <libxml/xmlIO.h>
#include <libxml/DOCBparser.h>
#include <libxml/xinclude.h>
#include <libxml/parser.h>
#include <libxml/catalog.h>
#include <libxslt/keys.h>
#include <libxslt/xslt.h>
#include <libxslt/xsltInternals.h>
#include <libxslt/transform.h>
#include <libxslt/xsltutils.h>
#include <libxslt/functions.h>
#include <libxslt/extensions.h>
#include <libxslt/documents.h>

/*
 * --------------------------------------------------------------------------
 * 
 * Native implementation for class
 * gnu.xml.libxmlj.transform.Transformer follows.
 */

static void
xmljSetProperty (JNIEnv * env, jobject outputProperties,
		 jmethodID setPropertyMethodID, const char *name,
		 const xmlChar * value)
{
  if (NULL != value)
    {
      jstring nameString = (*env)->NewStringUTF (env, name);
      jstring valueString = (*env)->NewStringUTF (env, (const char *) value);

      jobject prevValue = (*env)->CallObjectMethod (env, outputProperties,
						    setPropertyMethodID,
						    nameString, valueString);
      if (NULL != prevValue)
	{
	  (*env)->DeleteLocalRef (env, prevValue);
	}

      (*env)->DeleteLocalRef (env, nameString);
      (*env)->DeleteLocalRef (env, valueString);
    }
}

typedef struct CdataSectionScannerInfo_
{
  JNIEnv *env;
  jobject stringBuffer;
  jmethodID appendMethodID;
  int isFirst;
} CdataSectionScannerInfo;

static void
cdataSectionScanner (void *payload, void *data, xmlChar * name)
{
  CdataSectionScannerInfo *info = (CdataSectionScannerInfo *) data;
  JNIEnv *env = info->env;
  jstring nameString = (*env)->NewStringUTF (env, (const char *) name);
  jstring blankString = (*env)->NewStringUTF (env, " ");
  jobject stringBuffer;
  if (!info->isFirst)
    {
      stringBuffer
	= (*env)->CallObjectMethod (env,
				    info->stringBuffer,
				    info->appendMethodID, blankString);
      (*env)->DeleteLocalRef (env, stringBuffer);
    }
  info->isFirst = 0;
  stringBuffer
    = (*env)->CallObjectMethod (env,
				info->stringBuffer,
				info->appendMethodID, nameString);
  (*env)->DeleteLocalRef (env, stringBuffer);
  (*env)->DeleteLocalRef (env, blankString);
  (*env)->DeleteLocalRef (env, nameString);
}

void
xmljDocumentFunction (xmlXPathParserContextPtr ctxt, int nargs)
{
  xmlXPathObjectPtr obj, obj2 = NULL;

  if ((nargs < 1) || (nargs > 2))
    {
      xsltTransformError (xsltXPathGetTransformContext (ctxt), NULL, NULL,
			  "document() : invalid number of args %d\n", nargs);
      ctxt->error = XPATH_INVALID_ARITY;
      return;
    }
  if (ctxt->value == NULL)
    {
      xsltTransformError (xsltXPathGetTransformContext (ctxt), NULL, NULL,
			  "document() : invalid arg value\n");
      ctxt->error = XPATH_INVALID_TYPE;
      return;
    }

  if (nargs == 2)
    {
      if (ctxt->value->type != XPATH_NODESET)
	{
	  xsltTransformError (xsltXPathGetTransformContext (ctxt), NULL, NULL,
			      "document() : invalid arg expecting a nodeset\n");
	  ctxt->error = XPATH_INVALID_TYPE;
	  return;
	}

      obj2 = valuePop (ctxt);
    }

  if (ctxt->value->type == XPATH_NODESET)
    {
      int i;
      xmlXPathObjectPtr newobj, ret;

      obj = valuePop (ctxt);
      ret = xmlXPathNewNodeSet (NULL);

      if (obj->nodesetval)
	{
	  for (i = 0; i < obj->nodesetval->nodeNr; i++)
	    {
	      valuePush (ctxt,
			 xmlXPathNewNodeSet (obj->nodesetval->nodeTab[i]));
	      xmlXPathStringFunction (ctxt, 1);
	      if (nargs == 2)
		{
		  valuePush (ctxt, xmlXPathObjectCopy (obj2));
		}
	      else
		{
		  valuePush (ctxt,
			     xmlXPathNewNodeSet (obj->nodesetval->
						 nodeTab[i]));
		}
	      xsltDocumentFunction (ctxt, 2);
	      newobj = valuePop (ctxt);
	      ret->nodesetval = xmlXPathNodeSetMerge (ret->nodesetval,
						      newobj->nodesetval);
	      xmlXPathFreeObject (newobj);
	    }
	}

      xmlXPathFreeObject (obj);
      if (obj2 != NULL)
	xmlXPathFreeObject (obj2);
      valuePush (ctxt, ret);
      return;
    }
  /*
   * Make sure it's converted to a string
   */
  xmlXPathStringFunction (ctxt, 1);
  if (ctxt->value->type != XPATH_STRING)
    {
      xsltTransformError (xsltXPathGetTransformContext (ctxt), NULL, NULL,
			  "document() : invalid arg expecting a string\n");
      ctxt->error = XPATH_INVALID_TYPE;
      if (obj2 != NULL)
	xmlXPathFreeObject (obj2);
      return;
    }
  obj = valuePop (ctxt);
  if (obj->stringval == NULL)
    {
      valuePush (ctxt, xmlXPathNewNodeSet (NULL));
    }
  else
    {

      xsltTransformContextPtr tctxt;

      tctxt = xsltXPathGetTransformContext (ctxt);

      {
        SaxErrorContext *errorContext =
          (SaxErrorContext *) tctxt->style->_private;

        xmlDocPtr tree = xmljResolveURIAndOpen (errorContext,
                                                (const char*)obj->stringval, 
                                                NULL);

        xsltNewDocument (tctxt, tree);	/* FIXME - free at a later point */

        valuePush (ctxt, xmlXPathNewNodeSet ((xmlNodePtr) tree));
      }
    }
  xmlXPathFreeObject (obj);
  if (obj2 != NULL) {
    xmlXPathFreeObject (obj2);
  }
}

/*
 * Class:     gnu_xml_libxmlj_transform_LibxsltStylesheet
 * Method:    newLibxsltStylesheet
 * Signature: ([B)J
 */
JNIEXPORT jlong JNICALL
Java_gnu_xml_libxmlj_transform_LibxsltStylesheet_newLibxsltStylesheet(
  JNIEnv * env, jclass clazz, jobject inputStream, jstring inSystemId,
  jstring inPublicId, jobject errorAdapter, jobject outputProperties)
{
  xsltStylesheetPtr nativeStylesheetHandle = 0;

  xmlDocPtr xsltSourceDoc;

  /* xmlMemSetup (memcheck_free, memcheck_malloc, memcheck_realloc, memcheck_strdup); */
  xmlSetExternalEntityLoader (xmljLoadExternalEntity);

  xsltSourceDoc
    = xmljParseJavaInputStream (env, inputStream,
				inSystemId, inPublicId, errorAdapter);
  if (!(*env)->ExceptionOccurred (env))
    {
      jclass transformerExceptionClass
	= (*env)->FindClass (env, "javax/xml/transform/TransformerException");

      if (NULL != xsltSourceDoc)
	{
	  /* Read the style sheet and create Libxslt stylesheet object */

	  SaxErrorContext *errorContext =
	    xmljCreateSaxErrorContext (env, errorAdapter,
				       inSystemId,
				       inPublicId);
	  xmljSetThreadContext (errorContext);

	  if (!(*env)->ExceptionOccurred (env))
	    {
	      if (NULL != errorContext)
		{
		  xsltSetGenericErrorFunc (errorContext, xmljXsltErrorFunc);

		  nativeStylesheetHandle =
		    xsltParseStylesheetDoc (xsltSourceDoc);

		  if (!(*env)->ExceptionOccurred (env))
		    {
		      if (NULL != nativeStylesheetHandle)
			{
			  jclass propertiesClass
			    = (*env)->FindClass (env, "java/util/Properties");

			  jmethodID setPropertyMethodID
			    = (*env)->GetMethodID (env, propertiesClass,
						   "setProperty",
						   "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Object;");

			  nativeStylesheetHandle->_private = errorContext;

			  xmljSetProperty (env, outputProperties,
					   setPropertyMethodID, "encoding",
					   nativeStylesheetHandle->encoding);

			  xmljSetProperty (env, outputProperties,
					   setPropertyMethodID, "media-type",
					   nativeStylesheetHandle->mediaType);

			  xmljSetProperty (env, outputProperties,
					   setPropertyMethodID,
					   "doctype-public",
					   nativeStylesheetHandle->
					   doctypePublic);

			  xmljSetProperty (env, outputProperties,
					   setPropertyMethodID,
					   "doctype-system",
					   nativeStylesheetHandle->
					   doctypeSystem);

			  xmljSetProperty (env, outputProperties,
					   setPropertyMethodID, "indent",
					   (const xmlChar
					    *) (nativeStylesheetHandle->
						indent ? "yes" : "no"));

			  xmljSetProperty (env, outputProperties,
					   setPropertyMethodID, "method",
					   nativeStylesheetHandle->method);

			  xmljSetProperty (env, outputProperties,
					   setPropertyMethodID, "standalone",
					   (const xmlChar
					    *) (nativeStylesheetHandle->
						standalone ? "yes" : "no"));

			  xmljSetProperty (env, outputProperties,
					   setPropertyMethodID, "version",
					   nativeStylesheetHandle->version);

			  xmljSetProperty (env, outputProperties,
					   setPropertyMethodID,
					   "omit-xml-declaration",
					   (const xmlChar
					    *) (nativeStylesheetHandle->
						omitXmlDeclaration ? "yes" :
						"no"));

			  {
			    CdataSectionScannerInfo info;
			    jclass stringBufferClass
			      =
			      (*env)->FindClass (env,
						 "java/lang/StringBuffer");
			    jmethodID stringBufferConstructorID =
			      (*env)->GetMethodID (env, stringBufferClass,
						   "<init>", "()V");
			    jmethodID toStringMethodID =
			      (*env)->GetMethodID (env, stringBufferClass,
						   "toString",
						   "()Ljava/lang/String;");
			    info.env = env;
			    info.isFirst = 1;
			    info.stringBuffer
			      = (*env)->AllocObject (env, stringBufferClass);
			    (*env)->CallVoidMethod (env, info.stringBuffer,
						    stringBufferConstructorID);
			    info.appendMethodID =
			      (*env)->GetMethodID (env, stringBufferClass,
						   "append",
						   "(Ljava/lang/String;)Ljava/lang/StringBuffer;");

			    xmlHashScan (nativeStylesheetHandle->cdataSection,
					 cdataSectionScanner, &info);

			    {
			      jstring result = (jstring)
				(*env)->CallObjectMethod (env,
							  info.stringBuffer,
							  toStringMethodID);

			      jstring nameString =
				(*env)->NewStringUTF (env,
						      "cdata-section-elements");

			      jobject prevValue
				=
				(*env)->CallObjectMethod (env,
							  outputProperties,
							  setPropertyMethodID,
							  nameString, result);
			      if (NULL != prevValue)
				{
				  (*env)->DeleteLocalRef (env, prevValue);
				}
			      (*env)->DeleteLocalRef (env, nameString);
			    }

			    (*env)->DeleteLocalRef (env, info.stringBuffer);
			  }
			}
		      else
			{

			  (*env)->ThrowNew (env, transformerExceptionClass,
					    "Cannot interpret XSLT sheet");
			}
		    }
		}
	      else
		{

		  (*env)->ThrowNew (env, transformerExceptionClass,
				    "Cannot create internal error context object");
		}
	    }

	  xmljClearThreadContext ();
	}
      else
	{
	  (*env)->ThrowNew (env, transformerExceptionClass,
			    "Cannot parse XSLT sheet");
	}
    }

  /* Note: freeing xsltSourceDoc not allowed. */

  /* Return handle/address casted to Java long value */

  return (jlong) (int) nativeStylesheetHandle;
}

/*
 * Class:     gnu_xml_libxmlj_transform_LibxsltStylesheet
 * Method:    freeLibxsltStylesheet
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_transform_LibxsltStylesheet_freeLibxsltStylesheet
(JNIEnv * env, jclass clazz, jlong nativeStylesheetHandle)
{

  /* Cast Java long value to handle/address and free associated
   * libxslt resources.
   */

  xsltStylesheetPtr nativeStylesheet = (xsltStylesheetPtr) (int) nativeStylesheetHandle;
  nativeStylesheet->_private = NULL;
  xmlFreeDoc(nativeStylesheet->doc);
  nativeStylesheet->doc = NULL;
  xsltFreeStylesheet (nativeStylesheet);
}

xmlXPathFunction
xmljXPathFuncLookupFunc (void * ctxt,
			 const xmlChar * name, const xmlChar * ns_uri)
{
  if (0 == strcmp (name, "document") && NULL == ns_uri)
    {
      return xmljDocumentFunction;
    }
  else
    {
      return xsltXPathFunctionLookup (ctxt, name, ns_uri);
    }
}

/*
 * Class:     gnu_xml_libxmlj_transform_LibxsltStylesheet
 * Method:    libxsltTransform
 * Signature: ([B[BLjava/io/OutputStream;[Ljava/lang/String;)V
 */
JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_transform_LibxsltStylesheet_libxsltTransform(
  JNIEnv *env, jclass clazz, jlong xsltSource, jobject inputStream,
  jstring inSystemId, jstring inPublicId, jobject outputStream,
  jobjectArray parametersArray, jobject errorAdapter)
{
  /* Create the libxml source document for XML source data */

  xsltStylesheetPtr stylesheet = ((xsltStylesheetPtr) (int) xsltSource);

  {
    jclass javaContextClassID
      = (*env)->FindClass (env, "gnu/xml/libxmlj/transform/JavaContext");

    jmethodID parseDocumentCachedMethodID
      = (*env)->GetMethodID (env, javaContextClassID,
                             "parseDocumentCached",
                             "(Ljava/io/InputStream;Ljava/lang/String;Ljava/lang/String;)Lgnu/xml/libxmlj/transform/LibxmlDocument;");
    jclass libxmlDocumentClassID
      = (*env)->FindClass (env, "gnu/xml/libxmlj/transform/LibxmlDocument");

    jmethodID getNativeHandleMethodID
      = (*env)->GetMethodID (env, libxmlDocumentClassID,
                             "getNativeHandle", "()J");

    jobject libxmlDocument =
      (*env)->CallObjectMethod (env, 
                                errorAdapter,
                                parseDocumentCachedMethodID,
                                inputStream,
                                inSystemId,
                                inPublicId);

    xmlDocPtr xmlSourceDoc = 
      (xmlDocPtr)(int) (*env)->CallLongMethod (env, 
                                               libxmlDocument,
                                               getNativeHandleMethodID);

    (*env)->DeleteLocalRef(env, libxmlDocument);

    xmlSourceDoc->_private = (void*) (*env)->NewGlobalRef(env, libxmlDocument);

    if (!(*env)->ExceptionOccurred (env))
      {
	jclass transformerExceptionClass
	  =
	  (*env)->FindClass (env, "javax/xml/transform/TransformerException");

	if (NULL != xmlSourceDoc)
	  {
	    /* Transform the source document with the stylesheet
	     * and the given parameters. Return the result as
	     * byte array.
	     */
	    int parameterCount =
	      (*env)->GetArrayLength (env, parametersArray);

	    const char **parameters
	      =
	      (const char **) malloc ((parameterCount + 2) *
				      sizeof (const char *));

	    if (NULL != parameters)
	      {
		int i;

		for (i = 0; i < parameterCount; ++i)
		  {
		    jstring string =
		      (jstring) (*env)->GetObjectArrayElement (env,
							       parametersArray,
							       i);

		    if (NULL != string)
		      {
			parameters[i] =
			  (*env)->GetStringUTFChars (env, string, NULL);
		      }
		    else
		      {
			parameters[i] = NULL;
		      }
		  }

		parameters[parameterCount] = 0;
		parameters[parameterCount + 1] = 0;

		/* Apply stylesheet */
		{
		  xsltTransformContextPtr transformContext
		    = xsltNewTransformContext (stylesheet, xmlSourceDoc);

		  if (!(*env)->ExceptionOccurred (env))
		    {
		      if (NULL != transformContext)
			{
			  SaxErrorContext *saxErrorContext
			    = xmljCreateSaxErrorContext (env, errorAdapter,
							 inSystemId,
							 inPublicId);

			  xsltSetTransformErrorFunc (transformContext,
						     0, xmljXsltErrorFunc);

			  xmlXPathRegisterFuncLookup (transformContext->xpathCtxt,
						      xmljXPathFuncLookupFunc,
						      transformContext->xpathCtxt);

			  stylesheet->_private = saxErrorContext;

			  {
			    xmlDocPtr resultTree
			      = xsltApplyStylesheetUser (stylesheet,
							 xmlSourceDoc,
							 parameters,
							 NULL,
							 NULL,
							 transformContext);

			    if (!(*env)->ExceptionOccurred (env)
				&& NULL == resultTree)
			      {
				(*env)->ThrowNew (env,
						  transformerExceptionClass,
						  "XSL transformation failed");
			      }

			    if (!(*env)->ExceptionOccurred (env)
                                && NULL != resultTree)
			      {
				xmljSaveFileToJavaOutputStream (env,
								outputStream,
								resultTree,
								(const char*)
                                                                stylesheet->encoding);
			      }

                            if (NULL != resultTree)
                              {
                                xmlFreeDoc (resultTree);
                              }
			  }

			  xmljFreeSaxErrorContext ((SaxErrorContext *)
						   stylesheet->_private);

                          {
                            xsltDocumentPtr cur, doc;

                            cur = transformContext->docList;
                            while (cur != NULL) {
                              doc = cur;
                              cur = cur->next;
                              xsltFreeDocumentKeys(doc);
                              xmlFree(doc);
                            }
                            
                            cur = transformContext->styleList;
                            while (cur != NULL) {
                              doc = cur;
                              cur = cur->next;
                              xsltFreeDocumentKeys(doc);
                              xmlFree(doc);
                            }
                          }
                          transformContext->docList = NULL;
                          transformContext->styleList = NULL;
                          xsltFreeTransformContext (transformContext);
			}
		      else
			{
			  (*env)->ThrowNew (env,
					    transformerExceptionClass,
					    "Could not establish transform context");
			}
		    }
		}

		/* Release parameter strings */

		for (i = 0; i < parameterCount; ++i)
		  {
		    jstring string =
		      (jstring) (*env)->GetObjectArrayElement (env,
							       parametersArray,
							       i);

		    (*env)->ReleaseStringUTFChars (env, string,
						   parameters[i]);
		  }

		free (parameters);
	      }
	    else
	      {
		(*env)->ThrowNew (env, transformerExceptionClass,
				  "Couldn't allocate memory for parameters");
	      }
	  }
	else
	  {
	    (*env)->ThrowNew (env, transformerExceptionClass,
			      "Couldn't parse source document");
	  }
      }
    (*env)->DeleteGlobalRef(env, (jobject) xmlSourceDoc->_private);
  }
}

/*
 * --------------------------------------------------------------------------
 * Native implementation for class
 * gnu.xml.libxmlj.transform.TransformerFactoryImpl follows.
 */

/*
 * Class:     gnu_xml_libxmlj_transform_TransformerFactoryImpl
 * Method:    freeLibxsltGlobal
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_transform_TransformerFactoryImpl_freeLibxsltGlobal (
  JNIEnv *env, jclass clazz)
{
  xsltCleanupGlobals ();
  xmlCleanupParser ();
}

/*
 * Class:     gnu_xml_libxmlj_transform_JavaContext
 * Method:    parseDocument
 * Signature: (Ljava/io/InputStream;Ljava/lang/String;Ljava/lang/String;)J
 */
JNIEXPORT jlong JNICALL
Java_gnu_xml_libxmlj_transform_JavaContext_parseDocument (
  JNIEnv *env, jobject jthis, jobject inputStream, 
  jstring inSystemId, jstring inPublicId)
{
  xmlDocPtr tree = xmljParseJavaInputStream (env, inputStream,
					     inSystemId, inPublicId,
					     jthis);
  return (jlong) (int) tree;
}

/*
 * Class:     gnu_xml_libxmlj_transform_LibxmlDocument
 * Method:    freeDocument
 * Signature: (J)V
 */
JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_transform_LibxmlDocument_freeDocument (
  JNIEnv *env, jobject jthis, jlong nativeHandle)
{
  xmlFreeDoc ((xmlDocPtr) (int) nativeHandle);
}
