/*
 * GnomeSAXParserFactory.java
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

import java.util.Map;
import java.util.HashMap;

import javax.xml.parsers.SAXParser;
import javax.xml.parsers.SAXParserFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.xml.sax.SAXException;
import org.xml.sax.SAXNotRecognizedException;
import org.xml.sax.SAXNotSupportedException;

/**
 * JAXP SAX parser factory implementation that uses libxml2.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class GnomeSAXParserFactory
extends SAXParserFactory
{

  private Map features;

  /**
   * Creates a new SAX parser factory.
   */
  public GnomeSAXParserFactory ()
  {
    features = new HashMap ();
  }

  public SAXParser newSAXParser ()
    throws ParserConfigurationException, SAXException
  {
    // TODO features
    return new GnomeSAXParser (isNamespaceAware (), isValidating ());
  }

  public boolean getFeature (String name)
    throws ParserConfigurationException, SAXNotRecognizedException, SAXNotSupportedException
  {
    GnomeXMLReader.checkFeatureName (name);
    Boolean val = (Boolean) features.get (name);
    return (val == null) ? false : val.booleanValue ();
  }

  public void setFeature (String name, boolean flag)
    throws ParserConfigurationException, SAXNotRecognizedException, SAXNotSupportedException
  {
    GnomeXMLReader.checkFeatureName (name);
    features.put (name, flag ? Boolean.TRUE : Boolean.FALSE);
  }
  
}
