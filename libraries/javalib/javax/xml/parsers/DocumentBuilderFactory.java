/*
 * Copyright (C) 2001 Andrew Selkirk
 * Copyright (C) 2001 David Brownell
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

package javax.xml.parsers;

// Imports
import java.io.IOException;
import java.io.InputStream;
import java.io.File;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Properties;
import org.w3c.dom.*;
import org.xml.sax.*;

/**
 * DocumentBuilderFactory is used to resolve the problem that the
 * W3C DOM APIs don't include portable bootstrapping.
 *
 * @author	Andrew Selkirk, David Brownell
 * @version	1.2
 */
public abstract class DocumentBuilderFactory {

	//-------------------------------------------------------------
	// Variables --------------------------------------------------
	//-------------------------------------------------------------

	private static final	String defaultPropName	= 
		"javax.xml.parsers.DocumentBuilderFactory";

	private 		boolean validating	= false;
	private 		boolean namespaceAware	= false;
	private 		boolean whitespace	= false;
	private 		boolean expandEntityRef	= false;
	private 		boolean ignoreComments	= false;
	private 		boolean coalescing	= false;


	//-------------------------------------------------------------
	// Initialization ---------------------------------------------
	//-------------------------------------------------------------

	protected DocumentBuilderFactory() {
	} // DocumentBuilderFactory()


	//-------------------------------------------------------------
	// Methods ----------------------------------------------------
	//-------------------------------------------------------------

  /**
   * @exception IllegalArgumentException if implementation doesn't recognize the attribute
   */
	public abstract Object getAttribute(String name) 
		throws IllegalArgumentException;

	public boolean isCoalescing() {
		return coalescing;
	} // isCoalescing()

	public boolean isExpandEntityReferences() {
		return expandEntityRef;
	} // isExpandEntityReferences()

	public boolean isIgnoringComments() {
		return ignoreComments;
	} // isIgnoringComments()

	public boolean isIgnoringElementContentWhitespace() {
		return whitespace;
	} // isIgnoringElementContentWhitespace()

	public boolean isNamespaceAware() {
		return namespaceAware;
	} // isNamespaceAware()

	public boolean isValidating() {
		return validating;
	} // isValidating()

	public abstract DocumentBuilder newDocumentBuilder()
		throws ParserConfigurationException;

  /**
   * @exception FactoryConfigurationError if the implementation is not available
   */
	public static DocumentBuilderFactory newInstance() {
		try {
		    return (DocumentBuilderFactory)
			ClassStuff.createFactory (
				defaultPropName, 
				"gnu.xml.dom.JAXPFactory");
		} catch (ClassCastException e) {
			throw new FactoryConfigurationError (e,
				"Factory class is the wrong type");
		}
	}

  /**
   * @exception IllegalArgumentException if implementation doesn't recognize the attribute
   */
	public abstract void setAttribute(String name, Object value) 
		throws IllegalArgumentException;

	public void setCoalescing(boolean value) {
		coalescing = value;
	} // setCoalescing()

	public void setExpandEntityReferences(boolean value) {
		expandEntityRef = value;
	} // setExpandEntityReferences()

	public void setIgnoringComments(boolean value) {
		ignoreComments = value;
	} // setIgnoringComments()

	public void setIgnoringElementContentWhitespace(boolean value) {
		whitespace = value;
	} // setIgnoringElementContentWhitespace()

	public void setNamespaceAware(boolean value) {
		namespaceAware = value;
	} // setNamespaceAware()

	public void setValidating(boolean value) {
		validating = value;
	} // setValidating()
}
