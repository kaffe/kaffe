#include "gnu_xml_libxmlj_dom_GnomeDocument.h"
#include "gnu_xml_libxmlj_dom_GnomeElement.h"
#include "gnu_xml_libxmlj_dom_GnomeXPathExpression.h"
#include "gnu_xml_libxmlj_dom_GnomeXPathNodeList.h"
#include "gnu_xml_libxmlj_dom_GnomeXPathNSResolver.h"
#include "gnu_xml_libxmlj_dom_GnomeXPathResult.h"
#include "xmlj_node.h"
#include "xmlj_util.h"
#include <libxml/xpath.h>

xmlXPathContextPtr
xmljCreateXPathContextPtr (JNIEnv *env, xmlNodePtr node)
{
  xmlXPathContextPtr ctx;

  ctx = xmlXPathNewContext (node->doc);
  ctx->node = node;
  return ctx;
}

/**
 * Converts an xmlXPathObjectPtr to a Java XPathResult.
 */
jobject
xmljGetXPathResult (JNIEnv *env, xmlXPathObjectPtr obj)
{
  jclass cls;
  jmethodID method;
  jobject ret;
  jobject val;
  
  if (obj == NULL)
    {
      return NULL;
    }
  cls = (*env)->FindClass (env, "gnu/xml/libxmlj/dom/GnomeXPathResult");
  if (cls == NULL)
    {
      return NULL;
    }
  method = (*env)->GetMethodID (env, cls, "<init>", "(Ljava/lang/Object;)V");
  if (method == NULL)
    {
      return NULL;
    }
  val = xmljAsField (env, obj);
  ret = (*env)->NewObject (env, cls, method, val);
  
  return ret;
}

/**
 * Converts an xmlXPathObjectPtr to a Java XPathNodeList.
 */
jobject
xmljGetXPathNodeList (JNIEnv *env, xmlXPathObjectPtr obj)
{
  jclass cls;
  jmethodID method;
  jobject ret;
  jobject val;
  
  if (obj == NULL)
    {
      return NULL;
    }
  cls = (*env)->FindClass (env, "gnu/xml/libxmlj/dom/GnomeXPathNodeList");
  if (cls == NULL)
    {
      return NULL;
    }
  method = (*env)->GetMethodID (env, cls, "<init>", "(Ljava/lang/Object;)V");
  if (method == NULL)
    {
      return NULL;
    }
  val = xmljAsField (env, obj);
  ret = (*env)->NewObject (env, cls, method, val);
  
  return ret;
}

