/*
 * Transformer.java
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
import java.util.Properties;

/**
 * Apply a transformation from a source, populating a result.
 * Transformers may be reused, but not concurrently.
 *
 * @author	Andrew Selkirk, David Brownell
 * @version	1.0
 */
public abstract class Transformer
{

  //-------------------------------------------------------------
  // Initialization ---------------------------------------------
  //-------------------------------------------------------------

  /** Default constructor, for use only by subclasses. */
  protected Transformer()
  {
  } // Transformer()


  //-------------------------------------------------------------
  // Methods ----------------------------------------------------
  //-------------------------------------------------------------

  /**
   * Clears all parameter settings.
   * @see #setParameter
   */
  public abstract void clearParameters();

  /** Returns the error handler used as documents are transformed. */
  public abstract ErrorListener getErrorListener();

  /**
   * Returns a copy of the transformer's non-default output properties.
   * That is, properties set in the stylesheet or through
   * methods on this class are not set.
   * @see OutputKeys
   * @see #setOutputProperties
   */
  public abstract Properties getOutputProperties();

  /**
   * Returns the value of a property applying to this transform.
   * Values returned by this method are only those that have
   * been set explicitly.
   * @see OutputKeys
   * @see #setOutputProperty
   */
  public abstract String getOutputProperty(String name) 
    throws IllegalArgumentException;

  /**
   * Returns the value of a parameter passed to this transform.
   * These are primarily for use access within transformations
   * and extensions.
   * @see #setParameter
   */
  public abstract Object getParameter(String name);

  /** Returns the resolver applied to documents being transformed. */
  public abstract URIResolver getURIResolver();

  /** Assigns the error handler used as documents are transformed. */
  public abstract void setErrorListener(ErrorListener listener) 
    throws IllegalArgumentException;
  /**
   * Assigns a set of output properties, as if made by multiple
   * calls to {@link #setOutputProperty}.
   * @see OutputKeys
   * @param outputformat set of properties, or null to reset all
   *	properties to their default values
   */
  public abstract void setOutputProperties(Properties outputformat) 
    throws IllegalArgumentException;

  /**
   * Assigns the value of a transformation property, affecting
   * generation of output (mostly text syntax).  Parameters include
   * those defined by the xslt:output element.  Default settings may
   * be explicitly overridden.
   * @see OutputKeys
   * @see #getOutputProperty
   * @see #setOutputProperties
   * @param name an XML name, or a namespace-scoped XML name
   *	encoded as <em>{uri}localName</em>.
   * @param value associated with the name
   */
  public abstract void setOutputProperty(String name, String value) 
    throws IllegalArgumentException;

  /**
   * Assigns the value of a parameter passed to this transform.
   * These are primarily for use access within transformations
   * and extensions.
   * @see #getParameter
   * @see #clearParameters
   * @param name an XML name, or a namespace-scoped XML name
   *	encoded as <em>{uri}localName</em>.
   * @param value associated with the name
   */
  public abstract void setParameter(String name, Object value);

  /** Assigns the resolver applied to documents being transformed. */
  public abstract void setURIResolver(URIResolver resolver);

  /** Apply the appropriate transformation */
  public abstract void transform(Source source, Result result) 
    throws TransformerException;

  // -- JAXP 1.3 methods --

  /**
   * Reset this Transformer to its original configuration.
   */
  public void reset()
  {
  }

} // Transformer

