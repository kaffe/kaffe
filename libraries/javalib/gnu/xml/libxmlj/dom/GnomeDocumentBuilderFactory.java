/*
 * GnomeDocumentBuilderFactory.java
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

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

/**
 * Factory for JAXP document builders using the libxml2 implementation.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class GnomeDocumentBuilderFactory
extends DocumentBuilderFactory
{

  public GnomeDocumentBuilderFactory ()
  {
    setNamespaceAware (true);
  }

  public Object getAttribute (String name)
  {
    // TODO
    return null;
  }

  public DocumentBuilder newDocumentBuilder ()
    throws ParserConfigurationException
  {
    /*
    if (!isNamespaceAware ())
      {
        String msg = "Parser must be namespace-aware";
        throw new ParserConfigurationException (msg);
      }
    if (isIgnoringComments ())
      {
        String msg = "Ignoring comments not supported";
        throw new ParserConfigurationException (msg);
      }
    if (isIgnoringElementContentWhitespace ())
      {
        String msg = "Ignoring element content whitespace not supported";
        throw new ParserConfigurationException (msg);
      }
      */
    return new GnomeDocumentBuilder (isValidating (),
                                     isCoalescing (),
                                     isExpandEntityReferences ());
  }

  public void setAttribute (String name, Object value)
  {
    // TODO
  }
  
}
