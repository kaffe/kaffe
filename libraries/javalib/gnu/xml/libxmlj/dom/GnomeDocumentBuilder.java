/*
 * GnomeDocumentBuilder.java
 * Copyright (C) 2004 The Free Software Foundation
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
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License.
 */
package gnu.xml.libxmlj.dom;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.PushbackInputStream;
import java.net.MalformedURLException;
import java.net.URL;
import javax.xml.parsers.DocumentBuilder;

import org.w3c.dom.Document;
import org.w3c.dom.DocumentType;
import org.w3c.dom.DOMImplementation;
import org.xml.sax.EntityResolver;
import org.xml.sax.ErrorHandler;
import org.xml.sax.InputSource;
import org.xml.sax.Locator;
import org.xml.sax.SAXException;
import org.xml.sax.SAXParseException;

import gnu.xml.libxmlj.util.NamedInputStream;
import gnu.xml.libxmlj.util.StandaloneDocumentType;
import gnu.xml.libxmlj.util.StandaloneLocator;
import gnu.xml.libxmlj.util.XMLJ;

/**
 * A JAXP DOM implementation that uses Gnome libxml2 as the underlying
 * parser and node representation.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class GnomeDocumentBuilder
extends DocumentBuilder
implements DOMImplementation
{

  static
    {
      System.loadLibrary("xmlj");
    }

  // -- DocumentBuilder --

  private boolean validate;
  private boolean coalesce;
  private boolean expandEntities;
  private EntityResolver entityResolver;
  private ErrorHandler errorHandler;
  private boolean seenFatalError;

  /**
   * Constructs a new validating document builder.
   */
  public GnomeDocumentBuilder ()
    {
      this (true, false, false);
    }

  /**
   * Constructs a new document builder.
   * @param validate whether to validate during parsing
   * @param coalesce whether to merge CDATA as text nodes
   * @param expandEntities whether to expand entity references
   */
  public GnomeDocumentBuilder (boolean validate,
                               boolean coalesce,
                               boolean expandEntities)
    {
      this.validate = validate;
      this.coalesce = coalesce;
      this.expandEntities = expandEntities;
    }

  public DOMImplementation getDOMImplementation ()
    {
      return this;
    }

  public boolean isNamespaceAware ()
    {
      return true;
    }

  public boolean isValidating ()
    {
      return validate;
    }

  public Document newDocument()
    {
      return createDocument(null, null, null);
    }

  public Document parse (InputSource input)
    throws SAXException, IOException
    {
      NamedInputStream in = XMLJ.getInputStream (input);
      byte[] detectBuffer = in.getDetectBuffer ();
      String publicId = input.getPublicId();
      String systemId = input.getSystemId();
      // Handle zero-length document
      if (detectBuffer == null)
        {
          throw new SAXParseException ("No document element", publicId,
                                       systemId, 0, 0);
        }
      seenFatalError = false;
      return parseStream(in,
                         detectBuffer,
                         input.getPublicId (),
                         input.getSystemId (),
                         validate,
                         coalesce,
                         expandEntities,
                         entityResolver != null,
                         errorHandler != null);
    }

  private native Document parseStream (InputStream in,
                                       byte[] detectBuffer,
                                       String publicId,
                                       String systemId,
                                       boolean validate,
                                       boolean coalesce,
                                       boolean expandEntities,
                                       boolean entityResolver,
                                       boolean errorHandler);
  
  public void setEntityResolver (EntityResolver resolver)
    {
      entityResolver = resolver;
    }

  public void setErrorHandler (ErrorHandler handler)
    {
      errorHandler = handler;
    }

  // -- DOMImplementation --

  public boolean hasFeature (String feature, String version)
    {
      if ("XPath".equals (feature))
        {
          return ("3.0".equals (version));
        }
      // TODO
      throw new UnsupportedOperationException ();
    }

  // DOM Level 3

  public Object getFeature (String feature, String version)
    {
      // TODO
      return null;
    }

  public native Document createDocument (String namespaceURI,
                                         String qualifiedName,
                                         DocumentType doctype);

  public DocumentType createDocumentType (String qualifiedName,
                                          String publicId,
                                          String systemId)
    {
      return new StandaloneDocumentType (qualifiedName, publicId, systemId);
    }

  // Callback hooks from JNI
  
  private void setDocumentLocator (long ctx, long loc)
    {
      // ignore
    }

  private InputStream resolveEntity (String publicId, String systemId)
    throws SAXException, IOException
    {
      if (entityResolver != null)
        {
          InputSource source = entityResolver.resolveEntity(publicId,
                                                            systemId);
          return XMLJ.getInputStream(source);
        }
      else
        {
          return null;
        }
    }
  
  private void warning (String message,
                        int lineNumber,
                        int columnNumber,
                        String publicId,
                        String systemId)
    throws SAXException
    {
      if (!seenFatalError && errorHandler != null)
        {
          Locator l = new StandaloneLocator (lineNumber,
                                             columnNumber,
                                             publicId,
                                             systemId);
          errorHandler.warning (new SAXParseException (message, l));
        }
    }

  private void error (String message,
                      int lineNumber,
                      int columnNumber,
                      String publicId,
                      String systemId)
    throws SAXException
    {
      if (!seenFatalError && errorHandler != null)
        {
          Locator l = new StandaloneLocator (lineNumber,
                                             columnNumber,
                                             publicId,
                                             systemId);
          errorHandler.error (new SAXParseException (message, l));
        }
    }

  private void fatalError (String message,
                           int lineNumber,
                           int columnNumber,
                           String publicId,
                           String systemId)
    throws SAXException
    {
      if (!seenFatalError && errorHandler != null)
        {
          seenFatalError = true;
          Locator l = new StandaloneLocator (lineNumber,
                                             columnNumber,
                                             publicId,
                                             systemId);
          errorHandler.fatalError (new SAXParseException (message, l));
        }
    }

}
