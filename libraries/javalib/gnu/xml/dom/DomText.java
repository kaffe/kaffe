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

import org.w3c.dom.*;


/**
 * <p> "Text" implementation.  </p>
 *
 * @author David Brownell 
 */
public class DomText
  extends DomCharacterData
  implements Text
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
	super (TEXT_NODE, owner, value);
    }

    protected DomText (Document owner, char buf [], int off, int len)
    {
	super (TEXT_NODE, owner, buf, off, len);
    }

    // Used by DomCDATA
    DomText (short nodeType, Document owner, String value)
    {
	super (nodeType, owner, value);
    }

    DomText (short nodeType, Document owner, char buf [], int off, int len)
    {
	super (nodeType, owner, buf, off, len);
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
    
    // DOM Level 3

    public boolean isElementContentWhitespace ()
      {
        return getTextContent ().trim ().length () == 0;
      }

    public String getWholeText ()
      {
        Node first = this;
        Node node = getPreviousSibling ();
        while (node != null && node instanceof Text)
          {
            first = node;
            node = node.getPreviousSibling ();
          }
        StringBuffer buf = new StringBuffer (first.getNodeValue ());
        node = first.getNextSibling ();
        while (node != null && node instanceof Text)
          {
            buf.append (node.getNodeValue ());
            node = node.getNextSibling ();
          }
        return buf.toString ();
      }

    public Text replaceWholeText (String content) throws DOMException
      {
        boolean isEmpty = (content == null || content.length () == 0);
        if (!isEmpty)
          {
            setNodeValue (content);
          }
        
        Node first = this;
        Node node = getPreviousSibling ();
        while (node != null && node instanceof Text)
          {
            first = node;
            node = node.getPreviousSibling ();
          }
        node = first.getNextSibling ();
        Node parent = getParentNode ();
        if (first != this || isEmpty)
          {
            parent.removeChild (first);
          }
        while (node != null && node instanceof Text)
          {
            Node tmp = node;
            node = node.getNextSibling ();
            if (tmp != this || isEmpty)
              {
                parent.removeChild (tmp);
              }
          }
        return (isEmpty) ? null : this;
      }
    
}
