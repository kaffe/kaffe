/*
 * Copyright (c) 2000 World Wide Web Consortium,
 * (Massachusetts Institute of Technology, Institut National de
 * Recherche en Informatique et en Automatique, Keio University). All
 * Rights Reserved. This program is distributed under the W3C's Software
 * Intellectual Property License. This program is distributed in the
 * hope that it will be useful, but WITHOUT ANY WARRANTY; without even
 * the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See W3C License http://www.w3.org/Consortium/Legal/ for more
 * details.
 */

package org.w3c.dom.css;

import org.w3c.dom.DOMException;

/**
 *  The <code>CSS2CounterReset</code> interface represents a simple value for 
 * the  counter-reset CSS Level 2 property.
 * <p> For this extension of the <code>CSSValue</code> interface, the 
 * <code>valueType</code> attribute of the underlying <code>CSSValue</code> 
 * interface shall be <code>CSS_CUSTOM</code> . 
 * <p>See also the <a href='http://www.w3.org/TR/2000/CR-DOM-Level-2-20000510'>Document Object Model (DOM) Level 2 Specification</a>.
 * @since DOM Level   2
 */
public interface CSS2CounterReset extends CSSValue {
    /**
     *  The element name.
     * @exception DOMException
     *    SYNTAX_ERR: Raised if the specified identifier has a syntax error 
     *   and is unparsable.
     *   <br> NO_MODIFICATION_ALLOWED_ERR: Raised if this identifier is 
     *   readonly.
     */
    public String getIdentifier();
    public void setIdentifier(String identifier)
                                   throws DOMException;

    /**
     *  The reset (default value is 0).
     * @exception DOMException
     *    NO_MODIFICATION_ALLOWED_ERR: Raised if this identifier is readonly.
     */
    public short getReset();
    public void setReset(short reset)
                                   throws DOMException;

}

