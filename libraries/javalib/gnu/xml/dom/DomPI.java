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

import org.w3c.dom.Document;
import org.w3c.dom.ProcessingInstruction;


/**
 * <p> "ProcessingInstruction" (PI) implementation.
 * This is a non-core DOM class, supporting the "XML" feature. </p>
 *
 * <p> Unlike other DOM APIs in the "XML" feature, this one fully
 * exposes the functionality it describes.  So there is no reason
 * inherent in DOM to avoid using this API, unless you want to rely
 * on NOTATION declarations to associate meaning with your PIs;
 * there is no vendor-neutal way to record those notations in DOM.</p>
 *
 * <p> Also of note is that PI support is part of SAX, so that XML
 * systems using PIs can choose among multiple APIs. </p>
 *
 * @see DomNotation
 *
 * @author David Brownell 
 */
public class DomPI extends DomNode implements ProcessingInstruction
{
    private String	target;
    private String	data;


    /**
     * Constructs a ProcessingInstruction node associated with the
     * specified document, with the specified data.
     *
     * <p>This constructor should only be invoked by a Document object as
     * part of its createProcessingInstruction functionality, or through
     * a subclass which is similarly used in a "Sub-DOM" style layer. 
     */
    protected DomPI (Document owner, String target, String data)
    {
	super (owner);
	this.target = target;
	this.data = data;
    }


    /**
     * <b>DOM L1</b>
     * Returns the target of the processing instruction.
     */
    final public String getTarget ()
    {
	return target;
    }


    /**
     * <b>DOM L1</b>
     * Returns the target of the processing instruction
     * (same as getTarget).
     */
    final public String getNodeName ()
    {
	return target;
    }


    /**
     * <b>DOM L1</b>
     * Returns the constant PROCESSING_INSTRUCTION_NODE.
     */
    final public short getNodeType ()
	{ return PROCESSING_INSTRUCTION_NODE; }


    /**
     * <b>DOM L1</b>
     * Returns the data associated with the processing instruction.
     */
    final public String getData ()
    {
	return data;
    }


    /**
     * <b>DOM L1</b>
     * Returns the data associated with the processing instruction
     * (same as getData).
     */
    final public String getNodeValue ()
    {
	return data;
    }


    /**
     * <b>DOM L1</b>
     * Assigns the data associated with the processing instruction;
     * same as setNodeValue.
     */
    final public void setData (String data)
    {
	setNodeValue (data);
    }


    /**
     * <b>DOM L1</b>
     * Assigns the data associated with the processing instruction.
     */
    final public void setNodeValue (String data)
    {
	if (isReadonly ())
	    throw new DomEx (DomEx.NO_MODIFICATION_ALLOWED_ERR);
	this.data = data;
    }
}
