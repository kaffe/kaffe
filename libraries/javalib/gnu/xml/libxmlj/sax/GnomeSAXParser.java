/*
 * GnomeSAXParser.java
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
package gnu.xml.libxmlj.sax;

import javax.xml.parsers.SAXParser;

import org.xml.sax.Parser;
import org.xml.sax.SAXException;
import org.xml.sax.SAXNotRecognizedException;
import org.xml.sax.SAXNotSupportedException;
import org.xml.sax.XMLReader;

/**
 * JAXP SAX parser implementation that uses libxml2.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class GnomeSAXParser
extends SAXParser
{

  private boolean namespaceAware;
  private boolean validating;

  /**
   * Creates a new SAX parser.
   */
  GnomeSAXParser (boolean namespaceAware, boolean validating)
  {
    this.namespaceAware = namespaceAware;
    this.validating = validating;
  }

  public Parser getParser ()
    throws SAXException
  {
    throw new SAXNotSupportedException ("SAX version 1 not supported");
  }

  public XMLReader getXMLReader ()
    throws SAXException
  {
    return new GnomeXMLReader (namespaceAware, validating);
  }

  public Object getProperty (String name)
    throws SAXNotRecognizedException, SAXNotSupportedException
  {
    GnomeXMLReader.checkPropertyName (name);
    throw new SAXNotSupportedException (name);
  }

  public void setProperty (String name, Object value)
    throws SAXNotRecognizedException, SAXNotSupportedException
  {
    GnomeXMLReader.checkPropertyName (name);
    throw new SAXNotSupportedException (name);
  }

  public boolean isNamespaceAware ()
  {
    return namespaceAware;
  }

  public boolean isValidating ()
  {
    return validating;
  }
  
}
