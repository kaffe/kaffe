/*
 * DomAttr.java
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
 * <p> "Attr" implementation.  In DOM, attributes cost quite a lot of
 * memory because their values are complex structures rather than just
 * simple strings.  To reduce your costs, avoid having more than one
 * child of an attribute; stick to a single Text node child, and ignore
 * even that by using the attribute's "nodeValue" property.</p>
 *
 * <p> As a bit of general advice, only look at attribute modification
 * events through the DOMAttrModified event (sent to the associated
 * element).  Implementations are not guaranteed to report other events
 * in the same order, so you're very likely to write nonportable code if
 * you monitor events at the "children of Attr" level.</p>
 *
 * <p> At this writing, not all attribute modifications will cause the
 * DOMAttrModified event to be triggered ... only the ones using the string
 * methods (setNodeValue, setValue, and Element.setAttribute) to modify
 * those values.  That is, if you manipulate those children directly,
 * elements won't get notified that attribute values have changed.
 * The natural fix for that will report other modifications, but won't 
 * be able to expose "previous" attribute value; it'll need to be cached
 * or something (at which point why bother using child nodes). </p>
 *
 * <p><em>You are strongly advised not to use "children" of any attribute
 * nodes you work with.</em> </p>
 *
 * @author David Brownell
 */
public class DomAttr extends DomNsNode implements Attr
{
    private boolean	specified;

    // NOTE:  it could be possible to rework this code a bit so that
    // this extra field isn't needed; "parent" might do double duty,
    // with appropriate safeguards.  Using less space is healthy!
    private DomElement	element;


    /**
     * Constructs an Attr node associated with the specified document.
     * The "specified" flag is initialized to true, since this DOM has
     * no current "back door" mechanisms to manage default values so
     * that every value must effectively be "specified".
     *
     * <p>This constructor should only be invoked by a Document as part of
     * its createAttribute functionality, or through a subclass which is
     * similarly used in a "Sub-DOM" style layer.
     *
     * @param owner The document with which this node is associated
     * @param namespaceURI Combined with the local part of the name,
     *	this is used to uniquely identify a type of attribute
     * @param name Name of this attribute, which may include a prefix
     */
    protected DomAttr (Document owner, String namespaceURI, String name)
    {
	super (owner, namespaceURI, name);
	specified = true;

	// XXX register self to get insertion/removal events
	// and character data change events and when they happen,
	// report self-mutation
    }


    /**
     * <b>DOM L1</b>
     * Returns the attribute name (same as getNodeName)
     */
    final public String getName ()
    {
	return getNodeName ();
    }

    /**
     * <b>DOM L1</b>
     * Returns the constant ATTRIBUTE_NODE.
     */
    final public short getNodeType ()
	{ return ATTRIBUTE_NODE; }


    /**
     * <b>DOM L1</b>
     * Returns true if a parser reported this was in the source text.
     */
    final public boolean getSpecified ()
    {
	return specified;
    }


    /**
     * Records whether this attribute was in the source text.
     */
    final public void setSpecified (boolean value)
    {
	specified = value;
    }


    /**
     * <b>DOM L1</b>
     * Returns the attribute value, with character and entity
     * references substituted.
     * <em>NOTE:  entity refs as children aren't currently handled.</em>
     */
    public String getNodeValue ()
    {
	int	length = getLength ();
	String	retval = null;

	for (int i = 0; i < length; i++) {
	    Node	n = item (i);

	    if (n.getNodeType () == TEXT_NODE) {
		if (retval == null)
		    retval = n.getNodeValue ();
		else
		    retval += n.getNodeValue ();
		continue;
	    }
	    
	    // XXX entity ref child of attribute
	    // contents exclude comments, PIs, elements
	    throw new DomEx (DomEx.NOT_SUPPORTED_ERR);
	}
	if (retval == null)
	    retval = "";
	return retval;
    }


    /**
     * <b>DOM L1</b>
     * Assigns the value of the attribute; it will have one child,
     * which is a text node with the specified value (same as
     * setNodeValue).
     */
    final public void setValue (String value)
    {
	setNodeValue (value);
    }


    /**
     * <b>DOM L1</b>
     * Returns the value of the attribute as a non-null string; same
     * as getNodeValue.
     * <em>NOTE:  entity refs as children aren't currently handled.</em>
     */
    final public String getValue ()
    {
	return getNodeValue ();
    }


    /**
     * <b>DOM L1</b>
     * Assigns the attribute value; using this API, no entity or
     * character references will exist.
     * Causes a DOMAttrModified mutation event to be sent.
     */
    public void setNodeValue (String value)
    {
	int	len = getLength ();
	String	oldValue;

	if (isReadonly ())
	    throw new DomEx (DomEx.NO_MODIFICATION_ALLOWED_ERR);

	oldValue = getValue ();
	for (int i = 0; i < len; i++)
	    removeChild (getLastChild ());
	appendChild (getOwnerDocument ().createTextNode (value));
	specified = true;
	
	mutating (oldValue, value, MutationEvent.MODIFICATION);
    }


    /**
     * <b>DOM L2</b>
     * Returns the element with which this attribute is associated.
     */
    final public Element getOwnerElement ()
    {
	return element;
    }


    /**
     * Records the element with which this attribute is associated.
     */
    final public void setOwnerElement (Element e)
    {
	if (element != null)
	    throw new DomEx (DomEx.HIERARCHY_REQUEST_ERR);
	if (!(e instanceof DomElement))
	    throw new DomEx (DomEx.WRONG_DOCUMENT_ERR);
	element = (DomElement) e;
    }


    /**
     * Shallow clone of the attribute, breaking all ties with any
     * elements.
     */
    public Object clone ()
    {
	DomAttr retval = (DomAttr) super.clone ();

	retval.element = null;
	retval.specified = false;
	return retval;
    }

    private void mutating (String oldValue, String newValue, short why)
    {
	if (!reportMutations || element == null)
	    return;

	// EVENT:  DOMAttrModified, target = element,
	//	prev/new values provided, also attr name
	MutationEvent	event;

	event = (MutationEvent) createEvent ("MutationEvents");
	event.initMutationEvent ("DOMAttrModified",
		true /* bubbles */, false /* nocancel */,
		null, oldValue, newValue, getNodeName (), why);
	element.dispatchEvent (event);
    }

    // DOM Level 3 methods

    public TypeInfo getSchemaTypeInfo ()
      {
        // TODO
        return null;
      }

    public boolean isId ()
      {
        // TODO
        return false;
      }

}
