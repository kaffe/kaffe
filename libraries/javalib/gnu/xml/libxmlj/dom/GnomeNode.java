/*
 * GnomeNode.java
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
package gnu.xml.libxmlj.dom;

import java.util.Map;
import java.util.HashMap;

import org.w3c.dom.Document;
import org.w3c.dom.DOMException;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.Text;
import org.w3c.dom.UserDataHandler;

/**
 * A DOM node implemented in libxml2.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class GnomeNode
implements Node
{

  /**
   * Maps document pointers to a map of node pointers to node instances.
   */
  static Map instances;

  /**
   * Retrieves the node instance for the specified node pointer.
   * This creates a new instance and adds it to the cache if required.
   * @param doc the document pointer
   * @param node the node pointer
   * @param type the node type
   */
  static GnomeNode newInstance (final Object doc, final Object node,
                                final int type)
  {
    if (doc == null)
      {
        throw new NullPointerException ("doc");
      }
    if (node == null)
      {
        throw new NullPointerException ("node");
      }
    if (instances == null)
      {
        instances = new HashMap ();
      }
    Map docNodes = (Map) instances.get (doc);
    if (docNodes == null)
      {
        docNodes = new HashMap (1024); // TODO review optimal initial capacity
        instances.put (doc, docNodes);
      }
    GnomeNode nodeInstance = (GnomeNode) docNodes.get (node);
    if (nodeInstance != null)
      {
        return nodeInstance; // Return cached version
      }
    switch (type)
      {
      case ELEMENT_NODE:
        nodeInstance = new GnomeElement (node);
        break;
      case ATTRIBUTE_NODE:
        nodeInstance = new GnomeAttr (node);
        break;
      case TEXT_NODE:
        nodeInstance = new GnomeText (node);
        break;
      case CDATA_SECTION_NODE:
        nodeInstance = new GnomeCDATASection (node);
        break;
      case ENTITY_REFERENCE_NODE:
        nodeInstance = new GnomeEntityReference (node);
        break;
      case ENTITY_NODE:
        nodeInstance = new GnomeEntity (node);
        break;
      case PROCESSING_INSTRUCTION_NODE:
        nodeInstance = new GnomeProcessingInstruction (node);
        break;
      case COMMENT_NODE:
        nodeInstance = new GnomeComment (node);
        break;
      case DOCUMENT_NODE:
        nodeInstance = new GnomeDocument (node);
        break;
      case DOCUMENT_TYPE_NODE:
        nodeInstance = new GnomeDocumentType (node);
        break;
      case DOCUMENT_FRAGMENT_NODE:
        nodeInstance = new GnomeDocumentFragment (node);
        break;
      case NOTATION_NODE:
        nodeInstance = new GnomeNotation (node);
        break;
      default:
        nodeInstance = new GnomeNode (node);
      }
    if (type != DOCUMENT_NODE)
      {
        docNodes.put (node, nodeInstance);
      }
    return nodeInstance;
  }
  
  /**
   * Frees the specified document.
   * This removes all its nodes from the cache.
   */
  static void freeDocument (final Object doc)
  {
    if (instances == null || doc == null)
      {
        return;
      }
    instances.remove (doc);
    //System.out.println("Freed "+instances.remove(doc));
  }
  
  /**
   * xmlNodePtr
   */
  final Object id;

  Map userData;

  GnomeNode (final Object id)
  {
    this.id = id;
  }

  public native String getNodeName ();

  public native String getNodeValue ()
    throws DOMException;

  public native void setNodeValue (String nodeValue)
    throws DOMException;

  public native short getNodeType ();

  public native Node getParentNode ();

  public NodeList getChildNodes ()
  {
    return new GnomeNodeList (id);
  }

  public native Node getFirstChild ();

  public native Node getLastChild ();

  public native Node getPreviousSibling ();

  public native Node getNextSibling ();

  public NamedNodeMap getAttributes ()
  {
    return new GnomeNamedNodeMap (id, 0);
  }

  public native Document getOwnerDocument ();

  public native Node insertBefore (Node newChild, Node refChild)
    throws DOMException;

  public native Node replaceChild (Node newChild, Node oldChild)
    throws DOMException;

  public native Node removeChild (Node oldChild)
    throws DOMException;

  public native Node appendChild (Node newChild)
    throws DOMException;

  public native boolean hasChildNodes ();

  public native Node cloneNode (boolean deep);

  public native void normalize ();

  public boolean isSupported (String feature, String version)
  {
    return getOwnerDocument ().getImplementation ()
      .hasFeature (feature, version);
  }

  public native String getNamespaceURI ();

  public native String getPrefix ();

  public native void setPrefix (String prefix)
    throws DOMException;

  public native String getLocalName ();

  public native boolean hasAttributes ();

  public int hashCode ()
  {
    return id.hashCode ();
  }

  public boolean equals (Object other)
  {
    if (other == this)
      {
        return true;
      }
    return (other instanceof GnomeNode &&
            ((GnomeNode) other).id == id);
  }

  // DOM Level 3 methods

  public native String getBaseURI ();

  public short compareDocumentPosition (Node other)
    throws DOMException
  {
    throw new DOMException (DOMException.NOT_SUPPORTED_ERR,
                            "compareDocumentPosition");
  }
  
  public String getTextContent ()
    throws DOMException
  {
    switch (getNodeType ())
      {
      case ELEMENT_NODE:
      case ATTRIBUTE_NODE:
      case ENTITY_NODE:
      case ENTITY_REFERENCE_NODE:
      case DOCUMENT_FRAGMENT_NODE:
        StringBuffer buffer = new StringBuffer ();
        NodeList children = getChildNodes ();
        int len = children.getLength ();
        for (int i = 0; i < len; i++)
          {
            Node child = children.item (i);
            String textContent = child.getTextContent ();
            if (textContent != null)
              {
                buffer.append (textContent);
              }
          }
        return buffer.toString ();
      case TEXT_NODE:
      case CDATA_SECTION_NODE:
      case COMMENT_NODE:
      case PROCESSING_INSTRUCTION_NODE:
        return getNodeValue ();
      default:
        return null;
      }
  }
  
  public void setTextContent (String textContent)
    throws DOMException
  {
    switch (getNodeType ())
      {
      case ENTITY_REFERENCE_NODE:
        // entity references are read only
        throw new GnomeDOMException (DOMException.NO_MODIFICATION_ALLOWED_ERR,
                                     null);
      case ELEMENT_NODE:
      case ATTRIBUTE_NODE:
      case ENTITY_NODE:
      case DOCUMENT_FRAGMENT_NODE:
        NodeList children = getChildNodes ();
        int len = children.getLength ();
        for (int i = 0; i < len; i++)
          {
            Node child = children.item (i);
            removeChild (child);
          }
        if (textContent != null)
          {
            Text text = getOwnerDocument ().createTextNode (textContent);
            appendChild (text);
          }
        break;
      case TEXT_NODE:
      case CDATA_SECTION_NODE:
      case COMMENT_NODE:
      case PROCESSING_INSTRUCTION_NODE:
        setNodeValue (textContent);
        break;
      }
  }
  
  public boolean isSameNode (Node other)
  {
    return equals (other);
  }
  
  public native String lookupPrefix (String namespaceURI);
  
  public native boolean isDefaultNamespace (String namespaceURI);
  
  public native String lookupNamespaceURI (String prefix);
  
  public native boolean isEqualNode (Node arg);
  
  public Object getFeature (String feature, String version)
  {
    return getOwnerDocument ().getImplementation ()
      .getFeature (feature, version);
  }

  public Object setUserData (String key, Object data, UserDataHandler handler)
  {
    // TODO handler
    if (userData == null)
      {
        userData = new HashMap();
      }
    return userData.put (key, data);
  }

  public Object getUserData (String key)
  {
    if (userData == null)
      {
        return null;
      }
    return userData.get (key);
  }

  public String toString ()
  {
    StringBuffer buffer = new StringBuffer (getClass ().getName ());
    buffer.append ("[nodeName=");
    buffer.append (getNodeName ());
    buffer.append ("]");
    return buffer.toString ();
  }

}
