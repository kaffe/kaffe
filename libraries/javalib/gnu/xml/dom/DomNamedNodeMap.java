/*
 * DomNamedNodeMap.java
 * Copyright (C) 1999,2000,2001 The Free Software Foundation
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
 * obliged to do so.  If you do not wish to do so, delete this
 * exception statement from your version. 
 */

package gnu.xml.dom;

import java.util.Vector;

import org.w3c.dom.*;


/**
 * <p> "NamedNodeMap" implementation. </p>
 * Used mostly to hold element attributes, but sometimes also
 * to list notations or entities.
 *
 * @author David Brownell 
 */
public class DomNamedNodeMap implements NamedNodeMap
{
    private final Document		owner;

    private DomNode			contents [] = new DomNode [1];
    private int				length;
    private boolean			readonly;
    private final Element		element;

    private static final int		DELTA = 5;


    /**
     * Constructs an empty map associated with the specified document.
     */
    public DomNamedNodeMap (Document owner)
    {
	this.owner = owner;
	this.element = null;
    }

    // package private
    DomNamedNodeMap (Document owner, Element element)
    {
	this.owner = owner;
	this.element = element;
    }

    /**
     * Reduces space utilization for this object.
     */
    public void compact ()
    {
	if (contents.length != length)
	    setCapacity (length);
    }

    private void setCapacity (int len)
    {
	DomNode		newContents [] = new DomNode [len];

	System.arraycopy (contents, 0, newContents, 0, length);
	contents = newContents;
    }

    /**
     * Exposes the internal "readonly" flag.  In DOM, all NamedNodeMap
     * objects found in a DocumentType object are read-only (after
     * they are fully constructed), and those holding attributes of
     * a readonly element will also be readonly.
     */
    final public boolean isReadonly ()
    {
	return readonly;
    }

    
    /**
     * Sets the internal "readonly" flag so the node and its
     * children can't be changed.
     */
    public void makeReadonly ()
    {
	readonly = true;
	for (int i = 0; i < length; i++)
	    contents [i].makeReadonly ();
    }


    /**
     * <b>DOM L1</b>
     * Returns the named item from the map, or null; names are just
     * the nodeName property.
     */
    public Node getNamedItem (String name)
    {
	for (int i = 0; i < length; i++) {
	    if (contents [i].getNodeName ().equals (name))
		return contents [i];
	}
	return null;
    }


    /**
     * <b>DOM L2</b>
     * Returns the named item from the map, or null; names are the
     * localName and namespaceURI properties, ignoring any prefix.
     */
    public Node getNamedItemNS (String namespaceURI, String localName)
    {
	for (int i = 0; i < length; i++) {
	    DomNode	temp = contents [i];
	    String	tempName = temp.getLocalName ();
	    String	ns;

	    if (tempName != null && tempName.equals (localName)) {
		ns = temp.getNamespaceURI ();
		if ((ns == null && namespaceURI == null)
			|| ns.equals (namespaceURI)) {
		    return temp;
		}
	    }
	}
	return null;
    }


    private void checkAttr (Attr arg)
    {
	if (element == null)
	    return;

	Element	argOwner = arg.getOwnerElement ();

	if (argOwner != null) {
	    if (argOwner != element)
		throw new DomEx (DomEx.INUSE_ATTRIBUTE_ERR);
	    return;
	}

	// We can't escape implementation dependencies here; we let
	// the Java runtime deal with error reporting
	((DomAttr)arg).setOwnerElement (element);
    }


    /**
     * <b>DOM L1</b>
     * Stores the named item into the map, optionally overwriting
     * any existing node with that name.  The name used is just
     * the nodeName attribute.
     */
    public Node setNamedItem (Node arg)
    {
	if (readonly)
	    throw new DomEx (DomEx.NO_MODIFICATION_ALLOWED_ERR);
	if (arg.getOwnerDocument () != owner)
	    throw new DomEx (DomEx.WRONG_DOCUMENT_ERR);
	if (arg instanceof Attr)
	    checkAttr ((Attr) arg);

	String	name = arg.getNodeName ();

// maybe attribute ADDITION events (?)

	for (int i = 0; i < length; i++) {
	    Node temp = contents [i];
	    if (temp.getNodeName ().equals (name)) {
		contents [i] = (DomNode) arg;
		return temp;
	    }
	}
	if (length == contents.length)
	    setCapacity (length + DELTA);
	contents [length++] = (DomNode) arg;
	return null;
    }


