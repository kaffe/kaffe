/* SAXParser.java -- 
   Copyright (C) 2005  Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

package gnu.xml.stream;

import java.io.InputStream;
import java.io.IOException;
import java.io.Reader;
import java.net.URL;
import java.util.Iterator;
import java.util.Map;
import javax.xml.namespace.QName;
import javax.xml.stream.Location;
import javax.xml.stream.XMLEventReader;
import javax.xml.stream.XMLReporter;
import javax.xml.stream.XMLResolver;
import javax.xml.stream.XMLStreamConstants;
import javax.xml.stream.XMLStreamException;
import javax.xml.stream.XMLStreamReader;

import org.xml.sax.ContentHandler;
import org.xml.sax.DTDHandler;
import org.xml.sax.EntityResolver;
import org.xml.sax.ErrorHandler;
import org.xml.sax.InputSource;
import org.xml.sax.Parser;
import org.xml.sax.SAXException;
import org.xml.sax.SAXNotRecognizedException;
import org.xml.sax.SAXNotSupportedException;
import org.xml.sax.SAXParseException;
import org.xml.sax.XMLReader;
import org.xml.sax.ext.Attributes2;
import org.xml.sax.ext.DeclHandler;
import org.xml.sax.ext.EntityResolver2;
import org.xml.sax.ext.LexicalHandler;
import org.xml.sax.ext.Locator2;

/**
 * JAXP SAX parser using an underlying StAX parser.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class SAXParser
  extends javax.xml.parsers.SAXParser
  implements XMLReader, Attributes2, Locator2, XMLResolver, XMLReporter
{

  ContentHandler contentHandler;
  DeclHandler declHandler;
  DTDHandler dtdHandler;
  EntityResolver entityResolver;
  ErrorHandler errorHandler;
  LexicalHandler lexicalHandler;

  boolean validating;
  boolean namespaceAware;
  boolean xIncludeAware;
  boolean stringInterning = true;

  XMLParser parser;
  String encoding;
  String xmlVersion;
  boolean xmlStandalone;

  SAXParser(boolean validating, boolean namespaceAware, boolean xIncludeAware)
  {
    this.validating = validating;
    this.namespaceAware = namespaceAware;
    this.xIncludeAware = xIncludeAware;
  }

  // -- SAXParser --
  
  public Parser getParser()
    throws SAXException
  {
    return null;
  }
  
  public XMLReader getXMLReader()
    throws SAXException
  {
    return this;
  }

  public boolean isNamespaceAware()
  {
    return namespaceAware;
  }

  public boolean isValidating()
  {
    return validating;
  }

  public void setProperty(String name, Object value)
    throws SAXNotRecognizedException, SAXNotSupportedException
  {
    if (parser != null)
      throw new IllegalStateException("parsing in progress");
    String FEATURES = "http://xml.org/sax/features/";
    String PROPERTIES = "http://xml.org/sax/properties/";
    if ((FEATURES + "namespaces").equals(name))
      namespaceAware = Boolean.TRUE.equals(value);
    if ((FEATURES + "string-interning").equals(name))
      stringInterning = Boolean.TRUE.equals(value);
    if ((FEATURES + "validation").equals(name))
      validating = Boolean.TRUE.equals(value);
    if ((PROPERTIES + "declaration-handler").equals(name))
      declHandler = (DeclHandler) value;
    if ((PROPERTIES + "lexical-handler").equals(name))
      lexicalHandler = (LexicalHandler) value;
    throw new SAXNotSupportedException(name);
  }

  public Object getProperty(String name)
    throws SAXNotRecognizedException, SAXNotSupportedException
  {
    String FEATURES = "http://xml.org/sax/features/";
    String PROPERTIES = "http://xml.org/sax/properties/";
    if ((FEATURES + "is-standalone").equals(name))
      return xmlStandalone ? Boolean.TRUE : Boolean.FALSE;
    if ((FEATURES + "namespaces").equals(name))
      return namespaceAware ? Boolean.TRUE : Boolean.FALSE;
    if ((FEATURES + "string-interning").equals(name))
      return stringInterning ? Boolean.TRUE : Boolean.FALSE;
    if ((FEATURES + "use-attributes2").equals(name))
      return Boolean.TRUE;
    if ((FEATURES + "use-locator2").equals(name))
      return Boolean.TRUE;
    if ((FEATURES + "use-entity-resolver2").equals(name))
      return Boolean.FALSE;
    if ((FEATURES + "validation").equals(name))
      return validating ? Boolean.TRUE : Boolean.FALSE;
    if ((FEATURES + "xml-1.1").equals(name))
      return Boolean.TRUE;
    if ((PROPERTIES + "declaration-handler").equals(name))
      return declHandler;
    if ((PROPERTIES + "document-xml-version").equals(name))
      return xmlVersion;
    if ((PROPERTIES + "lexical-handler").equals(name))
      return lexicalHandler;
    throw new SAXNotSupportedException(name);
  }

  public boolean isXIncludeAware()
  {
    return xIncludeAware;
  }

  public void reset()
  {
    parser = null;
    encoding = null;
    xmlVersion = null;
  }

  // -- XMLReader --

  public boolean getFeature(String name)
    throws SAXNotRecognizedException, SAXNotSupportedException
  {
    Object ret = getProperty(name);
    if (ret instanceof Boolean)
      return ((Boolean) ret).booleanValue();
    throw new SAXNotSupportedException(name);
  }

  public void setFeature(String name, boolean value)
    throws SAXNotRecognizedException, SAXNotSupportedException
  {
    setProperty(name, value ? Boolean.TRUE : Boolean.FALSE);
  }

  public void setEntityResolver(EntityResolver resolver)
  {
    entityResolver = resolver;
  }

  public EntityResolver getEntityResolver()
  {
    return entityResolver;
  }

  public void setDTDHandler(DTDHandler handler)
  {
    dtdHandler = handler;
  }

  public DTDHandler getDTDHandler()
  {
    return dtdHandler;
  }

  public void setContentHandler(ContentHandler handler)
  {
    contentHandler = handler;
  }

  public ContentHandler getContentHandler()
  {
    return contentHandler;
  }

  public void setErrorHandler(ErrorHandler handler)
  {
    errorHandler = handler;
  }

  public ErrorHandler getErrorHandler()
  {
    return errorHandler;
  }

  public synchronized void parse(InputSource input)
    throws IOException, SAXException
  {
    reset();
    String systemId = input.getSystemId();
    InputStream in = input.getByteStream();
    if (in != null)
      parser = new XMLParser(in, systemId);
    else
      {
        Reader reader = input.getCharacterStream();
        if (reader != null)
          parser = new XMLParser(reader, systemId);
      }
    if (parser == null)
      {
        if (systemId == null)
          throw new SAXException("No stream or system ID specified");
        systemId = XMLParser.absolutize(null, systemId);
        in = new URL(systemId).openStream();
        parser = new XMLParser(in, systemId);
      }
    
    parser.setValidating(validating);
    parser.setNamespaceAware(namespaceAware);
    parser.setXIncludeAware(xIncludeAware);
    parser.setStringInterning(stringInterning);
    parser.setResolver(this);
    parser.setReporter(this);
    
    if (contentHandler != null)
      contentHandler.setDocumentLocator(this);
    try
      {
        while (parser.hasNext())
          {
            switch (parser.next())
              {
              case XMLStreamConstants.CHARACTERS:
                if (contentHandler != null)
                  {
                    char[] b = parser.getTextCharacters();
                    contentHandler.characters(b, 0, b.length);
                  }
                break;
              case XMLStreamConstants.SPACE:
                if (contentHandler != null)
                  {
                    char[] b = parser.getTextCharacters();
                    // TODO determine whether whitespace is ignorable
                    contentHandler.characters(b, 0, b.length);
                  }
                break;
              case XMLStreamConstants.CDATA:
                if (lexicalHandler != null)
                  lexicalHandler.startCDATA();
                if (contentHandler != null)
                  {
                    char[] b = parser.getTextCharacters();
                    // TODO determine whether whitespace and ignorable
                    contentHandler.characters(b, 0, b.length);
                  }
                if (lexicalHandler != null)
                  lexicalHandler.endCDATA();
                break;
              case XMLStreamConstants.START_ELEMENT:
                if (contentHandler != null)
                  {
                    QName name = parser.getName();
                    String uri = name.getNamespaceURI();
                    String localName = name.getLocalPart();
                    String prefix = name.getPrefix();
                    String qName = localName;
                    if (!"".equals(prefix))
                      qName = prefix + ":" + localName;
                    if (!namespaceAware)
                      {
                        uri = "";
                        localName = "";
                      }
                    contentHandler.startElement(uri, localName, qName, this);
                  }
                break;
              case XMLStreamConstants.END_ELEMENT:
                if (contentHandler != null)
                  {
                    QName name = parser.getName();
                    String uri = name.getNamespaceURI();
                    String localName = name.getLocalPart();
                    String prefix = name.getPrefix();
                    String qName = localName;
                    if (!"".equals(prefix))
                      qName = prefix + ":" + localName;
                    if (!namespaceAware)
                      {
                        uri = "";
                        localName = "";
                      }
                    contentHandler.endElement(uri, localName, qName);
                  }
                break;
              case XMLStreamConstants.COMMENT:
                if (lexicalHandler != null)
                  {
                    char[] b = parser.getTextCharacters();
                    lexicalHandler.comment(b, 0, b.length);
                  }
                break;
              case XMLStreamConstants.PROCESSING_INSTRUCTION:
                if (contentHandler != null)
                  {
                    String target = parser.getPITarget();
                    String data = parser.getPIData();
                    contentHandler.processingInstruction(target, data);
                  }
                break;
              case XMLStreamConstants.START_DOCUMENT:
                encoding = parser.getEncoding();
                xmlVersion = parser.getVersion();
                xmlStandalone = parser.isStandalone();
                if (contentHandler != null)
                  contentHandler.startDocument();
                break;
              case XMLStreamConstants.END_DOCUMENT:
                if (contentHandler != null)
                  contentHandler.endDocument();
                break;
              case XMLStreamConstants.DTD:
                if (lexicalHandler != null)
                  {
                    String rootName = parser.doctype.rootName;
                    String publicId = parser.doctype.publicId;
                    String systemId2 = parser.doctype.systemId;
                    lexicalHandler.startDTD(rootName, publicId, systemId2);
                  }
                if (declHandler != null)
                  {
                    for (Iterator i = parser.doctype.elements.entrySet().iterator();
                         i.hasNext(); )
                      {
                        Map.Entry entry = (Map.Entry) i.next();
                        String name = (String) entry.getKey();
                        String model = (String) entry.getValue();
                        declHandler.elementDecl(name, model);
                      }
                    for (Iterator i = parser.doctype.attlists.entrySet().iterator();
                         i.hasNext(); )
                      {
                        Map.Entry entry = (Map.Entry) i.next();
                        String elementName = (String) entry.getKey();
                        Map attlist = (Map) entry.getValue();
                        for (Iterator j = attlist.entrySet().iterator();
                             j.hasNext(); )
                          {
                            Map.Entry att = (Map.Entry) j.next();
                            String name = (String) att.getKey();
                            XMLParser.AttributeDecl decl =
                              (XMLParser.AttributeDecl) att.getValue();
                            String type = decl.type;
                            String value = decl.value;
                            String mode = null;
                            switch (decl.valueType)
                              {
                              case XMLParser.ATTRIBUTE_DEFAULT_FIXED:
                                mode = "#FIXED";
                                break;
                              case XMLParser.ATTRIBUTE_DEFAULT_REQUIRED:
                                mode = "#REQUIRED";
                                break;
                              case XMLParser.ATTRIBUTE_DEFAULT_IMPLIED:
                                mode = "#IMPLIED";
                                break;
                              }
                            declHandler.attributeDecl(elementName, name,
                                                      type, mode, value);
                          }
                      }
                    // TODO entity declarations
                  }
                if (lexicalHandler != null)
                  lexicalHandler.endDTD();
              }
          }
        reset();
      }
    catch (XMLStreamException e)
      {
        SAXParseException e2 = new SAXParseException(e.getMessage(), this);
        e2.initCause(e);
        throw e2;
      }
  }

  public void parse(String systemId)
    throws IOException, SAXException
  {
    parse(new InputSource(systemId));
  }

  // -- Attributes2 --

  public int getIndex(String qName)
  {
    int len = parser.getAttributeCount();
    for (int i = 0; i < len; i++)
      {
        QName q = parser.getAttributeQName(i);
        String localName = q.getLocalPart();
        String prefix = q.getPrefix();
        String qn = ("".equals(prefix)) ? localName : prefix + ":" + localName;
        if (qName.equals(qn))
          return i;
      }
    return -1;
  }

  public int getIndex(String uri, String localName)
  {
    int len = parser.getAttributeCount();
    for (int i = 0; i < len; i++)
      {
        QName q = parser.getAttributeQName(i);
        String ln = q.getLocalPart();
        String u = q.getNamespaceURI();
        if (u == null && uri != null)
          continue;
        if (u != null && !u.equals(uri))
          continue;
        if (ln.equals(localName))
          return i;
      }
    return -1;
  }

  public int getLength()
  {
    return parser.getAttributeCount();
  }

  public String getLocalName(int index)
  {
    return parser.getAttributeName(index);
  }

  public String getQName(int index)
  {
    QName q = parser.getAttributeQName(index);
    String localName = q.getLocalPart();
    String prefix = q.getPrefix();
    return ("".equals(prefix)) ? localName : prefix + ":" + localName;
  }

  public String getType(int index)
  {
    return parser.getAttributeType(index);
  }

  public String getType(String qName)
  {
    int index = getIndex(qName);
    return (index == -1) ? null : getType(index);
  }

  public String getType(String uri, String localName)
  {
    int index = getIndex(uri, localName);
    return (index == -1) ? null : getType(index);
  }

  public String getURI(int index)
  {
    return parser.getAttributeNamespace(index);
  }

  public String getValue(int index)
  {
    return parser.getAttributeValue(index);
  }

  public String getValue(String qName)
  {
    int index = getIndex(qName);
    return (index == -1) ? null : getValue(index);
  }

  public String getValue(String uri, String localName)
  {
    int index = getIndex(uri, localName);
    return (index == -1) ? null : getValue(index);
  }

  public boolean isDeclared(int index)
  {
    return parser.isAttributeDeclared(index);
  }

  public boolean isDeclared(String qName)
  {
    int index = getIndex(qName);
    return (index == -1) ? false : isDeclared(index);
  }

  public boolean isDeclared(String uri, String localName)
  {
    int index = getIndex(uri, localName);
    return (index == -1) ? false : isDeclared(index);
  }

  public boolean isSpecified(int index)
  {
    return parser.isAttributeSpecified(index);
  }

  public boolean isSpecified(String qName)
  {
    int index = getIndex(qName);
    return (index == -1) ? false : isSpecified(index);
  }

  public boolean isSpecified(String uri, String localName)
  {
    int index = getIndex(uri, localName);
    return (index == -1) ? false : isSpecified(index);
  }

  // -- Locator2 --
  
  public int getColumnNumber()
  {
    return parser.getColumnNumber();
  }

  public int getLineNumber()
  {
    return parser.getLineNumber();
  }

  public String getPublicId()
  {
    return null;
  }

  public String getSystemId()
  {
    return parser.getLocationURI();
  }
  
  public String getEncoding()
  {
    return encoding;
  }

  public String getXMLVersion()
  {
    return xmlVersion;
  }

  // -- XMLResolver --

  public InputStream resolve(String uri)
    throws XMLStreamException
  {
    if (entityResolver != null)
      {
        try
          {
            InputSource input = entityResolver.resolveEntity(null, uri);
            if (input != null)
              return input.getByteStream();
          }
        catch (SAXException e)
          {
            XMLStreamException e2 = new XMLStreamException(e.getMessage());
            e2.initCause(e);
            throw e2;
          }
        catch (IOException e)
          {
            XMLStreamException e2 = new XMLStreamException(e.getMessage());
            e2.initCause(e);
            throw e2;
          }
      }
    return null;
  }

  public XMLEventReader resolveAsXMLEventReader(String uri)
    throws XMLStreamException
  {
    // unused
    return null;
  }

  public XMLStreamReader resolveAsXMLStreamReader(String uri)
    throws XMLStreamException
  {
    // unused
    return null;
  }

  // -- XMLReporter --

  public void report(String message, String errorType,
                     Object relatedInformation, Location location)
    throws XMLStreamException
  {
    if (errorHandler != null)
      {
        try
          {
            errorHandler.warning(new SAXParseException(message, this));
          }
        catch (SAXException e)
          {
            XMLStreamException e2 = new XMLStreamException(e.getMessage());
            e2.initCause(e);
            throw e2;
          }
      }
  }
  
}
