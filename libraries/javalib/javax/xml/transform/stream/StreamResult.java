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
package javax.xml.transform.stream;

import java.io.OutputStream;
import java.io.Writer;
import java.io.File;
import java.io.IOException;
import javax.xml.transform.Result;


/**
 * Stream Result
 * @author	Andrew Selkirk, David Brownell
 * @version	1.0
 */
public class StreamResult implements Result {

	//-------------------------------------------------------------
	// Variables --------------------------------------------------
	//-------------------------------------------------------------

	public static final String FEATURE =
		"http://javax.xml.transform.stream.StreamResult/feature";

	private String		systemId	= null;
	private OutputStream	outputStream	= null;
	private Writer		writer		= null;


	//-------------------------------------------------------------
	// Initialization ---------------------------------------------
	//-------------------------------------------------------------

	public StreamResult() {
	}

	public StreamResult(OutputStream stream) {
		this.outputStream = stream;
	}

	public StreamResult(Writer writer) {
		this.writer = writer;
	}

	public StreamResult(String systemID) {
		this.systemId = systemID;
	}

	public StreamResult(File file) {
		setSystemId (file);
	}


	//-------------------------------------------------------------
	// Methods ----------------------------------------------------
	//-------------------------------------------------------------

	public OutputStream getOutputStream() {
		return outputStream;
	}

	public String getSystemId() {
		return systemId;
	}

	public Writer getWriter() {
		return writer;
	}

	public void setOutputStream(OutputStream stream) {
		this.outputStream = stream;
	}

	public void setWriter(Writer writer) {
		this.writer = writer;
	}

	public void setSystemId(File file) {
	    try {
		this.systemId = StreamSource.fileToURL (file).toString ();
	    } catch (IOException e) {
		// can't happen
		throw new RuntimeException (e.getMessage ());
	    }
	}

	public void setSystemId(String systemID) {
		this.systemId = systemID;
	}
}
