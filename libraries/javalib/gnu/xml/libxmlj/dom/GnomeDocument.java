/*
 * GnomeDocument.java
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

import org.w3c.dom.Attr;
import org.w3c.dom.CDATASection;
import org.w3c.dom.Comment;
import org.w3c.dom.Document;
import org.w3c.dom.DocumentFragment;
import org.w3c.dom.DocumentType;
import org.w3c.dom.DOMConfiguration;
import org.w3c.dom.DOMException;
import org.w3c.dom.DOMImplementation;
import org.w3c.dom.Element;
import org.w3c.dom.EntityReference;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.ProcessingInstruction;
import org.w3c.dom.Text;

/**
 * A DOM document node implemented in libxml2.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class GnomeDocument
extends GnomeNode
implements Document
{

  DOMImplementation dom;
  
  GnomeDocument(int id)
  {
    super(id);
  }

  protected void finalize()
  {
    free(id);
  }

  private native void free(int id);

  public native DocumentType getDoctype();

  public DOMImplementation getImplementation()
  {
    return dom;
  }

  public native Element getDocumentElement();

  public Element createElement(String tagName)
    throws DOMException
  {
    return createElementNS(null, tagName);
  }

  public native DocumentFragment createDocumentFragment();

  public native Text createTextNode(String data);

  public native Comment createComment(String data);

  public native CDATASection createCDATASection(String data)
    throws DOMException;

  public native ProcessingInstruction createProcessingInstruction(String
      target, String data)
    throws DOMException;

  public Attr createAttribute(String name)
    throws DOMException
  {
    return createAttributeNS(null, name);
  }

  public native EntityReference createEntityReference(String name)
    throws DOMException;

  public NodeList getElementsByTagName(String tagName)
  {
    return new MatchingNodeList(id, null, tagName, false);
  }

  public native Node importNode(Node importedNode, boolean deep)
    throws DOMException;

  public native Element createElementNS(String namespaceURI, String
      qualifiedName)
    throws DOMException;
  
  public native Attr createAttributeNS(String namespaceURI, String
      qualifiedName)
    throws DOMException;

  public NodeList getElementsByTagNameNS(String namespaceURI,
      String localName)
  {
    return new MatchingNodeList(id, namespaceURI, localName, true);
  }

  public native Element getElementById(String elementId);

  // DOM Level 3 methods

  public String getInputEncoding ()
    {
      // TODO
      return null;
    }

  public String getXmlEncoding ()
    {
      // TODO
      return null;
    }

  public boolean getXmlStandalone ()
    {
      // TODO
      return false;
    }

  public void setXmlStandalone (boolean xmlStandalone)
    {
      // TODO
    }

  public String getXmlVersion ()
    {
      // TODO
      return null;
    }

  public void setXmlVersion (String xmlVersion)
    {
      // TODO
    }

  public boolean getStrictErrorChecking ()
    {
      // TODO
      return false;
    }

  public void setStrictErrorChecking (boolean strictErrorChecking)
    {
      // TODO
    }

  public String getDocumentURI ()
    {
      // TODO
      return null;
    }

  public void setDocumentURI (String documentURI)
    {
      // TODO
    }

  public Node adoptNode (Node source)
    {
      // TODO
      return null;
    }

  public DOMConfiguration getDomConfig ()
    {
      // TODO
      return null;
    }

  public void normalizeDocument ()
    {
      // TODO
    }

  public Node renameNode (Node n, String namespaceURI, String qualifiedName)
    {
      // TODO
      return null;
    }

}
