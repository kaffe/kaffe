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
package javax.xml.transform.sax;

// Imports
import org.xml.sax.ContentHandler;
import org.xml.sax.ext.LexicalHandler;
import javax.xml.transform.Result;

/**
 * Collects the result of a SAX transform.
 * @author	Andrew Selkirk, David Brownell
 * @version	1.0
 */
public class SAXResult implements Result
{
	/**
	 * Used with <em>TransformerFactory.getFeature()</em> to determine
	 * whether the transformers it produces support SAXResult objects
	 * as outputs.
	 */
	public static final String FEATURE =
		"http://javax.xml.transform.sax.SAXResult/feature";

	private ContentHandler	handler		= null;
	private LexicalHandler	lexhandler	= null;
	private String		systemId	= null;


	//-------------------------------------------------------------
	// Initialization ---------------------------------------------
	//-------------------------------------------------------------

	public SAXResult() {
	} // SAXResult()

	public SAXResult(ContentHandler handler) {
		this.handler = handler;
	} // SAXResult()


	//-------------------------------------------------------------
	// Methods ----------------------------------------------------
	//-------------------------------------------------------------

	public ContentHandler getHandler() {
		return handler;
	}

	public String getSystemId() {
		return systemId;
	}

	public LexicalHandler getLexicalHandler() {
		return lexhandler;
	}


	public void setHandler(ContentHandler handler) {
		this.handler = handler;
	}

	public void setSystemId(String systemID) {
		this.systemId = systemID;
	}

	public void setLexicalHandler(LexicalHandler lexHandler) {
		this.lexhandler = lexHandler;
	}
}
