/*
 * TransformerFactory.java
 * Copyright (C) 2001 Andrew Selkirk
 * Copyright (C) 2001 The Free Software Foundation
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
package javax.xml.transform;

// Imports
import java.io.IOException;
import java.io.InputStream;
import java.io.FileInputStream;
import java.io.File;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Properties;

/**
 * Abstract class extended by implementations.
 *
 * @author	Andrew Selkirk, David Brownell
 * @version	1.0
 */
public abstract class TransformerFactory
{

  /** Constructor, for use by subclasses. */
  protected TransformerFactory()
  {
  }


  //-------------------------------------------------------------
  // Methods ----------------------------------------------------
  //-------------------------------------------------------------

  /**
   * Returns an object encapsulating the &lt;?xml-stylesheet&nbsp;?&gt;
   * processing instruction from the document that matches the
   * specified criteria.
   */
  public abstract Source getAssociatedStylesheet(Source source, 
                                                 String media,
                                                 String title,
                                                 String charset) 
    throws TransformerConfigurationException;

  /** Returns an implementation-specific attribute */
  public abstract Object getAttribute(String name) 
    throws IllegalArgumentException;

  /** Returns the ErrorListener used when parsing stylesheets. */
  public abstract ErrorListener getErrorListener();

  /**
   * Exposes capabilities of the underlying implementation.
   * Examples include SAXSource.FEATURE and DOMResult.FEATURE.
   */
  public abstract boolean getFeature(String name);

  /** Returns the URIResolver used when parsing stylesheets. */
  public abstract URIResolver getURIResolver();


  /**
   * Returns a new TransformerFactory.  The name of this class
   * is found by checking, in order:
   * the <em>javax.xml.transform.TransformerFactory</em>
   *	system property,
   * <em>$JAVA_HOME/lib/jaxp.properties</em> for the key with
   *	that same name,
   * JAR files in the class path with a <em>META-INF/services</em>
   *	file with that same name,
   * else the compiled-in platform default.
   */
  public static TransformerFactory newInstance() 
    throws TransformerFactoryConfigurationError
  {
    try
      {
        return (TransformerFactory) ClassStuff.createFactory (
                                                            "javax.xml.transform.TransformerFactory",
                                                            "gnu.xml.libxmlj.transform.GnomeTransformerFactory"
                                                            // "gnu.xml.util.SAXNullTransformerFactory"
                                                            // "org.apache.xalan.processor.TransformerFactoryImpl"
                                                           );
      }
    catch (ClassCastException e)
      {
        throw new TransformerFactoryConfigurationError(e);
      }
  }


  /**
   * Returns a pre-compiled stylesheet.
   * @param stylesheet XSLT stylesheet specifying transform
   */
  public abstract Templates newTemplates (Source stylesheet) 
    throws TransformerConfigurationException;

  /**
   * Returns a transformer that performs the null transform.
   */
  public abstract Transformer newTransformer() 
    throws TransformerConfigurationException;

  /**
   * Returns a transformer making a specified transform.
   * @param stylesheet XSLT stylesheet specifying transform
   */
  public abstract Transformer newTransformer (Source stylesheet) 
    throws TransformerConfigurationException;


  /** Assigns an implementation-specific attribute */
  public abstract void setAttribute(String name, Object value)
    throws IllegalArgumentException;

  /** Assigns the ErrorListener used when parsing stylesheets. */
  public abstract void setErrorListener(ErrorListener listener) 
    throws IllegalArgumentException;

  /** Assigns the URIResolver used when parsing stylesheets. */
  public abstract void setURIResolver(URIResolver resolver);

}
