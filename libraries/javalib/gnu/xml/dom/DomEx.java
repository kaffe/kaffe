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

import org.w3c.dom.DOMException;
import org.w3c.dom.Node;


/**
 * <p> DOMException implementation.   The version that
 * is provided by the W3C is abstract, so it can't be instantiated.
 *
 * <p> This also provides a bit more information about the error
 * that is being reported, in terms of the relevant DOM structures
 * and data.
 *
 * @author David Brownell 
 */
public class DomEx extends DOMException
{
    /** @serial Data that caused an error to be reported */
    private String data;

    /** @serial Node associated with the error. */
    private Node node;

    /** @serial Data associated with the error. */
    private int value;

    /**
     * Constructs an exception, with the diagnostic message
     * corresponding to the specified code.
     */
    public DomEx (short code)
    {
	super (code, diagnostic (code));
    }

    /**
     * Constructs an exception, with the diagnostic message
     * corresponding to the specified code and additional
     * information as provided.
     */
    public DomEx (short code, String data, Node node, int value)
    {
	super (code, diagnostic (code));
	this.data = data;
	this.node = node;
	this.value = value;
    }

    /** Returns the node to which the diagnotic applies, or null. */
    final public Node getNode () { return node; }

    /** Returns data to which the diagnotic applies, or null. */
    final public String getData () { return data; }

    /** Returns data to which the diagnotic applies, or null. */
    final public int getValue () { return value; }


    /**
     * Returns a diagnostic message that may be slightly more useful
     * than the generic one, where possible.
     */
    public String getMessage ()
    {
	String retval = super.getMessage ();

	if (data != null)
	    retval += "\nMore Information: " + data;
	if (value != 0)
	    retval += "\nNumber: " + value;
	if (node != null)
	    retval += "\nNode Name: " + node.getNodeName ();
	return retval;
    }


    // these strings should be localizable.

    private static String diagnostic (short code)
    {
	switch (code) {

	    // DOM L1:
	    case INDEX_SIZE_ERR:
		return "An index or size is out of range.";
	    case DOMSTRING_SIZE_ERR:
		return "A string is too big.";
	    case HIERARCHY_REQUEST_ERR:
		return "The node doesn't belong here.";
	    case WRONG_DOCUMENT_ERR:
		return "The node belongs in another document.";
	    case INVALID_CHARACTER_ERR:
		return "That character is not permitted.";
	    case NO_DATA_ALLOWED_ERR:
		return "This node does not permit data.";
	    case NO_MODIFICATION_ALLOWED_ERR:
		return "No changes are allowed.";
	    case NOT_FOUND_ERR:
		return "The node was not found in that context.";
	    case NOT_SUPPORTED_ERR:
		return "That object is not supported.";
	    case INUSE_ATTRIBUTE_ERR:
		return "The attribute belongs to a different element.";

	    // DOM L2:
	    case INVALID_STATE_ERR:
		return "The object is not usable.";
	    case SYNTAX_ERR:
		return "An illegal string was provided.";
	    case INVALID_MODIFICATION_ERR:
		return "An object's type may not be changed.";
	    case NAMESPACE_ERR:
		return "The operation violates XML Namespaces.";
	    case INVALID_ACCESS_ERR:
		return "Parameter or operation isn't supported by this node.";
	}
	return "Reserved exception number: " + code;
    }

}
