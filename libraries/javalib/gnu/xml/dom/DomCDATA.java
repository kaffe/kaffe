/*
 * DomCDATA.java
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

import org.w3c.dom.*;


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
