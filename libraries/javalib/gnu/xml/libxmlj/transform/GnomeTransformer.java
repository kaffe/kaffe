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

import java.io.ByteArrayOutputStream;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PushbackInputStream;

import java.net.URL;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Properties;

import javax.xml.transform.ErrorListener;
import javax.xml.transform.Source;
import javax.xml.transform.SourceLocator;
import javax.xml.transform.Result;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.URIResolver;

import javax.xml.transform.stream.StreamSource;
import javax.xml.transform.stream.StreamResult;

/**
 *  An implementation of {@link javax.xml.transform.Transformer} which
 *  performs XSLT transformation using <code>libxslt</code>.
 *
 *  @fixme This implementation keeps native handles in
 *  <code>long</code> fields. A solution mapping handle IDs to actual
 *  memory addresses within the native peer may be more safe. Will
 *  this one day be called a software with the 19-exabyte-problem?
 *
 *  @author Julian Scheid
 */
public class GnomeTransformer
  extends Transformer 
{

  /**
   *  The parameters added by the user via {@link setParameter()}.
   */
  private Map parameters = new HashMap ();

  /**
   *  The output properties set by the user.
   */
  private Properties outputProperties = new Properties ();

  /**
   *  The URIResolver passed in by the {@link TransformFactory} or
   *  {@link Templates}.
   */
  private URIResolver uriResolver;

  /**
   *  The ErrorListener passed in by the {@link TransformFactory} or
   *  {@link Templates}.
   */
  private ErrorListener errorListener;

  /**
   *  The object representing the underlying Libxslt Stylesheet.
   */
  private LibxsltStylesheet stylesheet;

  /**
   *  Package-private constructor. Called by {@link
   *  TransformerFactory}.
   *
   *  @param uriResolver the default URIResolver to use (can be
   *  overridden by the user).
   *
   *  @param errorListener the default ErrorListener to use (can be
   *  overridden by the user).
   *
   *  @param nativeStylesheetHandle implementation-specific handle
   *  to underlying stylesheet object.
   *
   *  @param attributes implementation-specific attributes passed in
   *  by the factory. These are currently not used.
   */
  GnomeTransformer (URIResolver uriResolver, 
                    ErrorListener errorListener,
                    LibxsltStylesheet stylesheet, 
                    Map attributes)
  {
    this.uriResolver = uriResolver;
    this.errorListener = errorListener;
    this.stylesheet = stylesheet;

    // ignore attributes
  }

  /**
   *  Package-private constructor. Called by {@link
   *  Templates}.
   *
   *  @param uriResolver the default URIResolver to use (can be
   *  overridden by the user).
   *
   *  @param errorListener the default ErrorListener to use (can be
   *  overridden by the user).
   *
   *  @param xsltSource the XML Source for reading the XSLT
   *  stylesheet represented by this object.
   *
   *  @param attributes implementation-specific attributes passed in
   *  by the factory. These are currently not used.
   */
  GnomeTransformer (URIResolver uriResolver, 
                    ErrorListener errorListener,
                    Source source,
                    Map attributes) 
    throws TransformerConfigurationException
  {
    this (uriResolver, errorListener,
          new LibxsltStylesheet (source, new JavaContext (uriResolver, 
                                                          errorListener)),
          attributes);
  }

  //--- Implementation of javax.xml.transform.Transformer follows.

  // Set, get and clear the parameters to use on transformation

  public synchronized void setParameter (String parameter, Object value)
  {
    parameters.put (parameter, value);
  } 

  public synchronized Object getParameter (String name)
  {
    return parameters.get (name);
  }

  public synchronized void clearParameters ()
  {
    parameters.clear ();
  }

  // Set and get the ErrorListener to use on transformation

  public void setErrorListener (ErrorListener listener)
  {
    this.errorListener = listener;
  } 

  public ErrorListener getErrorListener ()
  {
    return errorListener;
  }

  // Set and get the URIResolver to use on transformation

  public void setURIResolver (URIResolver uriResolver)
  {
    this.uriResolver = uriResolver;
  } 

  public URIResolver getURIResolver ()
  {
    return uriResolver;
  }

  // Set the output properties to use on transformation; get default
  // output properties and output properties specified in the
  // stylesheet or by the user.

  public void setOutputProperties (Properties outputProperties)
  {
    // Note: defensive copying
    this.outputProperties = new Properties (outputProperties);
  } 

  public void setOutputProperty (String name, String value)
  {
    outputProperties.setProperty (name, value);
  } 

  public Properties getOutputProperties ()
  {
    // Note: defensive copying
    return new Properties (this.outputProperties);
  }

  public String getOutputProperty (String name)
  {
    return outputProperties.getProperty (name);
  }

  /**
   *  <p> Transforms the given source and writes the result to the
   *  given target. </p>
   *
   *  @fixme This performs poorly at the time for large input
   *  documents, as both input and output document are held in
   *  memory, and even at the same time. A better implementation
   *  would be to implement the appropriate interfaces for reading
   *  and writing streams from the libxml API.  (check names.)
   *
   */
  public void transform (Source source, Result result)
    throws TransformerException
  {
    stylesheet.transform (source, result, parameters,
			  new JavaContext (uriResolver, errorListener));
  }

}
