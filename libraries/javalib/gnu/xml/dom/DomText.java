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
 * <p> "Text" implementation.  </p>
 *
 * @author David Brownell 
 */
public class DomText extends DomCharacterData implements Text
{
    // NOTE:  deleted unused per-instance "isIgnorable"
    // support to reclaim its space.

    /**
     * Constructs a text node associated with the specified
     * document and holding the specified data.
     *
     * <p>This constructor should only be invoked by a Document object
     * as part of its createTextNode functionality, or through a subclass
     * which is similarly used in a "Sub-DOM" style layer. 
     */
    protected DomText (Document owner, String value)
    {
	super (owner, value);
    }

    protected DomText (Document owner, char buf [], int off, int len)
    {
	super (owner, buf, off, len);
    }


    /**
     * <b>DOM L1</b>
     * Returns the string "#text".
     */
    // can't be 'final' with CDATA subclassing
    public String getNodeName ()
    {
	return "#text";
    }

    /**
     * <b>DOM L1</b>
     * Returns the constant TEXT_NODE.
     */
    // would be final except DomCDATA subclasses this ...
    public short getNodeType ()
	{ return TEXT_NODE; }


    /**
     * <b>DOM L1</b>
     * Splits this text node in two parts at the offset, returning
     * the new text node (the sibling with the second part).
     */
    public Text splitText (int offset)
    {
	if (isReadonly ())
	    throw new DomEx (DomEx.NO_MODIFICATION_ALLOWED_ERR);
	try {
	    String	before = getNodeValue ().substring (0, offset);
	    String	after = getNodeValue ().substring (offset);
	    Text	next;

	    if (getNodeType () == TEXT_NODE)
		next = getOwnerDocument ().createTextNode (after);
	    else // CDATA_SECTION_NODE
		next = getOwnerDocument ().createCDATASection (after);

	    getParentNode ().insertBefore (next, getNextSibling ());
	    setNodeValue (before);
	    return next;

	} catch (IndexOutOfBoundsException x) {
	    throw new DomEx (DomEx.INDEX_SIZE_ERR);
	}
    }
}
