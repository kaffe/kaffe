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
 *  The <code>CSS2BackgroundPosition</code> interface represents the  
 * background-position CSS Level 2 property.
 * <p> For this extension of the <code>CSSValue</code> interface, the 
 * <code>valueType</code> attribute of the underlying <code>CSSValue</code> 
 * interface shall be <code>CSS_CUSTOM</code> . 
 * <p>See also the <a href='http://www.w3.org/TR/2000/CR-DOM-Level-2-20000510'>Document Object Model (DOM) Level 2 Specification</a>.
 * @since DOM Level 2
 */
public interface CSS2BackgroundPosition extends CSSValue {
    /**
     *  A code defining the type of the horizontal value. It would be one of 
     * <code>CSS_PERCENTAGE</code> , <code>CSS_EMS</code> , 
     * <code>CSS_EXS</code> , <code>CSS_PX</code> , <code>CSS_CM</code> , 
     * <code>CSS_MM</code> , <code>CSS_IN</code> , <code>CSS_PT</code> , 
     * <code>CSS_PC</code> or <code>CSS_IDENT</code> . If one of horizontal or
     *  vertical is <code>CSS_IDENT</code> , it's guaranteed that the other is
     *  the same. 
     */
    public short getHorizontalType();

    /**
     *  A code defining the type of the horizontal value. The code can be one 
     * of the following units : <code>CSS_PERCENTAGE</code> , 
     * <code>CSS_EMS</code> , <code>CSS_EXS</code> , <code>CSS_PX</code> , 
     * <code>CSS_CM</code> , <code>CSS_MM</code> , <code>CSS_IN</code> , 
     * <code>CSS_PT</code> , <code>CSS_PC</code> , <code>CSS_IDENT</code> , 
     * <code>CSS_INHERIT</code> . If one of horizontal or vertical is 
     * <code>CSS_IDENT</code> or <code>CSS_INHERIT</code> , it's guaranteed 
     * that the other is the same. 
     */
    public short getVerticalType();

    /**
     *  If <code>horizontalType</code> is <code>CSS_IDENT</code> or 
     * <code>CSS_INHERIT</code> , this attribute contains the string 
     * representation of the ident, otherwise it contains an empty string. 
     */
    public String getHorizontalIdentifier();

    /**
     *  If <code>verticalType</code> is <code>CSS_IDENT</code> or 
     * <code>CSS_INHERIT</code> , this attribute contains the string 
     * representation of the ident, otherwise it contains an empty string. The
     *  value is <code>"center"</code> if only the horizontalIdentifier has 
     * been set. 
     */
    public String getVerticalIdentifier();

    /**
     *  This method is used to get the float value in a specified unit if the 
     * <code>horizontalPosition</code> represents a length or a percentage. If
     *  the float doesn't contain a float value or can't be converted into the
     *  specified unit, a <code>DOMException</code> is raised.
     * @param hType  The horizontal unit.
     * @return  The float value.
     * @exception DOMException
     *    INVALID_ACCESS_ERR: Raised if the property doesn't contain a float  
     *   or the value can't be converted.
     */
    public float getHorizontalPosition(float hType)
                                       throws DOMException;

    /**
     *  This method is used to get the float value in a specified unit if the 
     * <code>verticalPosition</code> represents a length or a percentage. If 
     * the float doesn't contain a float value or can't be converted into the 
     * specified unit, a <code>DOMException</code> is raised. The value is 
     * <code>50%</code> if only the horizontal value has been specified.
     * @param vType  The vertical unit.
     * @return  The float value.
     * @exception DOMException
     *    INVALID_ACCESS_ERR: Raised if the property doesn't contain a float  
     *   or the value can't be converted.
     */
    public float getVerticalPosition(float vType)
                                     throws DOMException;

    /**
     *  This method is used to set the horizontal position with a specified 
     * unit. If the vertical value is not a percentage or a length, it sets 
     * the vertical position to <code>50%</code> . 
     * @param hType  The specified unit (a length or a percentage).
     * @param value  The new value. 
     * @exception DOMException
     *    INVALID_ACCESS_ERR: Raised if the specified unit is not a length or 
     *   a percentage.
     *   <br> NO_MODIFICATION_ALLOWED_ERR: Raised if this property is 
     *   readonly.
     */
    public void setHorizontalPosition(short hType, 
                                      float value)
                                      throws DOMException;

    /**
     *  This method is used to set the vertical position with a specified 
     * unit. If the horizontal value is not a percentage or a length, it sets 
     * the vertical position to <code>50%</code> . 
     * @param vType  The specified unit (a length or a percentage).
     * @param value  The new value. 
     * @exception DOMException
     *    INVALID_ACCESS_ERR: Raised if the specified unit is not a length or 
     *   a percentage.
     *   <br> NO_MODIFICATION_ALLOWED_ERR: Raised if this property is 
     *   readonly.
     */
    public void setVerticalPosition(short vType, 
                                    float value)
                                    throws DOMException;

    /**
     *  Sets the identifiers. If the second identifier is the empty string, 
     * the vertical identifier is set to its default value (
     * <code>"center"</code> ).
     * @param hIdentifier  The new horizontal identifier.
     * @param vIdentifier  The new vertical identifier.
     * @exception DOMException
     *    SYNTAX_ERR: Raised if the identifiers have a syntax error and are 
     *   unparsable.
     *   <br> NO_MODIFICATION_ALLOWED_ERR: Raised if this property is 
     *   readonly.
     */
    public void setPositionIdentifier(String hIdentifier, 
                                      String vIdentifier)
                                      throws DOMException;

}

