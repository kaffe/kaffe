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

#include "gnu_xml_libxmlj_transform_GnomeTransformerFactory.h"
#include "gnu_xml_libxmlj_transform_JavaContext.h"
#include "gnu_xml_libxmlj_transform_LibxsltStylesheet.h"

#include "xmlj_dom.h"
#include "xmlj_io.h"
#include "xmlj_error.h"
#include "xmlj_node.h"
#include "xmlj_sax.h"
#include "xmlj_util.h"

#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#include <libxml/xmlmemory.h>
#include <libxml/debugXML.h>
#include <libxml/xmlIO.h>
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
        SAXParseContext *saxContext =
          (SAXParseContext *) tctxt->style->_private;

        xmlDocPtr tree = xmljResolveURIAndOpen (saxContext,
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

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_transform_LibxsltStylesheet_newLibxsltStylesheet(
  JNIEnv * env, jclass clazz, jobject inputStream, jbyteArray detectBuffer,
  jstring publicId, jstring systemId, jstring base,
  jobject javaContext, jobject outputProperties)
{
  xsltStylesheetPtr nativeStylesheetHandle = 0;
  xmlDocPtr xsltSourceDoc;
  xmlParserCtxtPtr ctx;
  SAXParseContext *saxParseContext;
  xmlSAXHandlerPtr sax;

  /* xmlMemSetup (memcheck_free, memcheck_malloc, memcheck_realloc, memcheck_strdup); */
  ctx = xmljNewParserContext (env, inputStream, detectBuffer, publicId,
                              systemId, base, 0, 0, 0, 0);
  if (ctx == NULL)
    {
      return 0;
    }

  saxParseContext = 
    xmljNewSAXParseContext (env, javaContext, ctx, publicId, systemId);
  if (saxParseContext == NULL)
    {
      xmljFreeParserContext (ctx);
      return 0;
    }
  
  sax = xmljNewSAXHandler (0, 0, 0, 0, 0, 0);
  if (sax == NULL)
    {
      xmljFreeSAXParseContext (saxParseContext);
      xmljFreeParserContext (ctx);
      return 0;
    }
  
  xsltSourceDoc
    = xmljParseDocument2 (env, ctx, saxParseContext, sax, 2);
  
  if (!(*env)->ExceptionOccurred (env) && NULL != xsltSourceDoc)
    {
      /* Read the style sheet and create Libxslt stylesheet object */
      
      xmljSetThreadContext (saxParseContext);
      
      /*xsltSetGenericErrorFunc (ctx, xmljSAXError);*/
          
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
              
              nativeStylesheetHandle->_private = saxParseContext;
              
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
              xmljThrowException (env,
                                  "javax/xml/transform/TransformerException",
                                  "Cannot interpret XSLT sheet");
            }
        }
      xmljClearThreadContext ();
    }

  /* Note: freeing xsltSourceDoc not allowed. */

  /*free (sax);*/
  xmljFreeSAXParseContext (saxParseContext);
  /*xmljFreeParserContext (ctx);*/
  
  /* Return handle/address casted to Java int value */
  return xmljAsField (env, nativeStylesheetHandle);
}

JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_transform_LibxsltStylesheet_freeLibxsltStylesheet
(JNIEnv * env, jclass clazz, jobject nativeStylesheetHandle)
{

  /* Cast Java int value to handle/address and free associated
   * libxslt resources.
   */

  xsltStylesheetPtr stylesheet;
 
  stylesheet = (xsltStylesheetPtr) xmljAsPointer (env, nativeStylesheetHandle);
  stylesheet->_private = NULL;
  xmlFreeDoc (stylesheet->doc);
  stylesheet->doc = NULL;
  xsltFreeStylesheet (stylesheet);
}

xmlXPathFunction
xmljXPathFuncLookupFunc (void * ctxt,
			 const xmlChar * name, const xmlChar * ns_uri)
{
  xmlChar *document = xmlCharStrdup ("document");
  xmlXPathFunction f;
  
  if (xmlStrEqual (name, document) && NULL == ns_uri)
    {
      f = xmljDocumentFunction;
    }
  else
    {
      f = xsltXPathFunctionLookup (ctxt, name, ns_uri);
    }
  xmlFree (document);
  return f;
}

JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_transform_LibxsltStylesheet_libxsltTransform(
  JNIEnv *env, jclass clazz, jobject xsltSource, jobject jdocument,
  jobject outputStream, jobjectArray parametersArray, jobject javaContext)
{
  xsltStylesheetPtr stylesheet;
  xmlDocPtr sourceDoc;
  xmlDocPtr resultDoc;
  
  stylesheet = (xsltStylesheetPtr) xmljAsPointer (env, xsltSource);
  sourceDoc = (xmlDocPtr) xmljGetNodeID (env, jdocument);
  
  if (!(*env)->ExceptionOccurred (env) && NULL != sourceDoc)
    {
      /* Transform the source document with the stylesheet
       * and the given parameters. Return the result as
       * byte array.
       */
      int parameterCount =
        (*env)->GetArrayLength (env, parametersArray);
      
      const char **parameters =
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
          resultDoc = xsltApplyStylesheet(stylesheet, sourceDoc, parameters);
          if (resultDoc == NULL)
            {
              xmljThrowException (env,
                                  "javax/xml/transform/TransformerException",
                                  "XSL transformation failed");
            }
          else
            {
              xmljSaveFileToJavaOutputStream (env,
                                              outputStream,
                                              resultDoc,
                                              (const char*) stylesheet->encoding);
              xmlFreeDoc (resultDoc);
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
          xmljThrowException (env,
                              "javax/xml/transform/TransformerException",
                              "Couldn't allocate memory for parameters");
        }
    }
}

/*
 * --------------------------------------------------------------------------
 * Native implementation for class
 * gnu.xml.libxmlj.transform.GnomeTransformerFactory follows.
 */

/*
 * Class:     gnu_xml_libxmlj_transform_GnomeTransformerFactory
 * Method:    freeLibxsltGlobal
 * Signature: ()V
 */
JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_transform_GnomeTransformerFactory_freeLibxsltGlobal (
  JNIEnv *env, jclass clazz)
{
  xsltCleanupGlobals ();
  xmlCleanupParser ();
}

/*
 * Class:     gnu_xml_libxmlj_transform_JavaContext
 * Method:    parseDocument
 * Signature: (Ljava/io/InputStream;Ljava/lang/String;Ljava/lang/String;)Lgnu/xml/libxmlj/dom/GnomeDocument
 */
JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_transform_JavaContext_parseDocument (JNIEnv *env,
                                                          jobject self,
                                                          jobject in,
                                                          jbyteArray
                                                          detectBuffer,
                                                          jstring publicId,
                                                          jstring systemId,
                                                          jstring base)
{
  xmlDocPtr doc = xmljParseDocument(env,
                                    self,
                                    in,
                                    detectBuffer,
                                    publicId,
                                    systemId,
                                    base,
                                    0, 0, 0,
                                    0, 0, 0, 0, 0, 0,
                                    0);
  return xmljCreateDocument (env, NULL, doc);
}