    /**
     * <b>DOM L2</b>
     * Stores the named item into the map, optionally overwriting
     * any existing node with that fully qualified name.  The name
     * used incorporates the localName and namespaceURI properties,
     * and ignores any prefix.
     */
    public Node setNamedItemNS (Node arg)
    {
	if (readonly)
	    throw new DomEx (DomEx.NO_MODIFICATION_ALLOWED_ERR);
	if (arg.getOwnerDocument () != owner)
	    throw new DomEx (DomEx.WRONG_DOCUMENT_ERR);
	if (arg instanceof Attr)
	    checkAttr ((Attr) arg);

	String	localName = arg.getLocalName ();
	String	namespaceURI = arg.getNamespaceURI ();

	if (localName == null)
	    throw new DomEx (DomEx.INVALID_ACCESS_ERR);

	for (int i = 0; i < length; i++) {
	    DomNode	temp = contents [i];
	    String	tempName = temp.getLocalName ();
	    String	ns;

	    if (tempName != null && tempName.equals (localName)) {
		ns = temp.getNamespaceURI ();
		if ((ns == null && namespaceURI == null)
			|| ns.equals (namespaceURI)) {
		    contents [i] = (DomNode) arg;
		    return temp;
		}
	    }
	}
	if (length == contents.length)
	    setCapacity (length + DELTA);
	contents [length++] = (DomNode) arg;
	return null;
    }

    private void maybeRestoreDefault (String uri, String name)
    {
	DomDoctype		doctype = (DomDoctype)owner.getDoctype ();
	DomDoctype.ElementInfo	info;
	String			value;
	DomAttr			attr;

	if (doctype == null)
	    return;
	if ((info = doctype.getElementInfo (element.getNodeName ())) == null)
	    return;
	if ((value = info.getAttrDefault (name)) == null)
	    return;
	if (uri == null)
	    attr = (DomAttr) owner.createAttribute (name);
	else
	    attr = (DomAttr) owner.createAttributeNS (uri, name);
	attr.setNodeValue (value);
	attr.setSpecified (false);
	setNamedItem (attr);
    }

    /**
     * <b>DOM L1</b>
     * Removes the named item from the map, or reports an exception;
     * names are just the nodeName property.
     */
    public Node removeNamedItem (String name)
    {
	if (readonly)
	    throw new DomEx (DomEx.NO_MODIFICATION_ALLOWED_ERR);

// report attribute REMOVAL event?

	for (int i = 0; i < length; i++) {
	    DomNode	temp = contents [i];
	    if (temp.getNodeName ().equals (name)) {
		System.arraycopy (contents, i+1, contents, i,
			length - (i + 1));
		contents [--length] = null;
		if (element != null)
		    maybeRestoreDefault (temp.getNamespaceURI (), name);
		return temp;
	    }
	}
	throw new DomEx (DomEx.NOT_FOUND_ERR);
    }


    /**
     * <b>DOM L2</b>
     * Removes the named item from the map, or reports an exception;
     * names are the localName and namespaceURI properties.
     */
    public Node removeNamedItemNS (String namespaceURI, String localName)
    {
	if (readonly)
	    throw new DomEx (DomEx.NO_MODIFICATION_ALLOWED_ERR);

	for (int i = 0; i < length; i++) {
	    DomNode	temp = contents [i];
	    String	tempName = temp.getLocalName ();
	    String	ns;

	    if (tempName != null && tempName.equals (localName)) {
		ns = temp.getNamespaceURI ();
		if ((ns == null && namespaceURI == null)
			|| ns.equals (namespaceURI)) {
		    System.arraycopy (contents, i+1, contents, i,
			    length - (i + 1));
		    contents [--length] = null;
		    if (element != null)
			maybeRestoreDefault (ns, temp.getNodeName ());
		    return temp;
		}
	    }
	}
	throw new DomEx (DomEx.NOT_FOUND_ERR);
    }


    /**
     * <b>DOM L1</b>
     * Returns the indexed item from the map, or null.
     */
    public Node item (int index)
    {
	if (index < 0 || index >= length)
	    return null;
	return contents [index];
    }


    /**
     * <b>DOM L1</b>
     * Returns the length of the map.
     */
    public int getLength ()
	{ return length; }
}
