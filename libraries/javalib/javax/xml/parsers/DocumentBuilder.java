/*
 * DocumentBuilder.java
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
import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.io.IOException;
// import java.net.*;
import javax.xml.validation.Schema;
import org.w3c.dom.DOMImplementation;
import org.w3c.dom.Document;
import org.xml.sax.InputSource;
import org.xml.sax.EntityResolver;
import org.xml.sax.ErrorHandler;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;


/**
 * Uses an XML parser to construct a DOM document.
 * @author	Andrew Selkirk, David Brownell
 * @version	1.2
 */
public abstract class DocumentBuilder
{
  /** Only subclasses may use the constructor. */
  protected DocumentBuilder() { }


  //-------------------------------------------------------------
  // Methods ----------------------------------------------------
  //-------------------------------------------------------------

  public abstract DOMImplementation getDOMImplementation ();

  public abstract boolean isNamespaceAware();

  public abstract boolean isValidating();

  public abstract Document newDocument();

  // we don't demand jdk 1.2 File.toURL() in the runtime
  // keep in sync with gnu.xml.util.Resolver
  // and javax.xml.transform.stream.StreamSource
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

  /**
   * Constructs an InputSource from the file, and invokes parse ().
   * The InputSource includes the URI for the file.
   * @param file the file to parse
   * @return the DOM representation of the xml document
   * @exception IOException 
   * @exception SAXException if parse errors occur
   * @exception IllegalArgumentException if the file is null
   */
  public Document parse (File file) 
    throws SAXException, IOException
  {
    if (file==null)
      {
        throw new IllegalArgumentException("File si 'null'");
      }
    InputSource	source;

    source = new InputSource (fileToURL (file));
    source.setByteStream (new FileInputStream(file));
    return parse (source);
  }

  /**
   * 
   * @exception IllegalArgumentException if InputSource is null
   */
  public abstract Document parse(InputSource source) 
    throws SAXException, IOException;

  /**
   * Avoid using this call; provide the system ID wherever possible.
   * System IDs are essential when parsers resolve relative URIs,
   * or provide diagnostics.
   * @exception IllegalArgumentException if InputStream is null
   */
  public Document parse(InputStream stream) 
    throws SAXException, IOException
  {
    if (stream==null)
      {
        throw new IllegalArgumentException("InputStream si 'null'");
      }
    return parse(new InputSource(stream));
  } // parse()

  /**
   * 
   * @exception IllegalArgumentException if InputStream is null
   */
  public Document parse(InputStream stream, String systemID) 
    throws SAXException, IOException
  {

    if (stream==null)
      {
        throw new IllegalArgumentException("InputStream si 'null'");
      }
    // Variables
    InputSource	source;

    // Create Source
    source = new InputSource(stream);
    source.setSystemId(systemID);

    // Parse Input Source
    return parse(source);

  } // parse()

  /**
   * 
   * @exception IllegalArgumentException if the URI is null
   */
  public Document parse(String uri) 
    throws SAXException, IOException
  {
    if (uri==null)
      {
        throw new IllegalArgumentException("URI si 'null'");
      }
    return parse(new InputSource(uri));
  } // parse()

  public abstract void setEntityResolver(EntityResolver resolver);

  public abstract void setErrorHandler(ErrorHandler handler);

  // -- JAXP 1.3 methods --
  
  /**
   * Reset this document builder to its original configuration.
   * @since 1.3
   */
  public void reset()
  {
  }

  /**
   * Returns the schema in use by the XML processor.
   */
  public Schema getSchema()
  {
    return null;
  }

  /**
   * Returns the XInclude processing mode in use by the parser.
   */
  public boolean isXIncludeAware()
  {
    return false;
  }
  
}
