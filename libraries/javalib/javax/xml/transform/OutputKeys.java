/*
 * Copyright (C) 2001 Andrew Selkirk
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
package javax.xml.transform;

/**
 * Provides names for XSLT output attributes.
 * @author	Andrew Selkirk
 * @version	1.0
 */
public class OutputKeys {

	//-------------------------------------------------------------
	// Constants --------------------------------------------------
	//-------------------------------------------------------------

	public static final String CDATA_SECTION_ELEMENTS = "cdata-section-elements";
	public static final String DOCTYPE_PUBLIC 	= "doctype-public";
	public static final String DOCTYPE_SYSTEM 	= "doctype-system";
	public static final String ENCODING		= "encoding";
	public static final String INDENT 		= "indent";
	public static final String MEDIA_TYPE 		= "media-type";
	public static final String METHOD		= "method";
	public static final String OMIT_XML_DECLARATION = "omit-xml-declaration";
	public static final String STANDALONE		= "standalone";
	public static final String VERSION		= "version";


	//-------------------------------------------------------------
	// Initialization ---------------------------------------------
	//-------------------------------------------------------------

	private OutputKeys() {
	} // OutputKeys()


} // OutputKeys

