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
package javax.xml.transform.sax;

// Imports
import org.xml.sax.InputSource;
import org.xml.sax.XMLFilter;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.Result;
import javax.xml.transform.Source;
import javax.xml.transform.Templates;
import javax.xml.transform.TransformerConfigurationException;

/**
 * A TransformerFactory that supports several separate modes for
 * working with SAX inputs and outputs.  Those modes include: <ul>
 *
 * <li> Pipeline Stage, pushing events through {@link TransformerHandler}
 *	objects used as SAX handlers, and passing the transformed data
 *	through a {@link SAXResult} encapsulating SAX ContentHandler and
 *	LexicalHandler objects;
 * <li> Pipeline Termination, like a normal pipeline stage but using some
 *	other kind of {@link Result} to store transformed data rather than
 *	passing it to another stage;
 * <li> Event producer, an {@link XMLFilter} object taking data from a URI
 *	or from a SAX {@link InputSource} input object and delivering it
 *	to a SAX ContentHandler;
 * <li>	Transformer objects produced by this factory will usually be able
 *	to accept {@link SAXSource} objects as inputs, and the XMLReader
 *	object in such a source could be an XMLFilter.
 * </ul>
 *
 * <p>Transformer objects produced by this factory will of course be
 * able to perform {@link Transformer#transform Transformer.transform()}
 * operations to map XML text into other text.
 *
 * <p>The factory also supports creating Templates objects.
 * 
 * @author	Andrew Selkirk, David Brownell
 * @version	1.0
 */
public abstract class SAXTransformerFactory extends TransformerFactory
{
	/**
	 * Used with <em>TransformerFactory.getFeature()</em> to determine
	 * whether the transformers it produces extend this class.
	 */
	public static final String FEATURE =
		"http://javax.xml.transform.sax.SAXTransformerFactory/feature";

	/**
	 * Used with <em>TransformerFactory.getFeature()</em> to determine
	 * whether <em>newXMLFilter()</em> methods are supported.
	 */
	public static final String FEATURE_XMLFILTER =
		"http://javax.xml.transform.sax.SAXTransformerFactory/feature/xmlfilter";


	//-------------------------------------------------------------
	// Initialization ---------------------------------------------
	//-------------------------------------------------------------

	/** Constructor, for use with subclasses */
	protected SAXTransformerFactory() {
	}


	//-------------------------------------------------------------
	// Methods ----------------------------------------------------
	//-------------------------------------------------------------

	/**
	 * Returns a SAX event consumer sending its inputs to some Result
	 * after transforming them according to a stylesheet.
	 */
	public abstract TransformerHandler
	newTransformerHandler (Source stylesheet)
		throws TransformerConfigurationException;

	/**
	 * Returns a SAX event consumer sending its inputs to some Result
	 * after transforming them according to a pre-parsed stylesheet.
	 */
	public abstract TransformerHandler
	newTransformerHandler (Templates stylesheet)
		throws TransformerConfigurationException;

	/**
	 * Returns a SAX event consumer sending its inputs to some Result
	 * without transforming them (null transformation).
	 */
	public abstract TransformerHandler newTransformerHandler()
		throws TransformerConfigurationException;

	/**
	 * Returns a SAX parser that transforms XML data according 
	 * to a stylesheet before reporting SAX events.
	 */
	public abstract XMLFilter newXMLFilter (Source stylesheet)
		throws TransformerConfigurationException;

	/**
	 * Returns a SAX parser that transforms XML data according 
	 * to a pre-parsed stylesheet before reporting SAX events.
	 */
	public abstract XMLFilter newXMLFilter (Templates stylesheet)
		throws TransformerConfigurationException;

	/**
	 * Returns a SAX event consumer collecting its inputs into
	 * a pre-parsed stylesheet.
	 */
	public abstract TemplatesHandler newTemplatesHandler()
		throws TransformerConfigurationException;

}
