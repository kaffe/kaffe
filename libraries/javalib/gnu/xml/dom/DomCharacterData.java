/*
 * DomCharacterData.java
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
import org.w3c.dom.events.MutationEvent;


/**
 * <p> Abstract "CharacterData" implementation.  This
 * facilitates reusing code in classes implementing subtypes of that DOM
 * interface (Text, Comment, CDATASection).  </p>
 *
 * @author David Brownell
 */
public abstract class DomCharacterData extends DomNode
    implements CharacterData
{
    private char		raw [];

    // package private
    DomCharacterData (Document doc, String value)
    {
	super (doc);
	if (value != null)
	    raw = value.toCharArray ();
	else
	    raw = new char [0];
    }

    // package private
    DomCharacterData (Document doc, char buf [], int offset, int length)
    {
	super (doc);
	if (buf == null)
	    raw = new char [0];
	else {
	    raw = new char [length];
	    System.arraycopy (buf, offset, raw, 0, length);
	}
    }

    /**
     * <b>DOM L1</b>
     * Appends the specified data to the value of this node.
     * Causes a DOMCharacterDataModified mutation event to be reported.
     */
    public void appendData (String arg)
    {
	if (isReadonly ())
	    throw new DomEx (DomEx.NO_MODIFICATION_ALLOWED_ERR);

	char	tmp [] = arg.toCharArray ();
	char	buf [] = new char [raw.length + tmp.length];

	System.arraycopy (raw, 0, buf, 0, raw.length);
	System.arraycopy (tmp, 0, buf, raw.length, tmp.length);
	mutating (new String (buf));
	raw = buf;
    }
    

    /**
     * <b>DOM L1</b>
     * Modifies the value of this node.
     * Causes a DOMCharacterDataModified mutation event to be reported.
     */
    public void deleteData (int offset, int count)
    {
	char	buf [];

	if (isReadonly ())
	    throw new DomEx (DomEx.NO_MODIFICATION_ALLOWED_ERR);
	if (offset < 0 || count < 0 || offset > raw.length)
	    throw new DomEx (DomEx.INDEX_SIZE_ERR);
	if ((offset + count) > raw.length)
	    count = raw.length - offset;
	if (count == 0)
	    return;
	buf = new char [raw.length - count];
	System.arraycopy (raw, 0, buf, 0, offset);
	System.arraycopy (raw, offset + count, buf, offset,
	    raw.length - (offset + count));
	mutating (new String (buf));
	raw = buf;
    }
    

    /**
     * <b>DOM L1</b>
     * Returns the value of this node.
     */
    public String getNodeValue ()
    {
	return new String (raw);
    }

    
    /**
     * <b>DOM L1</b>
     * Returns the value of this node; same as getNodeValue.
     */
    final public String getData ()
    {
	return getNodeValue ();
    }


    /**
     * <b>DOM L1</b>
     * Returns the length of the data.
     */
    public int getLength ()
    {
	return raw.length;
    }


    static final class EmptyNodeList implements NodeList
    {
	public int getLength () { return 0; }
	public Node item (int i) { return null; }
    }
    
    static final EmptyNodeList	theEmptyNodeList = new EmptyNodeList ();


    /**
     * <b>DOM L1</b>
     * Returns an empty list of children.
     */
    final public NodeList getChildNodes ()
    {
	return theEmptyNodeList;
    }


    /**
     * <b>DOM L1</b>
     * Modifies the value of this node.
     */
    public void insertData (int offset, String arg)
    {
	char	tmp [] = arg.toCharArray ();
	char	buf [] = new char [raw.length + tmp.length];

	if (isReadonly ())
	    throw new DomEx (DomEx.NO_MODIFICATION_ALLOWED_ERR);
	try {
	    System.arraycopy (raw, 0, buf, 0, offset);
	    System.arraycopy (tmp, 0, buf, offset,
		tmp.length);
	    System.arraycopy (raw, offset, buf, offset + tmp.length,
	    	raw.length - offset);
	    mutating (new String (buf));
	    raw = buf;
	} catch (IndexOutOfBoundsException x) {
	    throw new DomEx (DomEx.INDEX_SIZE_ERR);
	}
    }
    

    /**
     * <b>DOM L1</b>
     * Modifies the value of this node.  Causes DOMCharacterDataModified
     * mutation events to be reported (at least one).
     */
    public void replaceData (int offset, int count, String arg)
    {
	if (isReadonly ())
	    throw new DomEx (DomEx.NO_MODIFICATION_ALLOWED_ERR);

	// this could be rewritten to be faster,
	// and to report only one mutation event
	deleteData (offset, count);
	insertData (offset, arg);
    }
    

    /**
     * <b>DOM L1</b>
     * Assigns the value of this node.
     * Causes a DOMCharacterDataModified mutation event to be reported.
     */
    public void setNodeValue (String value)
    {
	if (isReadonly ())
	    throw new DomEx (DomEx.NO_MODIFICATION_ALLOWED_ERR);
	if (value == null)
	    value = "";
	mutating (value);
	raw = value.toCharArray ();
    }

    
    /**
     * <b>DOM L1</b>
     * Assigns the value of this node; same as setNodeValue.
     */
    final public void setData (String data)
    {
	setNodeValue (data);
    }

    
    /**
     * <b>DOM L1</b>
     * Returns the specified substring.
     */
    public String substringData (int offset, int count)
    {
	try {
	    return new String (raw, offset, count);
	} catch (IndexOutOfBoundsException e) {
	    if (offset >= 0 && count >= 0) {
		int len = raw.length;
		if (offset < len && (offset + count) > len)
		    return new String (raw, offset, len - offset);
	    }
	    throw new DomEx (DomEx.INDEX_SIZE_ERR);
	}
    }

    private void mutating (String newValue)
    {
	if (!reportMutations)
	    return;

	// EVENT:  DOMCharacterDataModified, target = this,
	//	prev/new values provided
	MutationEvent	event;

	event = (MutationEvent) createEvent ("MutationEvents");
	event.initMutationEvent ("DOMCharacterDataModified",
		true /* bubbles */, false /* nocancel */,
		null, new String (raw), newValue, null, (short) 0);
	dispatchEvent (event);
    }
}
