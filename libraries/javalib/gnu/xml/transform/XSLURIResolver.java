/*
 * XSLURIResolver.java
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
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLConnection;
import java.util.HashMap;
import java.util.Map;
import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.transform.ErrorListener;
import javax.xml.transform.Source;
import javax.xml.transform.TransformerException;
import javax.xml.transform.URIResolver;
import javax.xml.transform.dom.DOMSource;
import org.w3c.dom.Node;
import org.xml.sax.InputSource;
import org.xml.sax.SAXException;

/**
 * URI resolver for XSLT.
 * This resolver parses external entities into DOMSources. It
 * maintains a cache of URIs to DOMSources to avoid expensive re-parsing.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class XSLURIResolver
  implements URIResolver
{

  Map lastModifiedCache = new HashMap();
  Map nodeCache = new HashMap();
  DocumentBuilder builder;
  URIResolver userResolver;
  ErrorListener userListener;

  void setUserResolver(URIResolver userResolver)
  {
    this.userResolver = userResolver;
  }

  void setUserListener(ErrorListener userListener)
  {
    this.userListener = userListener;
  }

  /**
   * Clear the cache.
   */
  void flush()
  {
    lastModifiedCache.clear();
    nodeCache.clear();
  }

  public Source resolve(String href, String base)
    throws TransformerException
  {
    Source source = null;
    if (userResolver != null)
      {
        source = userResolver.resolve(base, href);
      }
    return resolveDOM(source, href, base);
  }

  DOMSource resolveDOM(Source source, String base, String href)
    throws TransformerException
  {
    if (source != null && source instanceof DOMSource)
      {
        return (DOMSource) source;
      }
    String systemId = (source == null) ? null : source.getSystemId();
    long lastModified = 0L, lastLastModified = 0L;

    try
      {
        URL url = resolveURL(systemId, base, href);
        systemId = url.toString();
        Node node = (Node) nodeCache.get(systemId);
        // Is the resource up to date?
        URLConnection conn = url.openConnection();
        Long llm = (Long) lastModifiedCache.get(systemId);
        if (llm != null)
          {
            lastLastModified = llm.longValue();
            conn.setIfModifiedSince(lastLastModified);
          }
        conn.connect();
        lastModified = conn.getLastModified();
        if (node != null && 
            lastModified > 0L &&
            lastModified <= lastLastModified)
          {
            // Resource unchanged
          }
        else
          {
            // Resource new or modified
            InputStream in = conn.getInputStream();
            InputSource input = new InputSource(in);
            input.setSystemId(systemId);
            DocumentBuilder builder = getDocumentBuilder();
            node = builder.parse(input);
            nodeCache.put(systemId, node);
            lastModifiedCache.put(systemId, new Long(lastModified));
          }
        return new DOMSource(node, systemId);
      }
    catch (IOException e)
      {
        throw new TransformerException(e);
      }
    catch (SAXException e)
      {
        throw new TransformerException(e);
      }
  }

  URL resolveURL(String systemId, String base, String href)
    throws IOException
  {
    URL url = null;
    try
      {
        if (systemId != null)
          {
            try
              {
                url = new URL(systemId);
              }
            catch (MalformedURLException e)
              {
                // Try building from base + href
              }
          }
        if (url == null)
          {
            if (base != null)
              {
                URL baseURL = new URL(base);
                url = new URL(baseURL, href);
              }
            else
              {
                url = new URL(href);
              }
          }
        return url;
      }
    catch (MalformedURLException e)
      {
        // Fall back to local filesystem
        File file = null;
        if (href == null)
          {
            href = systemId;
          }
        if (base != null)
          {
            int lsi = base.lastIndexOf(File.separatorChar);
            if (lsi != -1 && lsi < base.length() - 1)
              {
                base = base.substring(0, lsi);
              }
            File baseFile = new File(base);
            file = new File(baseFile, href);
          }
        else
          {
            file = new File(href);
          }
        return file.toURL();
      }
  }
  
  DocumentBuilder getDocumentBuilder()
    throws TransformerException
  {
    try
      {
        if (builder == null)
          {
            DocumentBuilderFactory factory =
              DocumentBuilderFactory.newInstance();
            factory.setNamespaceAware(true);
            factory.setExpandEntityReferences(true);
            builder = factory.newDocumentBuilder();
          }
        if (userResolver != null)
          {
            builder.setEntityResolver(new URIResolverEntityResolver(userResolver));
          }
        if (userListener != null)
          {
            builder.setErrorHandler(new ErrorListenerErrorHandler(userListener));
          }
        return builder;
      }
    catch (Exception e)
      {
        throw new TransformerException(e);
      }
  }
  
}

