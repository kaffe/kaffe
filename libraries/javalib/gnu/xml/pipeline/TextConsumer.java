/*
 * Copyright (C) 1999-2001 David Brownell
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

package gnu.xml.pipeline;

import java.io.*;

import org.xml.sax.*;

import gnu.xml.util.XMLWriter;


/**
 * Terminates a pipeline, consuming events to print them as well formed
 * XML (or XHTML) text.
 *
 * <p> Input must be well formed, and must include XML names (e.g. the
 * prefixes and prefix declarations must be present), or the output of
 * this class is undefined.
 *
 * @see NSFilter
 * @see WellFormednessFilter
 *
 * @author David Brownell
 */
public class TextConsumer extends XMLWriter implements EventConsumer
{
    /**
     * Constructs an event consumer which echoes its input as text,
     * optionally adhering to some basic XHTML formatting options
     * which increase interoperability with old (v3) browsers.
     *
     * <p> For the best interoperability, when writing as XHTML only
     * ASCII characters are emitted; other characters are turned to
     * entity or character references as needed, and no XML declaration
     * is provided in the document.
     */
    public TextConsumer (Writer w, boolean isXhtml)
    throws IOException
    {
	super (w, isXhtml ? "US-ASCII" : null);
	setXhtml (isXhtml);
    }

    /**
     * Constructs a consumer that writes its input as XML text.
     * XHTML rules are not followed.
     */
    public TextConsumer (Writer w)
    throws IOException
    {
	this (w, false);
    }
	
    /**
     * Constructs a consumer that writes its input as XML text,
     * encoded in UTF-8.  XHTML rules are not followed.
     */
    public TextConsumer (OutputStream out)
    throws IOException
    {
	this (new OutputStreamWriter (out, "UTF8"), false);
    }

    /** <b>EventConsumer</b> Returns the document handler being used. */
    public ContentHandler getContentHandler ()
	{ return this; }

    /** <b>EventConsumer</b> Returns the dtd handler being used. */
    public DTDHandler getDTDHandler ()
	{ return this; }

    /** <b>XMLReader</b>Retrieves a property (lexical and decl handlers) */
    public Object getProperty (String propertyId)
    throws SAXNotRecognizedException
    {
	if (EventFilter.LEXICAL_HANDLER.equals (propertyId))
	    return this;
	if (EventFilter.DECL_HANDLER.equals (propertyId))
	    return this;
	throw new SAXNotRecognizedException (propertyId);
    }
}
