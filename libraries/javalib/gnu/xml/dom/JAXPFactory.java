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


package gnu.xml.dom;

import java.io.IOException;

import org.w3c.dom.Document;
import org.w3c.dom.DocumentType;
import org.w3c.dom.DOMImplementation;

import org.xml.sax.EntityResolver;
import org.xml.sax.ErrorHandler;
import org.xml.sax.InputSource;
import org.xml.sax.XMLReader;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParserFactory;


/**
 * DOM bootstrapping API, for use with JAXP.
 *
 * @see Consumer
 *
 * @author David Brownell
 */
public final class JAXPFactory extends DocumentBuilderFactory
{
    private static final String	PROPERTY = "http://xml.org/sax/properties/";
    private static final String	FEATURE = "http://xml.org/sax/features/";

    private SAXParserFactory	pf;

    /**
     * Default constructor.
     */
    public JAXPFactory () { }

    /**
     * Constructs a JAXP document builder which uses the default
     * JAXP SAX2 parser and the DOM implementation in this package.
     */
    public DocumentBuilder newDocumentBuilder ()
    throws ParserConfigurationException
    {
	if (pf == null) {
	    // Force use of AElfred2 since not all JAXP parsers
	    // conform very well to the SAX2 API spec ...
	    pf = new gnu.xml.aelfred2.JAXPFactory ();
	    // pf = SAXParserFactory.newInstance ();
	}

	// JAXP default: false
	pf.setValidating (isValidating ());

	// FIXME:  this namespace setup may cause errors in some
	// conformant SAX2 parsers, which we CAN patch up by
	// splicing a "NSFilter" stage up front ...

	// JAXP default: false
	pf.setNamespaceAware (isNamespaceAware ());

	try {
	    // undo rude "namespace-prefixes=false" default
	    pf.setFeature (FEATURE + "namespace-prefixes", true);

	    return new JAXPBuilder (pf.newSAXParser ().getXMLReader (), this);
	} catch (SAXException e) {
	    throw new ParserConfigurationException (
		"can't create JAXP DocumentBuilder: " + e.getMessage ());
	}
    }

    /** There seems to be no useful specification for attribute names */
    public void setAttribute (String name, Object value)
    throws IllegalArgumentException
    {
	throw new IllegalArgumentException (name);
    }

    /** There seems to be no useful specification for attribute names */
    public Object getAttribute (String name)
    throws IllegalArgumentException
    {
	throw new IllegalArgumentException (name);
    }

    static final class JAXPBuilder extends DocumentBuilder
	implements ErrorHandler
    {
	private Consumer	consumer;
	private XMLReader	producer;
	private DomImpl		impl;

	JAXPBuilder (XMLReader parser, JAXPFactory factory)
	throws ParserConfigurationException
	{
	    impl = new DomImpl ();

	    // set up consumer side
	    try {
		consumer = new Consumer ();
	    } catch (SAXException e) {
		throw new ParserConfigurationException (e.getMessage ());
	    }

	    // JAXP defaults: true, noise nodes are good (bleech)
	    consumer.setHidingReferences (
		    factory.isExpandEntityReferences ());
	    consumer.setHidingComments (
		    factory.isIgnoringComments ());
	    consumer.setHidingWhitespace (
		    factory.isIgnoringElementContentWhitespace ());
	    consumer.setHidingCDATA (
		    factory.isCoalescing ());

	    // set up producer side
	    producer = parser;
	    producer.setContentHandler (consumer.getContentHandler ());
	    producer.setDTDHandler (consumer.getDTDHandler ());

	    try {
		String	id;

		// if validating, report validity errors, and default
		// to treating them as fatal
		if (factory.isValidating ()) {
		    producer.setFeature (FEATURE + "validation",
			true);
		    producer.setErrorHandler (this);
		}

		// always save prefix info, maybe do namespace processing
		producer.setFeature (FEATURE + "namespace-prefixes",
		    true);
		producer.setFeature (FEATURE + "namespaces",
		    factory.isNamespaceAware ());

		// set important handlers
		id = PROPERTY + "lexical-handler";
		producer.setProperty (id, consumer.getProperty (id));

		id = PROPERTY + "declaration-handler";
		producer.setProperty (id, consumer.getProperty (id));

	    } catch (SAXException e) {
		throw new ParserConfigurationException (e.getMessage ());
	    }
	}


	public Document parse (InputSource source) 
	throws SAXException, IOException
	{
	    producer.parse (source);
	    return consumer.getDocument ();
	}

	public boolean isNamespaceAware ()
	{
	    try {
		return producer.getFeature (FEATURE + "namespaces");
	    } catch (SAXException e) {
		// "can't happen"
		throw new RuntimeException (e.getMessage ());
	    }
	}

	public boolean isValidating ()
	{
	    try {
		return producer.getFeature (FEATURE + "validation");
	    } catch (SAXException e) {
		// "can't happen"
		throw new RuntimeException (e.getMessage ());
	    }
	}

	public void setEntityResolver (EntityResolver resolver)
	    { producer.setEntityResolver (resolver); }

	public void setErrorHandler (ErrorHandler handler)
	{
	    producer.setErrorHandler (handler);
	    consumer.setErrorHandler (handler);
	}

	public DOMImplementation getDOMImplementation ()
	    { return impl; }

	public Document newDocument ()
	    { return new DomDocument (); }
	
	// implementation of error handler that's used when validating
	public void fatalError (SAXParseException e) throws SAXException
	    { throw e; }
	public void error (SAXParseException e) throws SAXException
	    { throw e; }
	public void warning (SAXParseException e) throws SAXException
	    { /* ignore */ }
    }
}
