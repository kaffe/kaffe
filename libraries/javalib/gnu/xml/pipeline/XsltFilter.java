/*
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

package gnu.xml.pipeline;

import java.io.IOException;

import javax.xml.transform.TransformerFactory;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.sax.*;
import javax.xml.transform.stream.StreamSource;

import org.xml.sax.ContentHandler;
import org.xml.sax.SAXException;
import org.xml.sax.ext.LexicalHandler;


/**
 * Packages an XSLT transform as a pipeline component. 
 * Note that all DTD events (callbacks to DeclHandler and DTDHandler 
 * interfaces) are discarded, although XSLT transforms may be set up to
 * use the LexicalHandler to write DTDs with only an external subset.
 * Not every XSLT engine will necessarily be usable with this filter,
 * but current versions of
 * <a href="http://saxon.sourceforge.net">SAXON</a> and
 * <a href="http://xml.apache.org/xalan-j">Xalan</a> should work well.
 *
 * @see TransformerFactory
 *
 * @author David Brownell
 */
final public class XsltFilter extends EventFilter
{
    /**
     * Creates a filter that performs the specified transform.
     * Uses the JAXP 1.1 interfaces to access the default XSLT
     * engine configured for in the current execution context,
     * and parses the stylesheet without custom EntityResolver
     * or ErrorHandler support.
     *
     * @param stylesheet URI for the stylesheet specifying the
     *	XSLT transform
     * @param next provides the ContentHandler and LexicalHandler
     *	to receive XSLT output.
     * @exception SAXException if the stylesheet can't be parsed
     * @exception IOException if there are difficulties
     *	bootstrapping the XSLT engine, such as it not supporting
     *	SAX well enough to use this way.
     */
    public XsltFilter (String stylesheet, EventConsumer next)
    throws SAXException, IOException
    {
	// First, get a transformer with the stylesheet preloaded
	TransformerFactory	tf = null;
	TransformerHandler	th;

	try {
	    SAXTransformerFactory	stf;

	    tf = TransformerFactory.newInstance ();
	    if (!tf.getFeature (SAXTransformerFactory.FEATURE)	// sax inputs
		    || !tf.getFeature (SAXResult.FEATURE)	// sax outputs
		    || !tf.getFeature (StreamSource.FEATURE)	// stylesheet
		    )
		throw new IOException ("XSLT factory ("
		    + tf.getClass ().getName ()
		    + ") does not support SAX");
	    stf = (SAXTransformerFactory) tf;
	    th = stf.newTransformerHandler (new StreamSource (stylesheet));
	} catch (TransformerConfigurationException e) {
	    throw new IOException ("XSLT factory ("
		+ (tf == null
			? "none available"
			: tf.getClass ().getName ())
		+ ") configuration error, "
		+ e.getMessage ()
		);
	}

	// Hook its outputs up to the pipeline ...
	SAXResult		out = new SAXResult ();

	out.setHandler (next.getContentHandler ());
	try {
	    LexicalHandler	lh;
	    lh = (LexicalHandler) next.getProperty (LEXICAL_HANDLER);
	    out.setLexicalHandler (lh);
	} catch (Exception e) {
	    // ignore
	}
	th.setResult (out);

	// ... and make sure its inputs look like ours.
	setContentHandler (th);
	setProperty (LEXICAL_HANDLER, th);
    }
}
