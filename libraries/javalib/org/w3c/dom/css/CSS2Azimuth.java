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
 *  The <code>CSS2Azimuth</code> interface represents the  azimuth CSS Level 2 
 * property.
 * <p> For this extension of the <code>CSSValue</code> interface, the 
 * <code>valueType</code> attribute of the underlying <code>CSSValue</code> 
 * interface shall be <code>CSS_CUSTOM</code> . 
 * <p>See also the <a href='http://www.w3.org/TR/2000/CR-DOM-Level-2-20000510'>Document Object Model (DOM) Level 2 Specification</a>.
 * @since DOM Level 2
 */
public interface CSS2Azimuth extends CSSValue {
    /**
     *  A code defining the type of the value as defined in 
     * <code>CSSValue</code> . It would be one of <code>CSS_DEG</code> , 
     * <code>CSS_RAD</code> , <code>CSS_GRAD</code> or <code>CSS_IDENT</code> 
     * .
     */
    public short getAzimuthType();

    /**
     *  If <code>azimuthType</code> is <code>CSS_IDENT</code> , 
     * <code>identifier</code> contains one of left-side, far-left, left, 
     * center-left, center, center-right, right, far-right, right-side, 
     * leftwards, rightwards. The empty string if none is set.
     */
    public String getIdentifier();

    /**
     * <code>behind</code> indicates whether the behind identifier has been 
     * set.
     */
    public boolean getBehind();

    /**
     *  A method to set the angle value with a specified unit. This method 
     * will unset any previously set identifier value.
     * @param uType  The unitType could only be one of <code>CSS_DEG</code> , 
     *   <code>CSS_RAD</code> or <code>CSS_GRAD</code> ).
     * @param fValue  The new float value of the angle.
     * @exception DOMException
     *    INVALID_ACCESS_ERR: Raised if the unit type is invalid.
     *   <br> NO_MODIFICATION_ALLOWED_ERR: Raised if this property is 
     *   readonly.
     */
    public void setAngleValue(short uType, 
                              float fValue)
                              throws DOMException;

    /**
     *  Used to retrieved the float value of the azimuth property.
     * @param uType  The unit type can be only an angle unit type (
     *   <code>CSS_DEG</code> , <code>CSS_RAD</code> or <code>CSS_GRAD</code> 
     *   ).
     * @return  The float value.
     * @exception DOMException
     *    INVALID_ACCESS_ERR: Raised if the unit type is invalid.
     */
    public float getAngleValue(short uType)
                               throws DOMException;

    /**
     *  Setting the identifier for the azimuth property will unset any 
     * previously set angle value. The value of <code>azimuthType</code> is 
     * set to <code>CSS_IDENT</code>
     * @param ident  The new identifier. If the identifier is "leftwards" or 
     *   "rightward", the behind attribute is ignored.
     * @param b  The new value for behind.
     * @exception DOMException
     *    SYNTAX_ERR: Raised if the specified <code>identifier</code> has a 
     *   syntax error and is unparsable.
     *   <br> NO_MODIFICATION_ALLOWED_ERR: Raised if this property is 
     *   readonly.
     */
    public void setIdentifier(String ident, 
                              boolean b)
                              throws DOMException;

}

