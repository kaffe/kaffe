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

import org.w3c.dom.CDATASection;
import org.w3c.dom.Document;


/**
 * <p> "CDATASection" implementation.
 * This is a non-core DOM class, supporting the "XML" feature.
 * CDATA sections are just ways to represent text using different
 * delimeters. </p>
 *
 * <p> <em>You are strongly advised not to use CDATASection nodes.</em>
 * The advantage of having slightly prettier ways to print text that may
 * have lots of embedded XML delimiters, such as "&amp;" and "&lt;",
 * can be dwarfed by the cost of dealing with multiple kinds of text
 * nodes in all your algorithms. </p>
 *
 * @author David Brownell
 */
public class DomCDATA extends DomText implements CDATASection
{
    /**
     * Constructs a CDATA section node associated with the specified
     * document and holding the specified data.
     *
     * <p>This constructor should only be invoked by a Document as part of
     * its createCDATASection functionality, or through a subclass which is
     * similarly used in a "Sub-DOM" style layer.
     *
     */
    protected DomCDATA (Document owner, String value)
    {
	super (owner, value);
    }

    protected DomCDATA (Document owner, char buf [], int off, int len)
    {
	super (owner, buf, off, len);
    }


    /**
     * <b>DOM L1</b>
     * Returns the string "#cdata-section".
     */
    final public String getNodeName ()
    {
	return "#cdata-section";
    }


    /**
     * <b>DOM L1</b>
     * Returns the constant CDATA_SECTION_NODE.
     */
    final public short getNodeType ()
	{ return CDATA_SECTION_NODE; }
}
