/* 
 * $Id: JavaContext.java,v 1.1 2004/04/14 19:40:35 dalibor Exp $
 * Copyright (C) 2003 Julian Scheid
 * 
 * This file is part of GNU LibxmlJ, a JAXP-compliant Java wrapper for
 * the XML and XSLT C libraries for Gnome (libxml2/libxslt).
 * 
 * GNU LibxmlJ is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *  
 * GNU LibxmlJ is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GNU LibxmlJ; see the file COPYING.  If not, write to the
 * Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
 * 02111-1307 USA. 
 */

package gnu.xml.libxmlj.transform;

import javax.xml.transform.ErrorListener;
import javax.xml.transform.Source;
import javax.xml.transform.SourceLocator;
import javax.xml.transform.TransformerException;
import javax.xml.transform.URIResolver;

import javax.xml.transform.stream.StreamSource;

import java.io.InputStream;

import java.util.Map;
import java.util.HashMap;
import java.util.LinkedHashMap;

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
  
  //--- Implementation of
  //--- gnu.xml.transform.LibxsltTransformErrorAdapter follows.

  void saxWarning (String message, SourceLocator sourceLocator)
    throws TransformerException
  {
    errorListener.
      warning (new TransformerException (message.trim (), sourceLocator));
  } 

  void saxError (String message, SourceLocator sourceLocator) 
    throws TransformerException
  {
    errorListener.
      error (new TransformerException (message.trim (), sourceLocator));
  } 

  void saxFatalError (String message, SourceLocator sourceLocator) 
    throws TransformerException
  {
    errorListener.
      fatalError (new TransformerException (message.trim (), sourceLocator));
  } 

  void xsltGenericError (String message) 
    throws TransformerException
  {
    TransformerException exception =
      new TransformerException (message.trim ());
    errorListener.error (exception);
  } 


  SourceWrapper resolveURI (String href, String base) 
    throws TransformerException
  {
    return new SourceWrapper (uriResolver.resolve (href, base));
  }

  public String toString ()
  {
    return "JavaContext{errorListener=" + errorListener + ",uriResolver=" +
      uriResolver + "}";
  }

  //--- DOM caching methods follow

  private native long parseDocument (InputStream in, String systemId,
				     String publicId);

  LibxmlDocument resolveURIAndOpen (String href,
                                    String base) 
    throws TransformerException
  {
    Source source = uriResolver.resolve (href, base);

    return parseDocumentCached (source);
  }

  LibxmlDocument parseDocumentCached (InputStream in, String systemId, String publicId)
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

  LibxmlDocument parseDocumentCached (Source source)
    throws TransformerException
  {
    String systemId = source.getSystemId ();
    LibxmlDocument cachedValue = (LibxmlDocument) cache.get (systemId);
    if (null != cachedValue)
      {
        return cachedValue;
      }
    else
      {
        long rc 
          = parseDocument (new SourceWrapper (source).getInputStream (),
                           source.getSystemId (),
                           null);
        LibxmlDocument value = new LibxmlDocument (rc);
        cache.remove (systemId);
        cache.put (systemId, value);
        return value;
      }
  }
}
