#include "gnu_xml_libxmlj_dom_GnomeDocument.h"
#include "gnu_xml_libxmlj_dom_GnomeXPathExpression.h"
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
 * Converts an xmlXPathObjectPtr to a Java object.
 */
jobject
xmljGetXPathObject (JNIEnv *env, xmlXPathObjectPtr obj)
{
  jclass cls;
  jmethodID method;
  jobject ret;
      
  cls = (*env)->FindClass (env, "gnu/xml/libxmlj/dom/GnomeXPathResult");
  method = (*env)->GetMethodID (env, cls, "<init>", "(J)V");
  ret = (*env)->NewObject (env, cls, method, obj);
  
  return ret;
}

xmlXPathObjectPtr
xmljGetXPathObjectID (JNIEnv *env, jobject obj)
{
  jclass cls;
  jfieldID field;

  cls = (*env)->GetObjectClass (env, obj);
  field = (*env)->GetFieldID (env, cls, "obj", "J");
  return (xmlXPathObjectPtr) (*env)->GetLongField (env, obj, field);
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
  xmlXPathObjectPtr eval;
  jobject ret;
  
  str = xmljGetStringChars (env, expression);
  node = xmljGetNodeID (env, contextNode);
  ctx = xmljCreateXPathContextPtr (env, node);
  eval = xmlXPathEval (str, ctx);
  ret = (eval == NULL) ? NULL : xmljGetXPathObject (env, eval);
  xmlXPathFreeContext (ctx);
  return ret;
}

JNIEXPORT jlong JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathExpression_init (JNIEnv *env,
                                                    jobject self,
                                                    jstring expression)
{
  const xmlChar *str;

  str = xmljGetStringChars (env, expression);
  return (jlong) xmlXPathCompile (str);
}

JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathExpression_free (JNIEnv *env,
                                                    jobject self,
                                                    jlong ptr)
{
  xmlXPathCompExprPtr expr;

  expr = (xmlXPathCompExprPtr) ptr;
  xmlXPathFreeCompExpr (expr);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathExpression_evaluate (JNIEnv *env,
                                                        jobject self,
                                                        jlong ptr,
                                                        jobject contextNode,
                                                        jshort type,
                                                        jobject result)
{
  xmlXPathCompExprPtr expr;
  xmlNodePtr node;
  xmlXPathContextPtr ctx;
  xmlXPathObjectPtr eval;
  jobject ret;

  expr = (xmlXPathCompExprPtr) ptr;
  node = xmljGetNodeID (env, contextNode);
  ctx = xmljCreateXPathContextPtr (env, node);
  eval = xmlXPathCompiledEval (expr, ctx);
  ret = (eval == NULL) ? NULL : xmljGetXPathObject (env, eval);
  xmlXPathFreeContext (ctx);
  return ret;
}

JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathResult_free (JNIEnv *env,
                                                jobject self,
                                                jlong obj)
{
  xmlXPathFreeObject ((xmlXPathObjectPtr) obj);
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
      return -1; /* TODO */
    }
}

JNIEXPORT jdouble JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathResult_getNumberValue (JNIEnv *env,
                                                          jobject self)
{
  xmlXPathObjectPtr obj;
  
  obj = xmljGetXPathObjectID (env, self);
  return obj->floatval;
}

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeXPathResult_getStringValue (JNIEnv *env,
                                                          jobject self)
{
  xmlXPathObjectPtr obj;
  
  obj = xmljGetXPathObjectID (env, self);
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
  if (obj->nodesetval->nodeNr > 0)
    {
      return xmljGetNodeInstance (env, obj->nodesetval->nodeTab[index]);
    }
  else
    {
      return NULL;
    }
}

