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
 * SAXParserFactory is used to bootstrap JAXP wrappers for
 * SAX parsers.
 *
 * <para> Note that the JAXP 1.1 spec does not specify how
 * the <em>isValidating()</em> or <em>isNamespaceAware()</em>
 * flags relate to the SAX2 feature flags controlling those
 * same features.
 *
 * @author	Andrew Selkirk, David Brownell
 * @version	1.0
 */

public abstract class SAXParserFactory {

	//-------------------------------------------------------------
	// Variables --------------------------------------------------
	//-------------------------------------------------------------

	private static final	String defaultPropName	=
		"javax.xml.parsers.SAXParserFactory";

	private 		boolean validating	= false;
	private 		boolean namespaceAware	= false;


	//-------------------------------------------------------------
	// Initialization ---------------------------------------------
	//-------------------------------------------------------------

	protected SAXParserFactory() {
	} // SAXParserFactory()


	//-------------------------------------------------------------
	// Methods ----------------------------------------------------
	//-------------------------------------------------------------

	public static SAXParserFactory newInstance() {
		try {
		    return (SAXParserFactory)
			ClassStuff.createFactory (
				defaultPropName, 
				"gnu.xml.aelfred2.JAXPFactory");
		} catch (ClassCastException e) {
			throw new FactoryConfigurationError (e,
				"Factory class is the wrong type");
		}
	}

	/**
	 * Returns a new instance of a SAXParser using the platform
	 * default implementation and the currently specified factory
	 * feature flag settings.
	 *
	 * @exception ParserConfigurationException
	 *	when the parameter combination is not supported
	 * @exception SAXNotRecognizedException
	 *	if one of the specified SAX2 feature flags is not recognized
	 * @exception SAXNotSupportedException
	 *	if one of the specified SAX2 feature flags values can
	 *	not be set, perhaps because of sequencing requirements
	 *	(which could be met by using SAX2 directly)
	 */
	public abstract SAXParser newSAXParser()
		throws ParserConfigurationException, SAXException;

	public void setNamespaceAware(boolean value) {
		namespaceAware = value;
	} // setNamespaceAware()

	public void setValidating(boolean value) {
		validating = value;
	} // setValidating()
	
	public boolean isNamespaceAware() {
		return namespaceAware;
	} // isNamespaceAware()

	public boolean isValidating() {
		return validating;
	} // isValidating()

	/**
	 * Establishes a factory parameter corresponding to the
	 * specified feature flag.
	 *
	 * @param name identifies the feature flag
	 * @param value specifies the desired flag value
	 *
	 * @exception SAXNotRecognizedException
	 *	if the specified SAX2 feature flag is not recognized
	 * @exception SAXNotSupportedException
	 *	if the specified SAX2 feature flag values can not be set,
	 *	perhaps because of sequencing requirements (which could
	 *	be met by using SAX2 directly)
	 */
	public abstract void setFeature (String name, boolean value) 
		throws	ParserConfigurationException, 
			SAXNotRecognizedException, 
			SAXNotSupportedException;

	/**
	 * Retrieves a current factory feature flag setting.
	 *
	 * @param name identifies the feature flag
	 *
	 * @exception SAXNotRecognizedException
	 *	if the specified SAX2 feature flag is not recognized
	 * @exception SAXNotSupportedException
	 *	if the specified SAX2 feature flag values can not be
	 *	accessed before parsing begins.
	 */
	public abstract boolean getFeature (String name) 
		throws	ParserConfigurationException, 
			SAXNotRecognizedException, 
			SAXNotSupportedException;

} // SAXParserFactory

