/*
 * GnomeElement.java
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
import org.w3c.dom.DOMException;
import org.w3c.dom.Element;
import org.w3c.dom.NodeList;
import org.w3c.dom.TypeInfo;

/**
 * A DOM element node implemented in libxml2.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class GnomeElement
extends GnomeNode
implements Element
{

  GnomeElement (Object id)
  {
    super (id);
  }
  
  public String getTagName ()
  {
    return getNodeName ();
  }

  public native String getAttribute (String name);

  public native void setAttribute (String name, String value)
    throws DOMException;
  
  public void removeAttribute (String name)
    throws DOMException
  {
    Attr attr = getAttributeNode (name);
    if (attr != null)
      {
        removeAttributeNode (attr);
      }
  }

  public native Attr getAttributeNode (String name);
  
  public native Attr setAttributeNode (Attr newAttr)
    throws DOMException;

  public native Attr removeAttributeNode (Attr oldAttr)
    throws DOMException;

  public native NodeList getElementsByTagName (String name);
  
  public native String getAttributeNS (String namespaceURI, String localName);
  
  public native void setAttributeNS (String namespaceURI, String
                                     qualifiedName, String value)
    throws DOMException;

  public void removeAttributeNS (String namespaceURI, String localName)
    throws DOMException
  {
    Attr attr = getAttributeNodeNS (namespaceURI, localName);
    if (attr != null)
      {
        removeAttributeNode (attr);
      }
  }
  
  public native Attr getAttributeNodeNS (String namespaceURI,
                                         String localName);

  public native Attr setAttributeNodeNS (Attr newAttr)
    throws DOMException;

  public native NodeList getElementsByTagNameNS (String namespaceURI,
                                                 String localName);
  
  public native boolean hasAttribute (String name);

  public native boolean hasAttributeNS (String namespaceURI,
                                        String localName);

  // DOM Level 3 methods

  public TypeInfo getSchemaTypeInfo ()
  {
    return new GnomeTypeInfo (id);
  }

  public void setIdAttribute (String name, boolean isId)
  {
    setIdAttributeNS (null, name, isId);
  }

  public void setIdAttributeNode (Attr isAddr, boolean isId)
  {
    // TODO
  }

  public void setIdAttributeNS (String namespaceURI, String localName,
                                boolean isId)
  {
    // TODO
  }

  public String toString ()
  {
    StringBuffer buffer = new StringBuffer (getClass ().getName ());
    buffer.append ("[tagName=");
    buffer.append (getTagName ());
    buffer.append ("]");
    return buffer.toString ();
  }

}
