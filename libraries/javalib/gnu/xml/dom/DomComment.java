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
 * <p> "Comment" implementation.
 * Comments hold data intended for direct consumption by people;
 * programs should only use ProcessingInstruction nodes.  Note that
 * since SAX makes comment reporting optional, XML systems that
 * rely on comments (such as by using this class) will often lose
 * those comments at some point in the processing pipeline. </p>
 *
 * @author David Brownell
 */
public class DomComment extends DomCharacterData implements Comment
{
    /**
     * Constructs a comment node associated with the specified
     * document and holding the specified data.
     *
     * <p>This constructor should only be invoked by a Document as part of
     * its createComment functionality, or through a subclass which is
     * similarly used in a "Sub-DOM" style layer.
     */
    protected DomComment (Document owner, String value)
    {
	super (owner, value);
    }


    /**
     * <b>DOM L1</b>
     * Returns the string "#comment".
     */
    final public String getNodeName ()
    {
	return "#comment";
    }

    /**
     * <b>DOM L1</b>
     * Returns the constant COMMENT_NODE.
     */
    final public short getNodeType ()
	{ return COMMENT_NODE; }
}
