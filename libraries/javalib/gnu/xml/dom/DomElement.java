/*
 * Copyright (C) 1999-2001 David Brownell
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License. 
 */

package gnu.xml.dom;

import org.w3c.dom.*;


/**
 * <p> "Element" implementation.
 *
 * @author David Brownell 
 */
public class DomElement extends DomNsNode implements Element
{
    // Attributes are VERY expensive in DOM, and not just for
    // this implementation.  Avoid creating them.
    private DomNamedNodeMap	attributes;


    /**
     * Constructs an Element node associated with the specified document.
     *
     * <p>This constructor should only be invoked by a Document as part
     * of its createElement functionality, or through a subclass which is
     * similarly used in a "Sub-DOM" style layer.
     *
     * @param owner The document with which this node is associated
     * @param namespaceURI Combined with the local part of the name,
     *	this is used to uniquely identify a type of element
     * @param name Name of this element, which may include a prefix
     */
    protected DomElement (Document owner, String namespaceURI, String name)
    {
	super (owner, namespaceURI, name);
    }


    /**
     * <b>DOM L1</b>
     * Returns the element's attributes
     */
    public NamedNodeMap getAttributes ()
    {
	if (attributes == null)
	    attributes = new DomNamedNodeMap (getOwnerDocument (), this);
	return attributes;
    }


    /**
     * <b>DOM L2></b>
     * Returns true iff this is an element node with attributes.
     */
    public boolean hasAttributes ()
	{ return attributes != null && attributes.getLength () != 0; }


    /**
     * Shallow clone of the element, except that associated
     * attributes are (deep) cloned.
     */
    public Object clone ()
    {
	DomElement	retval = (DomElement) super.clone ();
	DomNamedNodeMap	atts;

	if (attributes == null)
	    return retval;

	atts = new DomNamedNodeMap (getOwnerDocument (), retval);
	retval.attributes = atts;
	for (int i = 0; i < attributes.getLength (); i++) {
	    Node	temp = attributes.item (i);

	    temp = temp.cloneNode (true);
	    atts.setNamedItem (temp);
	}
	return retval;
    }


    /**
     * Marks this element, its children, and its associated attributes as
     * readonly.
     */
    public void makeReadonly ()
    {
	super.makeReadonly ();
	if (attributes != null)
	    attributes.makeReadonly ();
    }

    /**
     * <b>DOM L1</b>
     * Returns the element name (same as getNodeName).
     */
    final public String getTagName ()
    {
	return getNodeName ();
    }

    /**
     * <b>DOM L1</b>
     * Returns the constant ELEMENT_NODE.
     */
    final public short getNodeType ()
	{ return ELEMENT_NODE; }


    /**
     * <b>DOM L1</b>
     * Returns the value of the specified attribute, or an
     * empty string.
     */
    public String getAttribute (String name)
    {
	Attr attr = getAttributeNode (name);

	if (attr == null)
	    return "";
	else
	    return attr.getValue ();
    }


    /**
     * <b>DOM L2</b>
     * Returns true if the element has an attribute with the
     * specified name (specified or DTD defaulted).
     */
    public boolean hasAttribute (String name)
    {
	return getAttributeNode (name) != null;
    }


    /**
     * <b>DOM L2</b>
     * Returns true if the element has an attribute with the
     * specified name (specified or DTD defaulted).
     */
    public boolean hasAttributeNS (String namespaceURI, String local)
    {
	return getAttributeNodeNS (namespaceURI, local) != null;
    }


    /**
     * <b>DOM L2</b>
     * Returns the value of the specified attribute, or an
     * empty string.
     */
    public String getAttributeNS (String namespaceURI, String local)
    {
	Attr attr = getAttributeNodeNS (namespaceURI, local);

	if (attr == null)
	    return "";
	else
	    return attr.getValue ();
    }


    /**
     * <b>DOM L1</b>
     * Returns the appropriate attribute node; the name is the
     * nodeName property of the attribute.
     */
    public Attr getAttributeNode (String name)
    {
	if (attributes == null)
	    return null;

	return (Attr) attributes.getNamedItem (name);
    }


