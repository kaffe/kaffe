/*
 * SAXParser.java
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

package javax.xml.parsers;

// Imports
import java.io.*;
import java.net.*;
import javax.xml.validation.Schema;
import org.w3c.dom.*;
import org.xml.sax.*;
import org.xml.sax.helpers.*;

/**
 * Wraps a SAX2 (or SAX1) parser.
 *
 * <p>Note that parsing with methods on this interface requires use of one
 * of the optional SAX base classes.  It's usually preferable to use the
 * SAX parser APIs directly.  SAX gives much more flexibility about how
 * application classes are organized, and about how the document entity is
 * packaged for delivery to the parser.  And JAXP doesn't otherwise provide
 * access to the SAX2 extension handlers for lexical or declaration events.
 *
 * @author	Andrew Selkirk
 * @author	David Brownell
 * @version	1.0
 */
public abstract class SAXParser
{
  /** Only subclasses may use the constructor. */
  protected SAXParser()
  {
  }


  //-------------------------------------------------------------
  // Methods ----------------------------------------------------
  //-------------------------------------------------------------

  public abstract void setProperty (String id, Object value) 
    throws SAXNotRecognizedException, SAXNotSupportedException;

  public abstract Object getProperty (String id) 
    throws SAXNotRecognizedException, SAXNotSupportedException;

  /**
   * Parse using (deprecated) SAX1 style handlers,
   * and a byte stream (with no URI).
   * Avoid using this API, since relative URIs in the document need
   * to be resolved against the document entity's URI, and good
   * diagnostics also need that URI.
   * @exception IllegalArgumentException if InputStream is null
   */
  public void parse(InputStream stream, HandlerBase handler) 
    throws SAXException, IOException
  {
    parse (new InputSource (stream), handler);
  }

  /**
   * Parse using (deprecated) SAX1 style handlers,
   * and a byte stream with a specified URI.
   * @exception IllegalArgumentException if InputStream is null
   */
  public void parse (
                     InputStream stream,
                     HandlerBase handler,
                     String systemID
                    ) throws SAXException, IOException
  {
    if(stream==null)
      {
        throw new IllegalArgumentException("InputStream is 'null'");
      }
    InputSource	source;

    // Prepare Source
    source = new InputSource(stream);
    source.setSystemId(systemID);

    parse(source, handler);

  }

  /**
   * Parse using SAX2 style handlers,
   * and a byte stream (with no URI).
   * Avoid using this API, since relative URIs in the document need
   * to be resolved against the document entity's URI, and good
   * diagnostics also need that URI.
   * @exception IllegalArgumentException if InputStream is null
   */
  public void parse(InputStream stream, DefaultHandler def) 
    throws SAXException, IOException
  {
    if(stream==null)
      {
        throw new IllegalArgumentException("InputStream is 'null'");
      }
    parse (new InputSource (stream), def);
  }

  /**
   * Parse using SAX2 style handlers,
   * and a byte stream with a specified URI.
   * @exception IllegalArgumentException if InputStream is null
   */
  public void parse (
                     InputStream stream,
                     DefaultHandler def,
                     String systemID
                    ) throws SAXException, IOException
  {
    if(stream==null)
      {
        throw new IllegalArgumentException("InputStream is 'null'");
      }
    InputSource	source;

    // Prepare Source
    source = new InputSource(stream);
    source.setSystemId(systemID);

    parse(source, def);

  }

  /**
   * Parse using (deprecated) SAX1 style handlers,
   * and a URI for the document entity.
   * @exception IllegalArgumentException if URI is null
   */
  public void parse(String uri, HandlerBase handler) 
    throws SAXException, IOException
  {
    if(uri==null)
      {
        throw new IllegalArgumentException("URI is 'null'");
      }
    parse (new InputSource (uri), handler);
  }

  /**
   * Parse using SAX2 style handlers,
   * and a URI for the document entity.
   * @exception IllegalArgumentException if URI is null
   */
  public void parse(String uri, DefaultHandler def) 
    throws SAXException, IOException
  {
    if(uri==null)
      {
        throw new IllegalArgumentException("URI is 'null'");
      }
    parse (new InputSource (uri), def);
  }

  /**
   * Parse using (deprecated) SAX1 style handlers,
   * turning a file name into the document URI.
   * @exception IllegalArgumentException if file is null
   */
  public void parse(File file, HandlerBase handler) 
    throws SAXException, IOException
  {
    if(file==null)
      {
        throw new IllegalArgumentException("The file is 'null'");
      }
    InputSource	in;

    in = new InputSource (DocumentBuilder.fileToURL (file));
    parse (in, handler);
  }

  /**
   * Parse using SAX2 style handlers,
   * turning a file name into the document URI.
   * @exception IllegalArgumentException if file is null
   */
  public void parse(File file, DefaultHandler def) 
    throws SAXException, IOException
  {
    if(file==null)
      {
        throw new IllegalArgumentException("The file is 'null'");
      }
    InputSource	in;

    in = new InputSource (DocumentBuilder.fileToURL (file));
    parse (in, def);
  }

  /**
   * Parse using (deprecated) SAX1 style handlers.
   * @exception IllegalArgumentException if InputSource is null
   */
  public void parse(InputSource source, HandlerBase handler) 
    throws SAXException, IOException
  {
    if(source==null)
      {
        throw new IllegalArgumentException("The InputSource is 'null'");
      }
    Parser	parser;

    // Prepare Parser
    parser = getParser();
    parser.setDocumentHandler(handler);
    parser.setDTDHandler(handler);
    parser.setEntityResolver(handler);
    parser.setErrorHandler(handler);

    // Parse
    parser.parse(source);

  }

  /**
   * Parse using SAX2 style handlers.
   * @exception IllegalArgumentException if InputSource is null
   */
  public void parse(InputSource source, DefaultHandler def) 
    throws SAXException, IOException
  {
    if(source==null)
      {
        throw new IllegalArgumentException("The InputSource is 'null'");
      }
    XMLReader	reader;

    // Prepare XML Reader
    reader = getXMLReader();
    reader.setContentHandler(def);
    reader.setDTDHandler(def);
    reader.setEntityResolver(def);
    reader.setErrorHandler(def);

    // NOTE:  this should NOT understand the
    // extension handlers (lexical, decl).

    reader.parse(source);
  }

  /**
   * Get a (deprecated) SAX1 driver for the underlying parser.
   */
  public abstract Parser getParser() throws SAXException;

  /**
   * Get a SAX2 driver for the underlying parser.
   * @since 1.1
   */
  public abstract XMLReader getXMLReader() throws SAXException;

  public abstract boolean isNamespaceAware();

  public abstract boolean isValidating();

  // -- JAXP 1.3 methods --

  /**
   * Resets this parser to its original configuration.
   * @since 1.3
   */
  public void reset()
  {
  }

  /**
   * Returns the schema in use by this parser.
   * @since 1.3
   */
  public Schema getSchema()
  {
    return null;
  }

  /**
   * Indicates whether this parser is XInclude-aware.
   * @since 1.3
   */
  public boolean isXIncludeAware()
  {
    return false;
  }
  
}
