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

  private native int parseDocument (InputStream in, String systemId,
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
        int rc 
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