    /**
     * <b>DOM L2</b>
     * Returns the appropriate attribute node; the name combines
     * the namespace name and the local part.
     */
    public Attr getAttributeNodeNS (String namespace, String localPart)
    {
	if (attributes == null)
	    return null;

	return (Attr) attributes.getNamedItemNS (namespace, localPart);
    }


    /**
     * <b>DOM L1</b>
     * Modifies an existing attribute to have the specified value,
     * or creates a new one with that value.  The name used is the
     * nodeName value. 
     */
    public void setAttribute (String name, String value)
    {
	Attr attr = getAttributeNode (name);

	if (attr != null) {
	    attr.setNodeValue (value);
	    return;
	}
	attr = getOwnerDocument ().createAttribute (name);
	attr.setNodeValue (value);
	setAttributeNode (attr);
    }


    /**
     * <b>DOM L2</b>
     * Modifies an existing attribute to have the specified value,
     * or creates a new one with that value.
     */
    public void setAttributeNS (String uri, String aname, String value)
    {
	if (("xmlns".equals (aname) || aname.startsWith ("xmlns:"))
		&& !DomDocument.xmlnsURI.equals (uri))
	    throw new DomEx (DomEx.NAMESPACE_ERR,
		"setting xmlns attribute to illegal value", this, 0);

	Attr attr = getAttributeNodeNS (uri, aname);

	if (attr != null) {
	    attr.setNodeValue (value);
	    return;
	}
	attr = getOwnerDocument ().createAttributeNS (uri, aname);
	attr.setNodeValue (value);
	setAttributeNodeNS (attr);
    }


    /**
     * <b>DOM L1</b>
     * Stores the specified attribute, optionally overwriting any
     * existing one with that name.
     */
    public Attr setAttributeNode (Attr attr)
    {
	return (Attr) getAttributes ().setNamedItem (attr);
    }

    /**
     * <b>DOM L2</b>
     * Stores the specified attribute, optionally overwriting any
     * existing one with that name.
     */
    public Attr setAttributeNodeNS (Attr attr)
    {
	return (Attr) getAttributes ().setNamedItemNS (attr);
    }

    /**
     * <b>DOM L1</b>
     * Removes the appropriate attribute node.
     * If there is no such node, this is (bizarrely enough) a NOP so you
     * won't see exceptions if your code deletes non-existent attributes.
     *
     * <p>Note that since there is no portable way for DOM to record
     * DTD information, default values for attributes will never be
     * provided automatically.
     */
    public void removeAttribute (String name)
    {
	if (attributes == null)
	    return;
	    // throw new DomEx (DomEx.NOT_FOUND_ERR, name, null, 0);

	try {
	    attributes.removeNamedItem (name);
	} catch (DomEx e) {
	    if (e.code == DomEx.NOT_FOUND_ERR)
		return;
	}
    }


    /**
     * <b>DOM L1</b>
     * Removes the appropriate attribute node; the name is the
     * nodeName property of the attribute.
     *
     * <p>Note that since there is no portable way for DOM to record
     * DTD information, default values for attributes will never be
     * provided automatically.
     */
    public Attr removeAttributeNode (Attr node)
    {
	if (attributes == null)
	    throw new DomEx (DomEx.NOT_FOUND_ERR, null, node, 0);

	return (Attr) attributes.removeNamedItem (node.getNodeName ());
    }


    /**
     * <b>DOM L2</b>
     * Removes the appropriate attribute node; the name combines
     * the namespace name and the local part.
     *
     * <p>Note that since there is no portable way for DOM to record
     * DTD information, default values for attributes will never be
     * provided automatically.
     */
    public void removeAttributeNS (String namespace, String localPart)
    {
	if (attributes == null)
	    throw new DomEx (DomEx.NOT_FOUND_ERR, localPart, null, 0);

	attributes.removeNamedItemNS (namespace, localPart);
    }
}
