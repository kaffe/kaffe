/*
 * URIResolverEntityResolver.java
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
package gnu.xml.libxmlj.transform;

import java.io.IOException;
import javax.xml.transform.URIResolver;
import javax.xml.transform.TransformerException;
import javax.xml.transform.sax.SAXSource;
import org.xml.sax.EntityResolver;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

/**
 * Provides an EntityResolver interface to a URIResolver.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class URIResolverEntityResolver
implements EntityResolver
{

  private URIResolver resolver;

  URIResolverEntityResolver (URIResolver resolver)
  {
    this.resolver = resolver;
  }

  public InputSource resolveEntity (String publicId, String systemId)
    throws SAXException, IOException
  {
    try
      {
        return SAXSource.sourceToInputSource (resolver.resolve (systemId,
                                                                null));
      }
    catch (TransformerException e)
      {
        Throwable cause = e.getCause ();
        if (cause instanceof SAXException)
          {
            throw (SAXException) cause;
          }
        else if (cause instanceof IOException)
          {
            throw (IOException) cause;
          }
        throw new SAXException (e);
      }
  }

}
