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

import java.io.InputStream;
import java.io.Reader;
import java.io.File;
import java.io.IOException;
import javax.xml.transform.Source;


/**
 * Stream Source
 * @author	Andrew Selkirk
 * @version	1.0
 */
public class StreamSource implements Source {

	//-------------------------------------------------------------
	// Variables --------------------------------------------------
	//-------------------------------------------------------------

	public static final String FEATURE =
		"http://javax.xml.transform.stream.StreamSource/feature";

	private String		publicId	= null;
	private String		systemId	= null;
	private InputStream	inputStream	= null;
	private Reader		reader		= null;


	//-------------------------------------------------------------
	// Initialization ---------------------------------------------
	//-------------------------------------------------------------

	public StreamSource() {
	}

	public StreamSource(File file) {
	    setSystemId (file);
	}

	public StreamSource(InputStream stream) {
		this.inputStream = stream;
	}

	public StreamSource(InputStream stream, String systemID) {
		this.inputStream = stream;
		this.systemId = systemID;
	}

	public StreamSource(Reader reader) {
		this.reader = reader;
	}

	public StreamSource(Reader reader, String systemID) {
		this.reader = reader;
		this.systemId = systemID;
	}

	public StreamSource(String systemID) {
		this.systemId = systemID;
	}


	//-------------------------------------------------------------
	// Methods ----------------------------------------------------
	//-------------------------------------------------------------

	public InputStream getInputStream() {
		return inputStream;
	}

	public String getPublicId() {
		return publicId;
	}

	public Reader getReader() {
		return reader;
	}

	public String getSystemId() {
		return systemId;
	}


	public void setInputStream(InputStream stream) {
		this.inputStream = stream;
	}

	public void setPublicId(String publicID) {
		this.publicId = publicID;
	}

	public void setReader(Reader reader) {
		this.reader = reader;
	}

	public void setSystemId(File file) {
	    try {
		this.systemId = fileToURL (file).toString ();
	    } catch (IOException e) {
		// can't happen
		throw new RuntimeException (e.getMessage ());
	    }
	}

	public void setSystemId(String systemID) {
		this.systemId = systemID;
	}

	// we don't demand jdk 1.2 File.toURL() in the runtime
	// keep in sync with gnu.xml.util.Resolver
	// and javax.xml.parsers.DocumentBuilder
	static String fileToURL (File f)
	throws IOException
	{
	    String	temp;

	    // FIXME: getAbsolutePath() seems buggy; I'm seeing components
	    // like "/foo/../" which are clearly not "absolute"
	    // and should have been resolved with the filesystem.

	    // Substituting "/" would be wrong, "foo" may have been
	    // symlinked ... the URL code will make that change
	    // later, so that things can get _really_ broken!

	    temp = f.getAbsolutePath ();

	    if (File.separatorChar != '/')
		temp = temp.replace (File.separatorChar, '/');
	    if (!temp.startsWith ("/"))
		temp = "/" + temp;
	    if (!temp.endsWith ("/") && f.isDirectory ())
		temp = temp + "/";
	    return "file:" + temp;
	}
}
