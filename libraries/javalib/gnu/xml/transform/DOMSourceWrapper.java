/*
 * DOMSourceWrapper.java
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

package gnu.xml.transform;

import java.io.File;
import java.io.InputStream;
import java.io.IOException;
import java.net.URL;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.ErrorListener;
import javax.xml.transform.Source;
import javax.xml.transform.TransformerException;
import javax.xml.transform.URIResolver;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.sax.SAXSource;
import javax.xml.transform.stream.StreamSource;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import org.xml.sax.InputSource;

/**
 * DOM source wrapper that parses the underlying source into a DOM tree if
 * necessary.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class DOMSourceWrapper
  extends DOMSource
{

  final Source source;
  final URIResolver uriResolver;
  final ErrorListener errorListener;

  DOMSourceWrapper(Source source, URIResolver resolver, ErrorListener listener)
  {
    this.source = source;
    uriResolver = resolver;
    errorListener = listener;
  }

  public Node getNode()
  {
    if (source instanceof DOMSource)
      {
        return ((DOMSource) source).getNode();
      }
    else
      {
        try
          {
            DocumentBuilderFactory factory =
              DocumentBuilderFactory.newInstance();
            factory.setNamespaceAware(true); // must have namespace support
            DocumentBuilder builder = factory.newDocumentBuilder();
            builder.setEntityResolver(new URIResolverEntityResolver(uriResolver));
            builder.setErrorHandler(new ErrorListenerErrorHandler(errorListener));
            if (source instanceof StreamSource)
              {
                InputStream in = ((StreamSource) source).getInputStream();
                if (in == null)
                  {
                    in = getInputStream(source.getSystemId());
                  }
                Document doc = builder.parse(in, source.getSystemId());
                in.close();
                return doc;
              }
            else if (source instanceof SAXSource)
              {
                InputSource in = ((SAXSource) source).getInputSource();
                if (in == null)
                  {
                    in = new InputSource(getInputStream(source.getSystemId()));
                  }
                return builder.parse(in);
              }
            else
              {
                errorListener.fatalError(new TransformerException("source type is unsupported"));
                return null;
              }
          }
        catch (Exception e)
          {
            // TODO handle this properly
            e.printStackTrace();
            return null;
          }
      }
  }

  InputStream getInputStream(String systemId)
    throws IOException
  {
    File cwd = new File(".");
    URL context = cwd.toURL();
    URL url = new URL(context, systemId);
    return url.openStream();
  }

  public String getSystemId()
  {
    return source.getSystemId();
  }

  public void setSystemId(String systemId)
  {
    source.setSystemId(systemId);
  }
  
}
