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
 * <p> Abstract implemention of nodes describing external DTD-related
 * objects.  This facilitates reusing code for Entity, Notation, and
 * DocumentType (really, external subset) nodes.  Such support is not
 * part of the core DOM; it's for the "XML" feature.  </p>
 *
 * <p> Note that you are strongly advised to avoid using the DOM
 * features that take advantage of this class, since (as of L2) none
 * of them is defined fully enough to permit full use of the
 * XML feature they partially expose. </p>
 *
 * @author David Brownell 
 */
public abstract class DomExtern extends DomNode
{
    private String	name;
    private String	publicId;
    private String	systemId;


    /**
     * Constructs a node associated with the specified document,
     * with the specified descriptive data.
     *
     * @param owner The document with which this object is associated
     * @param name Name of this object
     * @param publicId If non-null, provides the entity's PUBLIC identifier
     * @param systemId If non-null, provides the entity's SYSTEM identifier
     */
    // package private
    DomExtern (
	Document owner,
	String name,
	String publicId,
	String systemId
    )
    {
	super (owner);
	this.name = name;
	this.publicId = publicId;
	this.systemId = systemId;
    }


    /**
     * <b>DOM L1</b>
     * Returns the SYSTEM identifier associated with this object, if any.
     */
    final public String getSystemId ()
    {
	return systemId;
    }


    /**
     * <b>DOM L1</b>
     * Returns the PUBLIC identifier associated with this object, if any.
     */
    final public String getPublicId ()
    {
	return publicId;
    }


    /**
     * <b>DOM L1</b>
     * Returns the object's name.
     */
    final public String getNodeName ()
    {
	return name;
    }
}