xmlXPathObjectPtr
xmljGetXPathObjectID (JNIEnv *env, jobject obj)
{
  jclass cls;
  jfieldID field;
  jobject val;
  xmlXPathObjectPtr ret;

  cls = (*env)->GetObjectClass (env, obj);
  if (cls == NULL)
    {
      return NULL;
    }
  field = (*env)->GetFieldID (env, cls, "obj", "Ljava/lang/Object;");
  if (field == NULL)
    {
      return NULL;
    }
  val = (*env)->GetObjectField (env, obj, field);
  ret = (xmlXPathObjectPtr) xmljAsPointer (env, val);

  return ret;
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocument_evaluate (JNIEnv *env,
                                                 jobject self,
                                                 jstring expression,
                                                 jobject contextNode,
                                                 jobject resolver,
                                                 jshort type,
                                                 jobject result)
{
  const xmlChar *str;
  xmlNodePtr node;
  xmlXPathContextPtr ctx;
  xmlXPathObjectPtr eval = NULL;
  
  str = xmljGetStringChars (env, expression);
  node = xmljGetNodeID (env, contextNode);
  if (node == NULL)
    {
      return NULL;
    }
  ctx = xmljCreateXPathContextPtr (env, node);
  if (ctx != NULL)
    {
      eval = xmlXPathEval (str, ctx);
      xmlXPathFreeContext (ctx);
    }
  return xmljGetXPathResult (env, eval);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathExpression_init (JNIEnv *env,
                                                    jobject self,
                                                    jstring expression)
{
  const xmlChar *str;

  str = xmljGetStringChars (env, expression);
  return xmljAsField (env, xmlXPathCompile (str));
}

JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathExpression_free (JNIEnv *env,
                                                    jobject self,
                                                    jobject ptr)
{
  xmlXPathCompExprPtr expr;

  expr = (xmlXPathCompExprPtr) xmljAsPointer (env, ptr);
  xmlXPathFreeCompExpr (expr);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathExpression_evaluate (JNIEnv *env,
                                                        jobject self,
                                                        jobject ptr,
                                                        jobject contextNode,
                                                        jshort type,
                                                        jobject result)
{
  xmlXPathCompExprPtr expr;
  xmlNodePtr node;
  xmlXPathContextPtr ctx;
  xmlXPathObjectPtr eval = NULL;

  expr = (xmlXPathCompExprPtr) xmljAsPointer (env, ptr);
  node = xmljGetNodeID (env, contextNode);
  if (node == NULL)
    {
      return NULL;
    }
  ctx = xmljCreateXPathContextPtr (env, node);
  if (ctx != NULL)
    {
      eval = xmlXPathCompiledEval (expr, ctx);
      xmlXPathFreeContext (ctx);
    }
  return xmljGetXPathResult (env, eval);
}

JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathResult_free (JNIEnv *env,
                                                jobject self,
                                                jobject obj)
{
  xmlXPathFreeObject ((xmlXPathObjectPtr) xmljAsPointer (env, obj));
}

JNIEXPORT jshort JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathResult_getResultType (JNIEnv *env,
                                                         jobject self)
{
  xmlXPathObjectPtr obj;
  
  obj = xmljGetXPathObjectID (env, self);
  switch (obj->type)
    {
    case XPATH_UNDEFINED:
      return 0; /* ANY_TYPE */
    case XPATH_NUMBER:
      return 1; /* NUMBER_TYPE */
    case XPATH_STRING:
      return 2; /* STRING_TYPE */
    case XPATH_BOOLEAN:
      return 3; /* BOOLEAN_TYPE */
    case XPATH_NODESET:
      return 6; /* UNORDERED_NODE_SNAPSHOT_TYPE */
    case XPATH_POINT:
    case XPATH_RANGE:
    case XPATH_LOCATIONSET:
    case XPATH_USERS:
    case XPATH_XSLT_TREE:
      /* TODO */
    default:
      return -1; /* TODO */
    }
}

JNIEXPORT jdouble JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathResult_getNumberValue (JNIEnv *env,
                                                          jobject self)
{
  xmlXPathObjectPtr obj;
  
  obj = xmljGetXPathObjectID (env, self);
  if (obj == NULL)
    {
      return 0.0;
    }
  return obj->floatval;
}

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathResult_getStringValue (JNIEnv *env,
                                                          jobject self)
{
  xmlXPathObjectPtr obj;
  
  obj = xmljGetXPathObjectID (env, self);
  if (obj == NULL)
    {
      return NULL;
    }
  return xmljNewString (env, obj->stringval);
}

JNIEXPORT jboolean JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathResult_getBooleanValue (JNIEnv *env,
                                                           jobject self)
{
  xmlXPathObjectPtr obj;
  
  obj = xmljGetXPathObjectID (env, self);
  return obj->boolval;
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathResult_getSingleNodeValue (JNIEnv *env,
                                                              jobject self)
{
  xmlXPathObjectPtr obj;
  
  obj = xmljGetXPathObjectID (env, self);
  if (obj == NULL)
    {
      return NULL;
    }
  if (obj->nodesetval == NULL)
    {
      return NULL;
    }
  if (obj->nodesetval->nodeNr > 0)
    {
      return xmljGetNodeInstance (env, obj->nodesetval->nodeTab[0]);
    }
  else
    {
      return NULL;
    }
}

JNIEXPORT jboolean JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathResult_getInvalidIteratorState (JNIEnv *env,
                                                                   jobject self)
{
  xmlXPathObjectPtr obj;
  
  obj = xmljGetXPathObjectID (env, self);
  return 0; /* TODO */
}

JNIEXPORT jint JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathResult_getSnapshotLength (JNIEnv *env,
                                                             jobject self)
{
  xmlXPathObjectPtr obj;
  
  obj = xmljGetXPathObjectID (env, self);
  if (obj == NULL)
    {
      return -1;
    }
  if (obj->nodesetval == NULL)
    {
      return -1;
    }
  return obj->nodesetval->nodeNr;
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathResult_iterateNext (JNIEnv *env,
                                                       jobject self)
{
  xmlXPathObjectPtr obj;
  
  obj = xmljGetXPathObjectID (env, self);
  return NULL; /* TODO */
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathResult_snapshotItem (JNIEnv *env,
                                                        jobject self,
                                                        jint index)
{
  xmlXPathObjectPtr obj;
  
  obj = xmljGetXPathObjectID (env, self);
  if (obj == NULL)
    {
      return NULL;
    }
  if (obj->nodesetval == NULL)
    {
      return NULL;
    }
  if (obj->nodesetval->nodeNr > 0)
    {
      return xmljGetNodeInstance (env, obj->nodesetval->nodeTab[index]);
    }
  else
    {
      return NULL;
    }
}

/* -- GnomeXPathNodeList -- */

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocument_getElementsByTagName (JNIEnv *env,
                                                             jobject self,
                                                             jstring name)
{
  return Java_gnu_xml_libxmlj_dom_GnomeElement_getElementsByTagName (env,
                                                                     self,
                                                                     name);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeElement_getElementsByTagName (JNIEnv *env,
                                                             jobject self,
                                                             jstring name)
{
  const xmlChar *s_name;
  const xmlChar *format;
  xmlChar expr[256];
  xmlNodePtr node;
  xmlXPathContextPtr ctx;
  xmlXPathObjectPtr eval = NULL;
  
  s_name = xmljGetStringChars (env, name);
  if (xmlStrEqual (s_name, BAD_CAST "*"))
    {
      format = xmlCharStrdup ("descendant-or-self::*[node-type()=1]");
      if (xmlStrPrintf (expr, 256, format) == -1)
        {
          return NULL;
        }
    }
  else
    {
      format = xmlCharStrdup ("descendant-or-self::*[name()='%s']");
      if (xmlStrPrintf (expr, 256, format, s_name) == -1)
        {
          return NULL;
        }
    }
  node = xmljGetNodeID (env, self);
  if (node == NULL)
    {
      return NULL;
    }
  ctx = xmljCreateXPathContextPtr (env, node);
  if (ctx != NULL)
    {
      eval = xmlXPathEval (expr, ctx);
      xmlXPathFreeContext (ctx);
    }
  return xmljGetXPathNodeList (env, eval);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocument_getElementsByTagNameNS (JNIEnv *env,
                                                               jobject self,
                                                               jstring uri,
                                                               jstring localName)
{
  return Java_gnu_xml_libxmlj_dom_GnomeElement_getElementsByTagNameNS (env,
                                                                       self,
                                                                       uri,
                                                                       localName);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeElement_getElementsByTagNameNS (JNIEnv *env,
                                                               jobject self,
                                                               jstring uri,
                                                               jstring localName)
{
  const xmlChar *s_uri;
  const xmlChar *s_localName;
  const xmlChar *format;
  xmlChar expr[256];
  xmlNodePtr node;
  xmlXPathContextPtr ctx;
  xmlXPathObjectPtr eval = NULL;
  
  s_uri = xmljGetStringChars (env, uri);
  s_localName = xmljGetStringChars (env, localName);
  if (uri == NULL)
    {
      /* namespace URI is empty */
      if (xmlStrEqual (s_localName, BAD_CAST "*"))
        {
          format = xmlCharStrdup ("descendant-or-self::*[namespace-uri()='' and node-type()=1]");
          if (xmlStrPrintf (expr, 256, format) == -1)
            {
              return NULL;
            }
        }
      else
        {
          format = xmlCharStrdup ("descendant-or-self::*[namespace-uri()='' and local-name()='%s']");
          if (xmlStrPrintf (expr, 256, format, s_localName) == -1)
            {
              return NULL;
            }
        }
    }
  else if (xmlStrEqual (s_uri, BAD_CAST "*"))
    {
      /* matches all namespaces */
      if (xmlStrEqual (s_localName, BAD_CAST "*"))
        {
          format = xmlCharStrdup ("descendant-or-self::*[node-type()=1]");
          if (xmlStrPrintf (expr, 256, format) == -1)
            {
              return NULL;
            }
        }
      else
        {
          format = xmlCharStrdup ("descendant-or-self::*[local-name()='%s']");
          if (xmlStrPrintf (expr, 256, format, s_localName) == -1)
            {
              return NULL;
            }
        }
    }
  else
    {
      if (xmlStrEqual (s_localName, BAD_CAST "*"))
        {
          format = xmlCharStrdup ("descendant-or-self::*[namespace-uri()='%s' and node-type()=1]");
          if (xmlStrPrintf (expr, 256, format, s_uri) == -1)
            {
              return NULL;
            }
        }
      else
        {
          format = xmlCharStrdup ("descendant-or-self::*[namespace-uri()='%s' and local-name()='%s']");
          if (xmlStrPrintf (expr, 256, format, s_uri, s_localName) == -1)
            {
              return NULL;
            }
        }
    }
  node = xmljGetNodeID (env, self);
  if (node == NULL)
    {
      return NULL;
    }
  ctx = xmljCreateXPathContextPtr (env, node);
  if (ctx != NULL)
    {
      eval = xmlXPathEval (expr, ctx);
      xmlXPathFreeContext (ctx);
    }
  return xmljGetXPathNodeList (env, eval);
}

JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathNodeList_free (JNIEnv *env,
                                                  jobject self,
                                                  jobject obj)
{
  xmlXPathFreeObject ((xmlXPathObjectPtr) xmljAsPointer (env, obj));
}

JNIEXPORT jint JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathNodeList_getLength (JNIEnv *env,
                                                       jobject self)
{
  xmlXPathObjectPtr obj;
  
  obj = xmljGetXPathObjectID (env, self);
  if (obj == NULL)
    {
      return 0;
    }
  if (obj->nodesetval == NULL)
    {
      return 0;
    }
  return obj->nodesetval->nodeNr;
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathNodeList_item (JNIEnv *env,
                                                  jobject self,
                                                  jint index)
{
  xmlXPathObjectPtr obj;
  
  obj = xmljGetXPathObjectID (env, self);
  if (obj == NULL)
    {
      return NULL;
    }
  if (obj->nodesetval == NULL)
    {
      return NULL;
    }
  if (obj->nodesetval->nodeNr > 0)
    {
      return xmljGetNodeInstance (env, obj->nodesetval->nodeTab[index]);
    }
  else
    {
      return NULL;
    }
}

