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

package gnu.xml.aelfred2;

import java.util.Enumeration;
import java.util.Hashtable;

import org.xml.sax.Parser;
import org.xml.sax.XMLReader;
import org.xml.sax.SAXException;
import org.xml.sax.SAXNotRecognizedException;
import org.xml.sax.SAXNotSupportedException;
import org.xml.sax.helpers.XMLReaderAdapter;

import javax.xml.parsers.ParserConfigurationException;
import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;


/**
 * Configurable factory to create an &AElig;lfred2 JAXP parser; required
 * to bootstrap using JAXP.  You should use SAX2 directly where possible,
 * rather than through JAXP, since that gives you better control.
 * This class would normally be configured as a platform default factory.
 *
 * @author David Brownell
 */
public final class JAXPFactory extends SAXParserFactory
{
    private Hashtable	flags = new Hashtable ();

    /**
     * Constructs a factory which normally returns a non-validating
     * parser.
     */
    public JAXPFactory () { }

    public SAXParser newSAXParser ()
    throws ParserConfigurationException, SAXException
    {
	JaxpParser	jaxp = new JaxpParser ();
	Enumeration	e = flags.keys ();
	XMLReader	parser = jaxp.getXMLReader ();

	parser.setFeature (
		SAXDriver.FEATURE + "namespaces",
		isNamespaceAware ());
	parser.setFeature (
		SAXDriver.FEATURE + "validation",
		isValidating ());
	// that makes SAX2 feature flags trump JAXP

	while (e.hasMoreElements ()) {
	    String	uri = (String) e.nextElement ();
	    Boolean	value = (Boolean) flags.get (uri);
	    parser.setFeature (uri, value.booleanValue ());
	}

	return jaxp;
    }

    // yes, this "feature transfer" mechanism doesn't play well

    public void setFeature (String name, boolean value) 
    throws
	ParserConfigurationException,
	SAXNotRecognizedException,
	SAXNotSupportedException
    {
	try {
	    // force "early" detection of errors where possible
	    // (flags can't necessarily be set before parsing)
	    new JaxpParser ().getXMLReader ().setFeature (name, value);

	    flags.put (name, new Boolean (value));
	} catch (SAXNotRecognizedException e) {
	    throw new SAXNotRecognizedException (name);
	} catch (SAXNotSupportedException e) {
	    throw new SAXNotSupportedException (name);
	} catch (Exception e) {
	    throw new ParserConfigurationException (
		  e.getClass ().getName ()
		+ ": "
		+ e.getMessage ());
	}
    }

    public boolean getFeature (String name) 
    throws
	ParserConfigurationException,
	SAXNotRecognizedException,
	SAXNotSupportedException
    {
	Boolean	value = (Boolean) flags.get (name);
	
	if (value == null)
	    return value.booleanValue ();
	else
	    try {
		return new JaxpParser ().getXMLReader ().getFeature (name);
	    } catch (SAXNotRecognizedException e) {
		throw new SAXNotRecognizedException (name);
	    } catch (SAXNotSupportedException e) {
		throw new SAXNotSupportedException (name);
	    } catch (SAXException e) {
		throw new ParserConfigurationException (
		      e.getClass ().getName ()
		    + ": "
		    + e.getMessage ());
	    }
    }

    private static class JaxpParser extends SAXParser
    {
	private XmlReader	ae2 = new XmlReader ();
	private XMLReaderAdapter parser = null;

	JaxpParser () { }

	public void setProperty (String id, Object value) 
	throws SAXNotRecognizedException, SAXNotSupportedException
	    { ae2.setProperty (id, value); }

	public Object getProperty (String id) 
	throws SAXNotRecognizedException, SAXNotSupportedException
	    { return ae2.getProperty (id); }

	public Parser getParser ()
	throws SAXException
	{ 
	    if (parser == null)
		parser = new XMLReaderAdapter (ae2);
	    return parser;
	}

	public XMLReader getXMLReader ()
	throws SAXException
	    { return ae2; }

	public boolean isNamespaceAware ()
	{
	    try {
		return ae2.getFeature (SAXDriver.FEATURE + "namespaces");
	    } catch (Exception e) {
		throw new Error ();
	    }
	}

	public boolean isValidating ()
	{
	    try {
		return ae2.getFeature (SAXDriver.FEATURE + "validation");
	    } catch (Exception e) {
		throw new Error ();
	    }
	}
    }
}
