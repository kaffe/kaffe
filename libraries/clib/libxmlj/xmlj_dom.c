/*
 * xmlj_dom.c
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
#include "xmlj_dom.h"
#include "xmlj_error.h"
#include "xmlj_io.h"
#include "xmlj_node.h"
#include "xmlj_sax.h"
#include "xmlj_util.h"

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

resolveEntitySAXFunc defaultResolveEntity;
warningSAXFunc defaultWarning;
errorSAXFunc defaultError;
fatalErrorSAXFunc defaultFatalError;

JNIEnv *dom_cb_env;
jobject dom_cb_obj;

/* -- GnomeAttr -- */

JNIEXPORT jboolean JNICALL
Java_gnu_xml_libxmlj_dom_GnomeAttr_getSpecified (JNIEnv * env, jobject self)
{
  xmlAttrPtr attr;

  attr = (xmlAttrPtr) xmljGetNodeID (env, self);
  return (attr->atype != 0);
}

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeAttr_getValue (JNIEnv * env, jobject self)
{
  xmlNodePtr node;
  xmlBufferPtr buf;
  jstring ret;

  node = xmljGetNodeID (env, self);
  buf = xmlBufferCreate ();
  xmlNodeBufGetContent (buf, node);
  ret = xmljNewString (env, buf->content);
  xmlFree (buf);
  return ret;
}

JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_dom_GnomeAttr_setValue (JNIEnv * env,
                                             jobject self, jstring value)
{
  xmlNodePtr node;
  const xmlChar *s_value;

  node = xmljGetNodeID (env, self);
  s_value = xmljGetStringChars (env, value);
  xmlNodeSetContent (node, s_value);
}

/* -- GnomeDocument -- */

JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocument_free (JNIEnv * env,
                                             jobject self, jint id)
{
  xmlDocPtr doc;

  doc = (xmlDocPtr) id;
  xmljFreeDoc (env, doc);
  xmlFree (doc);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocument_getDoctype (JNIEnv * env, jobject self)
{
  xmlDocPtr doc;
  xmlDtdPtr dtd;

  doc = (xmlDocPtr) xmljGetNodeID (env, self);
  dtd = doc->extSubset;
  return xmljGetNodeInstance (env, (xmlNodePtr) dtd);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocument_getDocumentElement (JNIEnv * env,
                                                           jobject self)
{
  xmlDocPtr doc;

  doc = (xmlDocPtr) xmljGetNodeID (env, self);
  return xmljGetNodeInstance (env, xmlDocGetRootElement (doc));
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocument_createDocumentFragment (JNIEnv * env,
                                                               jobject self)
{
  xmlDocPtr doc;

  doc = (xmlDocPtr) xmljGetNodeID (env, self);
  return xmljGetNodeInstance (env, xmlNewDocFragment (doc));
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocument_createTextNode (JNIEnv * env,
                                                       jobject self,
                                                       jstring data)
{
  xmlDocPtr doc;
  xmlNodePtr text;
  const xmlChar *s_data;

  doc = (xmlDocPtr) xmljGetNodeID (env, self);
  s_data = xmljGetStringChars (env, data);
  text = xmlNewDocText (doc, s_data);
  return xmljGetNodeInstance (env, text);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocument_createComment (JNIEnv * env,
                                                      jobject self,
                                                      jstring data)
{
  xmlDocPtr doc;
  xmlNodePtr comment;
  const xmlChar *s_data;

  doc = (xmlDocPtr) xmljGetNodeID (env, self);
  s_data = xmljGetStringChars (env, data);
  comment = xmlNewDocComment (doc, s_data);
  return xmljGetNodeInstance (env, comment);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocument_createCDATASection (JNIEnv * env,
                                                           jobject self,
                                                           jstring data)
{
  xmlDocPtr doc;
  xmlNodePtr cdata;
  const xmlChar *s_data;
  int len;

  doc = (xmlDocPtr) xmljGetNodeID (env, self);
  s_data = xmljGetStringChars (env, data);
  len = xmlStrlen (s_data);
  cdata = xmlNewCDataBlock (doc, s_data, len);
  return xmljGetNodeInstance (env, cdata);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocument_createProcessingInstruction (JNIEnv *
                                                                    env,
                                                                    jobject
                                                                    self,
                                                                    jstring
                                                                    target,
                                                                    jstring
                                                                    data)
{
  xmlDocPtr doc;
  xmlNodePtr pi;
  const xmlChar *s_target;
  const xmlChar *s_data;

  doc = (xmlDocPtr) xmljGetNodeID (env, self);
  s_target = xmljGetStringChars (env, target);
  s_data = xmljGetStringChars (env, data);
  pi = xmlNewPI (s_target, s_data);
  return xmljGetNodeInstance (env, pi);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocument_createEntityReference (JNIEnv * env,
                                                              jobject self,
                                                              jstring name)
{
  xmlDocPtr doc;
  xmlNodePtr ref;
  const xmlChar *s_name;

  doc = (xmlDocPtr) xmljGetNodeID (env, self);
  s_name = xmljGetStringChars (env, name);
  ref = xmlNewReference (doc, s_name);
  return xmljGetNodeInstance (env, ref);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocument_importNode (JNIEnv * env,
                                                   jobject self,
                                                   jobject importedNode,
                                                   jboolean deep)
{
  /* TODO */
  return NULL;
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocument_createElementNS (JNIEnv * env,
                                                        jobject self,
                                                        jstring uri,
                                                        jstring qName)
{
  xmlDocPtr doc;
  xmlNodePtr element;
  xmlNsPtr ns;
  const xmlChar *s_uri;
  const xmlChar *s_qName;
  const xmlChar *s_prefix;
  const xmlChar *s_localName;

  doc = (xmlDocPtr) xmljGetNodeID (env, self);
  s_qName = xmljGetStringChars (env, qName);
  if (uri != NULL)
    {
      s_uri = xmljGetStringChars (env, uri);
      s_prefix = xmljGetPrefix (s_qName);
      s_localName = xmljGetLocalName (s_qName);
      ns = xmlNewNs ((xmlNodePtr) doc, s_uri, s_prefix);
      element = xmlNewDocNode (doc, ns, s_localName, NULL);
    }
  else
    element = xmlNewDocNode (doc, NULL, s_qName, NULL);
  return xmljGetNodeInstance (env, element);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocument_createAttributeNS (JNIEnv * env,
                                                          jobject self,
                                                          jstring uri,
                                                          jstring qName)
{
  xmlDocPtr doc;
  xmlNodePtr attr;
  xmlNsPtr ns;
  const xmlChar *s_uri;
  const xmlChar *s_qName;
  const xmlChar *s_prefix;
  const xmlChar *s_localName;

  doc = (xmlDocPtr) xmljGetNodeID (env, self);
  s_qName = xmljGetStringChars (env, qName);
  if (uri != NULL)
    {
      s_uri = xmljGetStringChars (env, uri);
      s_prefix = xmljGetPrefix (s_qName);
      s_localName = xmljGetLocalName (s_qName);
      ns = xmlNewNs ((xmlNodePtr) doc, s_uri, s_prefix);
      attr =
        (xmlNodePtr) xmlNewNsProp ((xmlNodePtr) doc, ns, s_localName, NULL);
    }
  else
    attr = (xmlNodePtr) xmlNewNsProp ((xmlNodePtr) doc, NULL, s_qName, NULL);
  return xmljGetNodeInstance (env, attr);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocument_getElementById (JNIEnv * env,
                                                       jobject self,
                                                       jstring elementId)
{
  /* TODO */
  return NULL;
}

/* -- GnomeDocumentBuilder -- */

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocumentBuilder_parseStream (JNIEnv * env,
                                                           jobject self,
                                                           jobject in,
                                                           jstring publicId,
                                                           jstring systemId,
                                                           jboolean validate,
                                                           jboolean coalesce,
                                                           jboolean
                                                           expandEntities,
                                                           jboolean
                                                           entityResolver,
                                                           jboolean
                                                           errorHandler)
{
  xmlDocPtr doc;

  doc = xmljParseDocument(env,
                          self,
                          in,
                          publicId,
                          systemId,
                          validate,
                          coalesce,
                          expandEntities,
                          0,
                          0,
                          entityResolver,
                          errorHandler,
                          0,
                          0,
                          0);
  return createDocument (env, self, doc);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocumentBuilder_createDocument (JNIEnv * env,
                                                              jobject self,
                                                              jstring
                                                              namespaceURI,
                                                              jstring
                                                              qualifiedName,
                                                              jobject doctype)
{
  xmlDocPtr doc;
  const xmlChar *version;

  version = xmlCharStrdup ("1.0");
  doc = xmlNewDoc (version);
  /* TODO namespaceURI
   * TODO qualifiedName
   * TODO doctype */
  return createDocument (env, self, doc);
}

/* -- GnomeDocumentType -- */

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocumentType_getEntities (JNIEnv * env,
                                                        jobject self)
{
  xmlDtdPtr dtd;

  dtd = (xmlDtdPtr) xmljGetNodeID (env, self);
  /* TODO dtd->entities */
  return NULL;
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocumentType_getNotations (JNIEnv * env,
                                                         jobject self)
{
  xmlDtdPtr dtd;

  dtd = (xmlDtdPtr) xmljGetNodeID (env, self);
  /* TODO dtd->entities */
  return NULL;
}

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocumentType_getPublicId (JNIEnv * env,
                                                        jobject self)
{
  xmlDtdPtr dtd;

  dtd = (xmlDtdPtr) xmljGetNodeID (env, self);
  return xmljNewString (env, dtd->ExternalID);
}

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocumentType_getSystemId (JNIEnv * env,
                                                        jobject self)
{
  xmlDtdPtr dtd;

  dtd = (xmlDtdPtr) xmljGetNodeID (env, self);
  return xmljNewString (env, dtd->SystemID);
}

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeDocumentType_getInternalSubset (JNIEnv * env,
                                                              jobject self)
{
  /* TODO */
  return NULL;
}

/* -- GnomeElement -- */

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeElement_removeAttributeNode (JNIEnv * env,
                                                           jobject self,
                                                           jobject oldAttr)
{
  /* TODO */
  return NULL;
}

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeElement_getAttributeNS (JNIEnv * env,
                                                      jobject self,
                                                      jstring uri,
                                                      jstring localName)
{
  xmlNodePtr node;
  const xmlChar *s_uri;
  const xmlChar *s_localName;
  const xmlChar *s_value;

  node = xmljGetNodeID (env, self);
  s_localName = xmljGetStringChars (env, localName);
  if (uri == NULL)
    {
      s_value = xmlGetNoNsProp (node, s_localName);
    }
  else
    {
      s_uri = xmljGetStringChars (env, uri);
      s_value = xmlGetNsProp (node, s_localName, s_uri);
    }
  return xmljNewString (env, s_value);
}

JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_dom_GnomeElement_setAttributeNS (JNIEnv * env,
                                                      jobject self,
                                                      jstring uri,
                                                      jstring qName,
                                                      jstring value)
{
  xmlNodePtr node;
  xmlNsPtr ns;
  const xmlChar *s_uri;
  const xmlChar *s_qName;
  const xmlChar *s_prefix;
  const xmlChar *s_localName;
  const xmlChar *s_value;

  node = xmljGetNodeID (env, self);
  s_qName = xmljGetStringChars (env, qName);
  s_value = xmljGetStringChars (env, value);
  if (uri == NULL)
    {
      xmlSetProp (node, s_qName, s_value);
    }
  else
    {
      s_prefix = xmljGetPrefix (s_qName);
      s_localName = xmljGetLocalName (s_qName);
      s_uri = xmljGetStringChars (env, uri);
      ns = xmlNewNs (node, s_uri, s_prefix);
      xmlSetNsProp (node, ns, s_localName, s_value);
    }
}

JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_dom_GnomeElement_removeAttributeNS (JNIEnv * env,
                                                         jobject self,
                                                         jstring uri,
                                                         jstring localName)
{
  /* TODO */
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeElement_getAttributeNodeNS (JNIEnv * env,
                                                          jobject self,
                                                          jstring uri,
                                                          jstring localName)
{
  xmlNodePtr node;
  xmlAttrPtr attr;
  const xmlChar *s_uri;
  const xmlChar *s_localName;

  node = xmljGetNodeID (env, self);
  attr = node->properties;
  s_uri = xmljGetStringChars (env, uri);
  s_localName = xmljGetStringChars (env, localName);
  while (attr != NULL)
    {
      if (uri == NULL)
        {
          if (xmljMatch (s_localName, (xmlNodePtr) attr))
            break;
        }
      else
        {
          if (xmljMatchNS (s_uri, s_localName, (xmlNodePtr) attr))
            break;
        }
      attr = attr->next;
    }
  return xmljGetNodeInstance (env, (xmlNodePtr) attr);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeElement_setAttributeNodeNS (JNIEnv * env,
                                                          jobject self,
                                                          jobject newAttr)
{
  /* TODO */
  return NULL;
}

JNIEXPORT jboolean JNICALL
Java_gnu_xml_libxmlj_dom_GnomeElement_hasAttributeNS (JNIEnv * env,
                                                      jobject self,
                                                      jstring uri,
                                                      jstring localName)
{
  xmlNodePtr node;
  xmlAttrPtr attr;
  const xmlChar *s_uri;
  const xmlChar *s_localName;

  node = xmljGetNodeID (env, self);
  attr = node->properties;
  s_uri = xmljGetStringChars (env, uri);
  s_localName = xmljGetStringChars (env, localName);
  while (attr != NULL)
    {
      if (uri == NULL)
        {
          if (xmljMatch (s_localName, (xmlNodePtr) attr))
            break;
        }
      else
        {
          if (xmljMatchNS (s_uri, s_localName, (xmlNodePtr) attr))
            break;
        }
      attr = attr->next;
    }
  return (attr != NULL);
}

/* -- GnomeEntity -- */

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeEntity_getPublicId (JNIEnv * env, jobject self)
{
  /* TODO */
  return NULL;
}

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeEntity_getSystemId (JNIEnv * env, jobject self)
{
  /* TODO */
  return NULL;
}

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeEntity_getNotationName (JNIEnv * env,
                                                      jobject self)
{
  /* TODO */
  return NULL;
}

/* -- GnomeNamedNodeMap -- */

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNamedNodeMap_getNamedItem (JNIEnv * env,
                                                         jobject self,
                                                         jstring name)
{
  xmlAttrPtr attr;

  attr = getNamedItem (env, self, name);
  return xmljGetNodeInstance (env, (xmlNodePtr) attr);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNamedNodeMap_setNamedItem (JNIEnv * env,
                                                         jobject self,
                                                         jobject arg)
{
  xmlNodePtr node;
  xmlNodePtr argNode;

  node = xmljGetNodeID (env, self);
  argNode = xmljGetNodeID (env, arg);

  if (argNode->doc != node->doc)
    xmljThrowDOMException (env, 4, NULL);	/* WRONG_DOCUMENT_ERR */
  if (argNode->parent != NULL)
    xmljThrowDOMException (env, 10, NULL);	/* INUSE_ATTRIBUTE_ERR */
  if (!xmlAddChild (node, argNode))
    xmljThrowDOMException (env, 3, NULL);	/* HIERARCHY_REQUEST_ERR */
  return arg;
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNamedNodeMap_removeNamedItem (JNIEnv * env,
                                                            jobject self,
                                                            jstring name)
{
  xmlAttrPtr attr;

  attr = getNamedItem (env, self, name);
  if (attr == NULL)
    {
      xmljThrowDOMException (env, 8, NULL);	/* NOT_FOUND_ERR */
      return NULL;
    }
  else
    {
      xmlUnlinkNode ((xmlNodePtr) attr);
      return xmljGetNodeInstance (env, (xmlNodePtr) attr);
    }
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNamedNodeMap_item (JNIEnv * env,
                                                 jobject self, jint index)
{
  xmlNodePtr node;
  xmlAttrPtr attr;
  jint count;

  node = xmljGetNodeID (env, self);
  attr = node->properties;
  for (count = 0; attr != NULL && count < index; count++)
    attr = attr->next;
  if (attr == NULL)
    printf ("No attribute at index %d\n", index);
  return xmljGetNodeInstance (env, (xmlNodePtr) attr);
}

JNIEXPORT jint JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNamedNodeMap_getLength (JNIEnv * env,
                                                      jobject self)
{
  xmlNodePtr node;
  xmlAttrPtr attr;
  jint count;

  node = xmljGetNodeID (env, self);
  count = 0;
  attr = node->properties;
  while (attr != NULL)
    {
      count++;
      attr = attr->next;
    }
  return count;
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNamedNodeMap_getNamedItemNS (JNIEnv * env,
                                                           jobject self,
                                                           jstring uri,
                                                           jstring localName)
{
  xmlAttrPtr attr;

  attr = getNamedItemNS (env, self, uri, localName);
  return xmljGetNodeInstance (env, (xmlNodePtr) attr);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNamedNodeMap_setNamedItemNS (JNIEnv * env,
                                                           jobject self,
                                                           jobject arg)
{
  return Java_gnu_xml_libxmlj_dom_GnomeNamedNodeMap_setNamedItem (env, self,
                                                                  arg);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNamedNodeMap_removeNamedItemNS (JNIEnv * env,
                                                              jobject self,
                                                              jstring uri,
                                                              jstring
                                                              localName)
{
  xmlAttrPtr attr;

  attr = getNamedItemNS (env, self, uri, localName);
  if (attr == NULL)
    {
      xmljThrowDOMException (env, 8, NULL);	/* NOT_FOUND_ERR */
      return NULL;
    }
  else
    {
      xmlUnlinkNode ((xmlNodePtr) attr);
      return xmljGetNodeInstance (env, (xmlNodePtr) attr);
    }
}

/* -- GnomeNode -- */

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_getNodeName (JNIEnv * env, jobject self)
{
  xmlNodePtr node;

  node = xmljGetNodeID (env, self);
  return xmljNewString (env, node->name);
}

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_getNodeValue (JNIEnv * env, jobject self)
{
  xmlNodePtr node;
  xmlBufferPtr buf;
  jstring ret;


  node = xmljGetNodeID (env, self);

  /* If not character data, return null */
  if (node->type != XML_TEXT_NODE &&
      node->type != XML_CDATA_SECTION_NODE && node->type != XML_COMMENT_NODE)
    return NULL;

  buf = xmlBufferCreate ();
  xmlNodeBufGetContent (buf, node);
  ret = xmljNewString (env, buf->content);
  xmlFree (buf);
  return ret;
}

JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_setNodeValue (JNIEnv * env,
                                                 jobject self,
                                                 jstring nodeValue)
{
  xmlNodePtr node;
  const xmlChar *s_nodeValue;

  node = xmljGetNodeID (env, self);

  /* If not character data, return */
  if (node->type != XML_TEXT_NODE &&
      node->type != XML_CDATA_SECTION_NODE && node->type != XML_COMMENT_NODE)
    return;

  s_nodeValue = xmljGetStringChars (env, nodeValue);
  xmlNodeSetContent (node, s_nodeValue);
}

JNIEXPORT jshort JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_getNodeType (JNIEnv * env, jobject self)
{
  xmlNodePtr node;

  node = xmljGetNodeID (env, self);
  return node->type;
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_getParentNode (JNIEnv * env, jobject self)
{
  xmlNodePtr node;

  node = xmljGetNodeID (env, self);
  return xmljGetNodeInstance (env, node->parent);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_getFirstChild (JNIEnv * env, jobject self)
{
  xmlNodePtr node;

  node = xmljGetNodeID (env, self);
  return xmljGetNodeInstance (env, node->children);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_getLastChild (JNIEnv * env, jobject self)
{
  xmlNodePtr node;

  node = xmljGetNodeID (env, self);
  return xmljGetNodeInstance (env, node->last);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_getPreviousSibling (JNIEnv * env,
                                                       jobject self)
{
  xmlNodePtr node;

  node = xmljGetNodeID (env, self);
  return xmljGetNodeInstance (env, node->prev);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_getNextSibling (JNIEnv * env, jobject self)
{
  xmlNodePtr node;

  node = xmljGetNodeID (env, self);
  return xmljGetNodeInstance (env, node->next);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_getAttributes (JNIEnv * env, jobject self)
{
  xmlNodePtr node;
  jclass cls;
  jmethodID method;

  node = xmljGetNodeID (env, self);

  /* Construct named node map object */
  cls = (*env)->FindClass (env, "gnu/xml/libxmlj/dom/GnomeNamedNodeMap");
  method = (*env)->GetMethodID (env, cls, "<init>", "(I)V");
  return (*env)->NewObject (env, cls, method, node);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_getOwnerDocument (JNIEnv * env,
                                                     jobject self)
{
  xmlNodePtr node;

  node = xmljGetNodeID (env, self);
  return xmljGetNodeInstance (env, (xmlNodePtr) node->doc);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_insertBefore (JNIEnv * env,
                                                 jobject self,
                                                 jobject newChild,
                                                 jobject refChild)
{
  xmlNodePtr newChildNode;
  xmlNodePtr refChildNode;

  newChildNode = xmljGetNodeID (env, newChild);
  refChildNode = xmljGetNodeID (env, refChild);

  if (newChildNode->doc != refChildNode->doc)
    xmljThrowDOMException (env, 4, NULL);	/* WRONG_DOCUMENT_ERR */
  if (!xmlAddPrevSibling (refChildNode, newChildNode))
    xmljThrowDOMException (env, 3, NULL);	/* HIERARCHY_REQUEST_ERR */
  return newChild;
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_replaceChild (JNIEnv * env,
                                                 jobject self,
                                                 jobject newChild,
                                                 jobject oldChild)
{
  xmlNodePtr newChildNode;
  xmlNodePtr oldChildNode;

  newChildNode = xmljGetNodeID (env, newChild);
  oldChildNode = xmljGetNodeID (env, oldChild);

  if (newChildNode->doc != oldChildNode->doc)
    xmljThrowDOMException (env, 4, NULL);	/* WRONG_DOCUMENT_ERR */
  if (!xmlReplaceNode (oldChildNode, newChildNode))
    xmljThrowDOMException (env, 3, NULL);	/* HIERARCHY_REQUEST_ERR */
  return newChild;
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_removeChild (JNIEnv * env,
                                                jobject self,
                                                jobject oldChild)
{
  xmlNodePtr node;
  xmlNodePtr oldChildNode;

  node = xmljGetNodeID (env, self);
  oldChildNode = xmljGetNodeID (env, oldChild);

  if (oldChildNode->parent != node)
    xmljThrowDOMException (env, 8, NULL);	/* NOT_FOUND_ERR */
  xmlUnlinkNode (oldChildNode);
  return oldChild;
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_appendChild (JNIEnv * env,
                                                jobject self,
                                                jobject newChild)
{
  xmlNodePtr node;
  xmlNodePtr newChildNode;

  node = xmljGetNodeID (env, self);
  newChildNode = xmljGetNodeID (env, newChild);

  if (newChildNode->doc != node->doc)
    xmljThrowDOMException (env, 4, NULL);	/* WRONG_DOCUMENT_ERR */
  if (!xmlAddChild (node, newChildNode))
    xmljThrowDOMException (env, 3, NULL);	/* HIERARCHY_REQUEST_ERR */
  return newChild;
}

JNIEXPORT jboolean JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_hasChildNodes (JNIEnv * env, jobject self)
{
  xmlNodePtr node;

  node = xmljGetNodeID (env, self);
  return (node->children != NULL);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_cloneNode (JNIEnv * env,
                                              jobject self, jboolean deep)
{
  /* TODO */
  return NULL;
}

JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_normalize (JNIEnv * env, jobject self)
{
  /* TODO */
}

JNIEXPORT jboolean JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_isSupported (JNIEnv * env,
                                                jobject self,
                                                jstring feature,
                                                jstring version)
{
  /* TODO */
  return 0;
}

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_getNamespaceURI (JNIEnv * env,
                                                    jobject self)
{
  xmlNodePtr node;

  node = xmljGetNodeID (env, self);
  if (node->ns == NULL)
    return NULL;
  else
    return xmljNewString (env, node->ns->href);
}

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_getPrefix (JNIEnv * env, jobject self)
{
  xmlNodePtr node;

  node = xmljGetNodeID (env, self);
  if (node->ns == NULL)
    return NULL;
  else
    return xmljNewString (env, node->ns->prefix);
}

JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_setPrefix (JNIEnv * env,
                                              jobject self, jstring prefix)
{
  xmlNodePtr node;
  const xmlChar *s_prefix;

  s_prefix = xmljGetStringChars (env, prefix);
  if (xmlValidateName (s_prefix, 0))
    xmljThrowDOMException (env, 5, NULL);	/* INVALID_CHARACTER_ERR */
  node = xmljGetNodeID (env, self);
  if (node->ns == NULL)
    xmljThrowDOMException (env, 14, NULL);	/* NAMESPACE_ERR */
  node->ns->prefix = s_prefix;
}

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_getLocalName (JNIEnv * env, jobject self)
{
  xmlNodePtr node;

  node = xmljGetNodeID (env, self);
  if (node->name == NULL)
    return NULL;
  else
    return xmljNewString (env, node->name);
}

JNIEXPORT jboolean JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNode_hasAttributes (JNIEnv * env, jobject self)
{
  xmlNodePtr node;

  node = xmljGetNodeID (env, self);
  return (node->properties != NULL);
}

/* -- GnomeNodeList -- */

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNodeList_item (JNIEnv * env,
                                             jobject self, jint index)
{
  xmlNodePtr node;
  jint count;

  node = xmljGetNodeID (env, self);
  node = node->children;
  count = 0;
  for (count = 0; node != NULL && count < index; count++)
    node = node->next;
  return xmljGetNodeInstance (env, node);
}

JNIEXPORT jint JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNodeList_getLength (JNIEnv * env, jobject self)
{
  xmlNodePtr node;
  jint count;

  node = xmljGetNodeID (env, self);
  count = 0;
  node = node->children;
  while (node != NULL)
    {
      count++;
      node = node->next;
    }
  return count;
}

/* -- GnomeNotation -- */

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNotation_getPublicId (JNIEnv * env,
                                                    jobject self)
{
  xmlNotationPtr notation;

  notation = (xmlNotationPtr) xmljGetNodeID (env, self);
  if (notation->PublicID == NULL)
    return NULL;
  else
    return xmljNewString (env, notation->PublicID);
}

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeNotation_getSystemId (JNIEnv * env,
                                                    jobject self)
{
  xmlNotationPtr notation;

  notation = (xmlNotationPtr) xmljGetNodeID (env, self);
  if (notation->SystemID == NULL)
    return NULL;
  else
    return xmljNewString (env, notation->SystemID);
}

/* -- GnomeProcessingInstruction -- */

JNIEXPORT jstring JNICALL
Java_gnu_xml_libxmlj_dom_GnomeProcessingInstruction_getData (JNIEnv * env,
                                                             jobject self)
{
  xmlNodePtr node;
  xmlBufferPtr buf;
  jstring ret;

  node = xmljGetNodeID (env, self);
  buf = xmlBufferCreate ();
  xmlNodeBufGetContent (buf, node);
  ret = xmljNewString (env, buf->content);
  xmlFree (buf);
  return ret;
}

JNIEXPORT void JNICALL
Java_gnu_xml_libxmlj_dom_GnomeProcessingInstruction_setData (JNIEnv * env,
                                                             jobject self,
                                                             jstring data)
{
  xmlNodePtr node;
  const xmlChar *s_data;

  node = xmljGetNodeID (env, self);
  s_data = xmljGetStringChars (env, data);
  xmlNodeSetContent (node, s_data);
}

JNIEXPORT jobject JNICALL
Java_gnu_xml_libxmlj_dom_MatchingNodeList_item (JNIEnv * env,
                                                jobject self,
                                                jint index,
                                                jstring uri,
                                                jstring name, jboolean ns)
{
  xmlNodePtr node;
  const xmlChar *s_uri;
  const xmlChar *s_name;
  jint count;

  /* Get parent node */
  node = xmljGetNodeID (env, self);

  /* Get search criteria */
  s_name = xmljGetStringChars (env, name);
  if (ns)
    s_uri = xmljGetStringChars (env, uri);
  else
    s_uri = NULL;

  node = node->children;
  count = 0;
  while (node != NULL && count < index)
    {
      if (ns)
        {
          while (node != NULL && xmljMatchNS (s_uri, s_name, node))
            node = node->next;
        }
      else
        {
          while (node != NULL && xmljMatch (s_name, node))
            node = node->next;
        }
      count++;
    }
  return xmljGetNodeInstance (env, node);
}

JNIEXPORT jint JNICALL
Java_gnu_xml_libxmlj_dom_MatchingNodeList_getLength (JNIEnv * env,
                                                     jobject self,
                                                     jstring uri,
                                                     jstring name,
                                                     jboolean ns)
{
  xmlNodePtr node;
  const xmlChar *s_uri;
  const xmlChar *s_name;
  jint count;

  /* Get parent node */
  node = xmljGetNodeID (env, self);
  /* Get search criteria */
  s_name = xmljGetStringChars (env, name);
  if (ns)
    s_uri = xmljGetStringChars (env, uri);
  else
    s_uri = NULL;

  count = 0;
  node = node->children;
  while (node != NULL)
    {
      if (ns)
        {
          while (node != NULL && xmljMatchNS (s_uri, s_name, node))
            node = node->next;
        }
      else
        {
          while (node != NULL && xmljMatch (s_name, node))
            node = node->next;
        }
      count++;
    }
  return count;
}

/* -- Utility -- */

/*
 * Create GnomeDocument object from the given xmlDocPtr
 */
jobject
createDocument (JNIEnv * env, jobject self, xmlDocPtr doc)
{
  jclass cls;
  jfieldID field;
  jobject ret;

  if (doc == NULL)
    return NULL;

  /* Get document object */
  ret = xmljGetNodeInstance (env, (xmlNodePtr) doc);

  /* Set DOM implementation field */
  cls = (*env)->FindClass (env, "gnu/xml/libxmlj/dom/GnomeDocument");
  field = (*env)->GetFieldID (env, cls, "dom",
                              "Lorg/w3c/dom/DOMImplementation;");
  (*env)->SetObjectField (env, ret, field, self);
  return ret;
}

/*
 * Get parser context from GnomeDocumentBuilder field
 */
xmlParserCtxtPtr
getContext (JNIEnv * env, jobject self)
{
  xmlParserCtxtPtr ret;
  jclass cls;
  jfieldID field;

  cls = (*env)->GetObjectClass (env, self);
  field = (*env)->GetFieldID (env, cls, "context", "I");
  ret = (xmlParserCtxtPtr) (*env)->GetIntField (env, self, field);
  return ret;
}

/* -- Callback functions -- */

xmlParserInputPtr
xmljDOMResolveEntity (void *ctx,
                      const xmlChar * publicId, const xmlChar * systemId)
{
  /* TODO */
  return defaultResolveEntity (ctx, publicId, systemId);
}

void
xmljDOMWarning (void *ctx, const char *msg, ...)
{
  va_list args;

  va_start (args, msg);
  dispatchLogCallback (msg, "warning");
  va_end (args);
}

void
xmljDOMError (void *ctx, const char *msg, ...)
{
  va_list args;

  va_start (args, msg);
  dispatchLogCallback (msg, "error");
  va_end (args);
}

void
xmljDOMFatalError (void *ctx, const char *msg, ...)
{
  va_list args;

  va_start (args, msg);
  dispatchLogCallback (msg, "fatalError");
  va_end (args);
}

void
dispatchLogCallback (const char *msg, const char *methodName)
{
  xmlParserCtxtPtr context;
  const xmlChar *errorMsg;
  const xmlChar *systemId;

  jclass cls;
  jmethodID method;

  jstring j_msg;
  jstring j_publicId;
  jstring j_systemId;
  jint lineNumber;
  jint columnNumber;

  context = getContext (dom_cb_env, dom_cb_obj);

  /* Get the logging method to invoke */
  cls = (*dom_cb_env)->GetObjectClass (dom_cb_env, dom_cb_obj);
  method = (*dom_cb_env)->GetMethodID (dom_cb_env, cls,
                                       methodName,
                                       "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;II)V");
  if (method == NULL)
    return;

  /* Prepare arguments */
  if (context->lastError.message != NULL)
    errorMsg = xmlCharStrdup (context->lastError.message);
  else
    errorMsg = (msg == NULL) ? NULL : xmlCharStrdup (msg);
  systemId = (context->lastError.file == NULL) ? NULL :
    xmlCharStrdup (context->lastError.file);

  j_msg = (errorMsg == NULL) ? NULL : xmljNewString (dom_cb_env, errorMsg);
  j_publicId = NULL;
  j_systemId = (systemId == NULL) ? NULL :
    xmljNewString (dom_cb_env, systemId);
  lineNumber = (jint) context->lastError.line;
  columnNumber = -1;

  /* Invoke the method */
  (*dom_cb_env)->CallVoidMethod (dom_cb_env, dom_cb_obj, method,
                                 j_msg, j_publicId, j_systemId,
                                 lineNumber, columnNumber);
}

xmlAttrPtr
getNamedItem (JNIEnv * env, jobject self, jstring name)
{
  xmlNodePtr node;
  xmlAttrPtr attr;
  const xmlChar *s_name;

  s_name = xmljGetStringChars (env, name);

  node = xmljGetNodeID (env, self);
  attr = node->properties;
  while (attr != NULL)
    {
      if (xmljMatch (s_name, (xmlNodePtr) attr))
        break;
      attr = attr->next;
    }

  return attr;
}

xmlAttrPtr
getNamedItemNS (JNIEnv * env, jobject self, jstring uri, jstring localName)
{
  xmlNodePtr node;
  xmlAttrPtr attr;
  const xmlChar *s_uri;
  const xmlChar *s_localName;

  s_uri = xmljGetStringChars (env, uri);
  s_localName = xmljGetStringChars (env, localName);

  node = xmljGetNodeID (env, self);
  attr = node->properties;
  while (attr != NULL)
    {
      if (xmljMatchNS (s_uri, s_localName, (xmlNodePtr) attr))
        break;
      attr = attr->next;
    }

  return attr;
}
