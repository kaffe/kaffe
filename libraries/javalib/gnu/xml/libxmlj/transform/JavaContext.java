/* 
 * Copyright (C) 2003, 2004 Free Software Foundation, Inc.
 * 
 * This file is part of GNU Classpathx/jaxp.
 * 
 * GNU Classpath is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *  
 * GNU Classpath is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU Classpath; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA.
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
 * obligated to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 */

package gnu.xml.libxmlj.transform;

import javax.xml.transform.ErrorListener;
import javax.xml.transform.Source;
import javax.xml.transform.SourceLocator;
import javax.xml.transform.TransformerException;
import javax.xml.transform.URIResolver;

import javax.xml.transform.stream.StreamSource;

import java.io.InputStream;
import java.io.IOException;
import java.net.URL;

import java.util.Map;
import java.util.HashMap;
import java.util.LinkedHashMap;

import gnu.xml.libxmlj.dom.GnomeDocument;
import gnu.xml.libxmlj.util.NamedInputStream;
import gnu.xml.libxmlj.util.StandaloneLocator;
import gnu.xml.libxmlj.util.XMLJ;

class JavaContext
{
  private static final int CACHE_SIZE = 25;

  private ErrorListener errorListener;
  private URIResolver uriResolver;
  private static Map cache = new LinkedHashMap (CACHE_SIZE + 1, .75F, true) 
    {
      // This method is called just after a new entry has been added
      public boolean removeEldestEntry (Map.Entry eldest) 
      {
        if (this.size () > CACHE_SIZE)
          {
            // Note: the following is a workaround for a problem in
            // the Gcj LinkedHashMap implementation which will
            // otherwise prevent the removed object to be finalized in
            // some cases.
            this.remove(eldest.getKey());
            try {
              eldest.setValue(null);
            }
            catch (Throwable ignore) {
            }
          }

        return false;
      }
    };

  static void cleanup ()
  {
    cache = null;
  }

  JavaContext (URIResolver uriResolver, ErrorListener errorListener)
  {
    this.errorListener = errorListener;
    this.uriResolver = uriResolver;
  }

  // -- Callbacks from libxmlj --

  private InputStream resolveEntity (String publicId,
                                     String systemId)
    throws TransformerException
    {
      if (uriResolver != null)
        {
          systemId = uriResolver.resolve (null, systemId).getSystemId ();
        }
      if (systemId == null)
        {
          return null;
        }
      try
        {
          URL url = new URL (systemId);
          return XMLJ.getInputStream (url);
        }
      catch (IOException e)
        {
          throw new TransformerException (e);
        }
    }

  private void setDocumentLocator (Object ctx, Object loc)
    {
    }

  private void warning (String message,
                        int lineNumber,
                        int columnNumber,
                        String publicId,
                        String systemId)
    throws TransformerException
    {
      if (errorListener == null)
        {
          return;
        }
      SourceLocator l = new StandaloneLocator (lineNumber,
                                               columnNumber,
                                               publicId,
                                               systemId);
      errorListener.warning (new TransformerException (message, l));
    }
  
  private void error (String message,
                      int lineNumber,
                      int columnNumber,
                      String publicId,
                      String systemId)
    throws TransformerException
    {
      if (errorListener == null)
        {
          return;
        }
      SourceLocator l = new StandaloneLocator (lineNumber,
                                               columnNumber,
                                               publicId,
                                               systemId);
      errorListener.error (new TransformerException (message, l));
    } 
  
  private void fatalError (String message,
                           int lineNumber,
                           int columnNumber,
                           String publicId,
                           String systemId)
    throws TransformerException
    {
      if (errorListener == null)
        {
          return;
        }
      SourceLocator l = new StandaloneLocator (lineNumber,
                                               columnNumber,
                                               publicId,
                                               systemId);
      errorListener.fatalError (new TransformerException (message, l));
    } 
 
  // -- TODO remove these --
   
  void xsltGenericError (String message) 
    throws TransformerException
  {
    TransformerException exception =
      new TransformerException (message.trim ());
    errorListener.error (exception);
  } 

  public String toString ()
  {
    return "JavaContext{errorListener=" + errorListener + ",uriResolver=" +
      uriResolver + "}";
  }

  //--- DOM caching methods follow

  private native GnomeDocument parseDocument (InputStream in,
                                              byte[] detectBuffer,
                                              String systemId,
                                              String publicId,
                                              String base);

  GnomeDocument resolveURIAndOpen (String href,
                                   String base) 
    throws TransformerException
  {
    if (uriResolver != null)
      {
        Source source = uriResolver.resolve (href, base);
        return parseDocumentCached (source);
      }
    else
      {
        try
          {
            URL url = new URL (XMLJ.getAbsoluteURI (base, href));
            InputStream in = url.openStream();
            return parseDocumentCached (in, null, url.toString ());
          }
        catch (IOException e)
          {
            throw new TransformerException(e);
          }
      }               
  }

  GnomeDocument parseDocumentCached (InputStream in,
                                     String publicId,
                                     String systemId)
    throws TransformerException
  {
    StreamSource source = new StreamSource ();

    source.setSystemId(systemId);
    if (null != in) 
      {
        source.setInputStream (in);
      }
    if (null != publicId) 
      {
        source.setPublicId (publicId);
      }
    return parseDocumentCached (source);
  }

  GnomeDocument parseDocumentCached (Source source)
    throws TransformerException
  {
    String systemId = source.getSystemId ();
    String base = XMLJ.getBaseURI (systemId);
    GnomeDocument cachedValue = (GnomeDocument) cache.get (systemId);
    if (null != cachedValue)
      {
        return cachedValue;
      }
    else
      {
        try
          {
            NamedInputStream in = XMLJ.getInputStream (source);
            byte[] detectBuffer = in.getDetectBuffer ();
            if (detectBuffer == null)
              {
                throw new TransformerException ("No document element");
              }
            GnomeDocument value =
              parseDocument (in, detectBuffer, null, systemId, base);
            cache.remove (systemId);
            cache.put (systemId, value);
            return value;
          }
        catch (IOException e)
          {
            throw new TransformerException (e);
          }
      }
  }
}
