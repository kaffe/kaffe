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

/**
 *  The <code>CSS2TextShadow</code> interface represents a simple value for 
 * the  text-shadow CSS Level 2 property.
 * <p> For this extension of the <code>CSSValue</code> interface, the 
 * <code>valueType</code> attribute of the underlying <code>CSSValue</code> 
 * interface shall be <code>CSS_CUSTOM</code> . 
 * <p>See also the <a href='http://www.w3.org/TR/2000/CR-DOM-Level-2-20000510'>Document Object Model (DOM) Level 2 Specification</a>.
 * @since DOM Level 2
 */
public interface CSS2TextShadow extends CSSValue {
    /**
     *  Specifies the color of the text shadow. The CSS Value can contain an 
     * empty string if no color has been specified.
     */
    public CSSValue getColor();

    /**
     *  The horizontal position of the text shadow. <code>0</code> if no 
     * length has been specified.
     */
    public CSSValue getHorizontal();

    /**
     *  The vertical position of the text shadow. <code>0</code> if no length 
     * has been specified.
     */
    public CSSValue getVertical();

    /**
     *  The blur radius of the text shadow. <code>0</code> if no length has 
     * been specified.
     */
    public CSSValue getBlur();

}

