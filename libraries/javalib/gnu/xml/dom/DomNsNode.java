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
 * <p> Abstract implemention of namespace support.  This facilitates
 * sharing code for attribute and element nodes.
 *
 * @author David Brownell 
 */
public abstract class DomNsNode extends DomNode
{
    private String		name;		// changed by setPrefix
    private final String	namespace;


    /**
     * Constructs a node associated with the specified document, and
     * with the specified namespace information.
     *
     * @param owner The document with which this entity is associated
     * @param namespaceURI Combined with the local part of the name,
     *	this identifies a type of element or attribute; may be null.
     *  If this is the empty string, it is reassigned as null so that
     *  applications only need to test that case.
     * @param name Name of this node, which may include a prefix
     */
    // package private
    DomNsNode (Document owner, String namespaceURI, String name)
    {
	super (owner);
	this.name = name;
	if ("".equals (namespaceURI))
	    namespaceURI = null;
	this.namespace = namespaceURI;
    }


    /**
     * <b>DOM L1</b>
     * Returns the node's name, including any namespace prefix.
     */
    final public String getNodeName ()
    {
	return name;
    }


    /**
     * <b>DOM L2</b>
     * Returns the node's namespace URI
     * <em>or null</em> if the node name is not namespace scoped.
     */
    final public String getNamespaceURI ()
    {
	return namespace;
    }


    /**
     * <b>DOM L2</b>
     * Returns any prefix part of the node's name (before any colon).
     */
    public String getPrefix ()
    {
	if (namespace == null)
	    return null;

	int index = name.indexOf (':');
	if (index < 0)
	    return null;
	else
	    return name.substring (0, index);
    }


    /**
     * <b>DOM L2</b>
     * Assigns the prefix part of the node's name (before any colon).
     */
    public void setPrefix (String prefix)
    {
	String local = getLocalName ();

	if (isReadonly ())
	    throw new DomEx (DomEx.NO_MODIFICATION_ALLOWED_ERR);

	if (prefix == null) {
	    name = local;
	    return;
	} else if (namespace == null)
	    throw new DomEx (DomEx.NAMESPACE_ERR,
		"can't set prefix, node has no namespace URI", this, 0);

	DomDocument.verifyXmlName (prefix);
	if (prefix.indexOf (':') != -1)
	    throw new DomEx (DomEx.NAMESPACE_ERR,
		"illegal prefix " + prefix, this, 0);

	if ("xml".equals (prefix)
		&& !DomDocument.xmlNamespace.equals (namespace))
	    throw new DomEx (DomEx.NAMESPACE_ERR,
		"xml namespace is always " + DomDocument.xmlNamespace, this, 0);

	if ("xmlns".equals (prefix)) {
	    if (namespace != null || getNodeType () != ATTRIBUTE_NODE)
		throw new DomEx (DomEx.NAMESPACE_ERR,
			"xmlns attribute prefix is reserved", this, 0);
	} else if (getNodeType () == ATTRIBUTE_NODE
		&& ("xmlns".equals (name) || name.startsWith ("xmlns:")))
	    throw new DomEx (DomEx.NAMESPACE_ERR,
		    "namespace declarations can't change names", this, 0);

	name = prefix + ':' + local;
    }


    /**
     * <b>DOM L2</b>
     * Returns the local part of the node's name (after any colon),
     * <em>or null</em> if the node name is not namespace scoped.
     */
    public String getLocalName ()
    {
	if (namespace == null)
	    return null;

	int index = name.indexOf (':');
	if (index < 0)
	    return name;
	else
	    return name.substring (index + 1);
    }
}
