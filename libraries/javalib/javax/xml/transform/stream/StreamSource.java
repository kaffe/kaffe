/*
 * StreamSource.java
 * Copyright (C) 2001 Andrew Selkirk
 * Copyright (C) 2001 The Free Software Foundation
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
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this library with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this library, you may extend
 * this exception to your version of the library, but you are not
 * obliged to do so.  If you do not wish to do so, delete this
 * exception statement from your version. 
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
