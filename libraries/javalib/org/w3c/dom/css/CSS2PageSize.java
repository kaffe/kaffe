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
 *  The <code>CSS2PageSize</code> interface represents the  size CSS Level 2 
 * descriptor.
 * <p> For this extension of the <code>CSSValue</code> interface, the 
 * <code>valueType</code> attribute of the underlying <code>CSSValue</code> 
 * interface shall be <code>CSS_CUSTOM</code> . 
 * <p>See also the <a href='http://www.w3.org/TR/2000/CR-DOM-Level-2-20000510'>Document Object Model (DOM) Level 2 Specification</a>.
 * @since DOM Level 2
 */
public interface CSS2PageSize extends CSSValue {
    /**
     *  A code defining the type of the width of the page. It would be one of 
     * <code>CSS_EMS</code> , <code>CSS_EXS</code> , <code>CSS_PX</code> , 
     * <code>CSS_CM</code> , <code>CSS_MM</code> , <code>CSS_IN</code> , 
     * <code>CSS_PT</code> , <code>CSS_PC</code> or <code>CSS_IDENT</code> .
     */
    public short getWidthType();

    /**
     *  A code defining the type of the height of the page. It would be one of 
     * <code>CSS_EMS</code> , <code>CSS_EXS</code> , <code>CSS_PX</code> , 
     * <code>CSS_CM</code> , <code>CSS_MM</code> , <code>CSS_IN</code> , 
     * <code>CSS_PT</code> , <code>CSS_PC</code> or <code>CSS_IDENT</code> . 
     * If one of width or height is <code>CSS_IDENT</code> , it's guaranteed 
     * that the other is the same.
     */
    public short getHeightType();

    /**
     *  If <code>width</code> is <code>CSS_IDENT</code> , this attribute 
     * contains the string representation of the ident, otherwise it contains 
     * an empty string. 
     */
    public String getIdentifier();

    /**
     *  This method is used to get the float value in a specified unit if the 
     * <code>widthType</code> represents a length. If the float doesn't 
     * contain a float value or can't be converted into the specified unit, a 
     * <code>DOMException</code> is raised.
     * @param wType  The width unit.
     * @return  The float value.
     * @exception DOMException
     *    INVALID_ACCESS_ERR: Raised if the property doesn't contain a float  
     *   or the value can't be converted.
     */
    public float getWidth(float wType)
                          throws DOMException;

    /**
     *  This method is used to get the float value in a specified unit if the 
     * <code>heightType</code> represents a length. If the float doesn't 
     * contain a float value or can't be converted into the specified unit, a 
     * <code>DOMException</code> is raised. If only the width value has been 
     * specified, the height value is the same.
     * @param hType  The height unit.
     * @return  The float value.
     * @exception DOMException
     *    INVALID_ACCESS_ERR: Raised if the property doesn't contain a float  
     *   or the value can't be converted.
     */
    public float getHeightSize(float hType)
                               throws DOMException;

    /**
     *  This method is used to set the width position with a specified unit. 
     * If the <code>heightType</code> is not a length, it sets the height 
     * position to the same value. 
     * @param wType  The width unit.
     * @param value  The new value. 
     * @exception DOMException
     *    INVALID_ACCESS_ERR: Raised if the specified unit is not a length or 
     *   a percentage.
     *   <br> NO_MODIFICATION_ALLOWED_ERR: Raised if this property is 
     *   readonly.
     */
    public void setWidthSize(short wType, 
                             float value)
                             throws DOMException;

    /**
     *  This method is used to set the height position with a specified unit. 
     * If the <code>widthType</code> is not a length, it sets the width 
     * position to the same value. 
     * @param hType  The height unit.
     * @param value  The new value. 
     * @exception DOMException
     *    INVALID_ACCESS_ERR: Raised if the specified unit is not a length or 
     *   a percentage.
     *   <br> NO_MODIFICATION_ALLOWED_ERR: Raised if this property is 
     *   readonly.
     */
    public void setHeightSize(short hType, 
                              float value)
                              throws DOMException;

    /**
     *  Sets the identifier.
     * @param ident  The new identifier.
     * @exception DOMException
     *    SYNTAX_ERR: Raised if the identifier has a syntax error and is 
     *   unparsable.
     *   <br> NO_MODIFICATION_ALLOWED_ERR: Raised if this property is 
     *   readonly.
     */
    public void setIdentifier(String ident)
                              throws DOMException;

}

