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
 * <p> "EntityReference" implementation (reference to parsed entity).
 * This is a non-core DOM class, supporting the "XML" feature.
 * It does not represent builtin entities (such as "&amp;amp;")
 * or character references, which are always directly expanded in
 * DOM trees.</p>
 *
 * <p> Note that while the DOM specification permits these nodes to have
 * a readonly set of children, this is not required.  Similarly, it does
 * not require a DOM to couple EntityReference nodes with any Entity nodes
 * that have the same entity name (and equivalent children).  It also
 * effectively guarantees that references created directly or indirectly
 * through the <em>Document.ImportNode</em> method will not have children.
 * The level of functionality you may get is extremely variable.
 *
 * <p> Also significant is that even at their most functional level, the fact
 * that EntityReference children must be readonly has caused significant
 * problems when modifying work products held in DOM trees.  Other problems
 * include issues related to undeclared namespace prefixes (and references
 * to the current default namespace) that may be found in the text of such
 * parsed entities nodes.  These must be contextually bound as part of DOM
 * tree construction.  When such nodes are moved, the namespace associated
 * with a given prefix (or default) may change to be in conflict with the
 * namespace bound to the node at creation time.
 *
 * <p> In short, <em>avoid using this DOM functionality</em>.
 *
 * @see DomDoctype
 * @see DomEntity
 *
 * @author David Brownell 
 */
public class DomEntityReference extends DomNode implements EntityReference
{
    private String	name;


    /**
     * Constructs an EntityReference node associated with the specified
     * document.  The creator should populate this with whatever contents
     * are appropriate, and then mark it as readonly.
     *
     * <p>This constructor should only be invoked by a Document as part of
     * its createEntityReference functionality, or through a subclass which
     * is similarly used in a "Sub-DOM" style layer.
     *
     * @see DomNode#makeReadonly
     */
    protected DomEntityReference (Document owner, String name)
    {
	super (owner);
	this.name = name;
    }


    /**
     * <b>DOM L1</b>
     * Returns the name of the referenced entity.
     */
    final public String getNodeName ()
    {
	return name;
    }

    /**
     * <b>DOM L1</b>
     * Returns the constant ENTITY_REFERENCE_NODE.
     */
    final public short getNodeType ()
	{ return ENTITY_REFERENCE_NODE; }
}
