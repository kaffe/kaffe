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

import java.util.HashMap;
import java.util.Map;

import javax.xml.transform.ErrorListener;
import javax.xml.transform.Source;
import javax.xml.transform.Templates;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.URIResolver;

import javax.xml.transform.stream.StreamSource;

/**
 *  An implementation of <code>TransformerFactory</code> producing
 *  <code>Transformer</code> objects which use <code>libxslt</code>
 *  for transformation.
 *
 *  @author Julian Scheid
 */
public class TransformerFactoryImpl
  extends TransformerFactory
{

  /**
   *  Libxmlj's default ErrorListener.
   */
  private static final ErrorListener defaultErrorListener 
    = new DefaultErrorListenerImpl ();

  /**
   *  Libxmlj's default URIResolver.
   */
  private static final URIResolver defaultURIResolver
    = new DefaultURIResolverImpl ();

  /**
   *  URIResolver set by user, or default implementation.
   */
  private URIResolver uriResolver = defaultURIResolver;

  /**
   *  ErrorListener set by user, or default implementation.
   */
  private ErrorListener errorListener = defaultErrorListener;

  /**
   *  Attributes set by user.
   */
  private Map attributes = new HashMap ();

  //--- Implementation of javax.xml.transform.TransformerFactory
  //--- follows.

  /**
   *  @fixme The API documentation isn't clear about what to do when
   *  no associated stylesheet could be found. Is this supposed to
   *  return <code>null</code>?
   */
  public Source getAssociatedStylesheet (Source source, String media,
					 String title,
					 String charset) 
    throws TransformerConfigurationException
  {
    String stylesheetURI
      = getAssociatedStylesheet (new SourceWrapper (source), media, title,
				 charset);
    return new StreamSource (stylesheetURI);
  }

  public void setAttribute (String name, Object value)
  {
    this.attributes.put (name, value);
  } 

  public Object getAttribute (String name)
  {
    return this.attributes.get (name);
  }
  
  public void setErrorListener (ErrorListener errorListener)
  {
    this.errorListener = errorListener;
  } 

  public ErrorListener getErrorListener ()
  {
    return this.errorListener;
  }
  
  public void setURIResolver (URIResolver uriResolver)
  {
    this.uriResolver = uriResolver;
  } 

  public URIResolver getURIResolver ()
  {
    return this.uriResolver;
  }
  
  public boolean getFeature (String name)
  {
    // No features yet.
    return false;
  }

  /**
   *  Returns a new instance of class {@link Transformer} for a
   *  null souce.
   */
  public Transformer newTransformer ()
    throws TransformerConfigurationException
  {
    return newTransformer (null);
  }

  /**
   *  Returns a new instance of class {@link Transformer} for
   *  the given souce.
   */
  public Transformer newTransformer (Source source)
    throws TransformerConfigurationException
  {
    return new TransformerImpl (uriResolver,
                                errorListener,
                                source, 
                                attributes);
  }

  /**
   *  Returns a new instance of class {@link Transformer} for
   *  the given souce.
   */
  public Templates newTemplates (Source source) 
    throws TransformerConfigurationException
  {
    return new TemplatesImpl (uriResolver,
                              errorListener, 
                              source,
                              attributes);
  }

  private static native String getAssociatedStylesheet (SourceWrapper source, 
                                                        String media,
                                                        String title,
                                                        String charset) 
    throws TransformerConfigurationException;

  /**
   *  Perform native cleanup.
   */
  private static native void freeLibxsltGlobal ();

  /**
   *  Install a shutdown hook for pre-mortem cleanup.
   */
  static
  {
    System.loadLibrary ("xmlj");

    Runtime.getRuntime ().addShutdownHook (new Thread ()
      {
        public void run ()
        {          
          JavaContext.cleanup();

          // Make sure finalizers are run
          System.gc ();
          Runtime.getRuntime ().runFinalization ();

          // Perform global cleanup on the native level
          freeLibxsltGlobal ();
        }
      });
  }
}
