/*
 * SAXParserFactory.java
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

package javax.xml.parsers;

// Imports
import java.io.IOException;
import java.io.InputStream;
import java.io.File;
import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Properties;
import javax.xml.validation.Schema;
import org.w3c.dom.*;
import org.xml.sax.*;

/**
 * SAXParserFactory is used to bootstrap JAXP wrappers for
 * SAX parsers.
 *
 * <para> Note that the JAXP 1.1 spec does not specify how
 * the <em>isValidating()</em> or <em>isNamespaceAware()</em>
 * flags relate to the SAX2 feature flags controlling those
 * same features.
 *
 * @author	Andrew Selkirk, David Brownell
 * @version	1.0
 */

public abstract class SAXParserFactory
{

  //-------------------------------------------------------------
  // Variables --------------------------------------------------
  //-------------------------------------------------------------

  private static final	String defaultPropName	=
    "javax.xml.parsers.SAXParserFactory";

  private 		boolean validating	= false;
  private 		boolean namespaceAware	= false;

  private Schema schema;
  private boolean xIncludeAware;

  //-------------------------------------------------------------
  // Initialization ---------------------------------------------
  //-------------------------------------------------------------

  protected SAXParserFactory()
  {
  } // SAXParserFactory()


  //-------------------------------------------------------------
  // Methods ----------------------------------------------------
  //-------------------------------------------------------------

  public static SAXParserFactory newInstance()
  {
    try
      {
        return (SAXParserFactory)
          ClassStuff.createFactory (
                                    defaultPropName, 
                                    "gnu.xml.aelfred2.JAXPFactory");
      }
    catch (ClassCastException e)
      {
        throw new FactoryConfigurationError (e,
                                             "Factory class is the wrong type");
      }
  }

  /**
   * Returns a new instance of a SAXParser using the platform
   * default implementation and the currently specified factory
   * feature flag settings.
   *
   * @exception ParserConfigurationException
   *	when the parameter combination is not supported
   * @exception SAXNotRecognizedException
   *	if one of the specified SAX2 feature flags is not recognized
   * @exception SAXNotSupportedException
   *	if one of the specified SAX2 feature flags values can
   *	not be set, perhaps because of sequencing requirements
   *	(which could be met by using SAX2 directly)
   */
  public abstract SAXParser newSAXParser()
    throws ParserConfigurationException, SAXException;

  public void setNamespaceAware(boolean value)
  {
    namespaceAware = value;
  } // setNamespaceAware()

  public void setValidating(boolean value)
  {
    validating = value;
  } // setValidating()

  public boolean isNamespaceAware()
  {
    return namespaceAware;
  } // isNamespaceAware()

  public boolean isValidating()
  {
    return validating;
  } // isValidating()

  /**
   * Establishes a factory parameter corresponding to the
   * specified feature flag.
   *
   * @param name identifies the feature flag
   * @param value specifies the desired flag value
   *
   * @exception SAXNotRecognizedException
   *	if the specified SAX2 feature flag is not recognized
   * @exception SAXNotSupportedException
   *	if the specified SAX2 feature flag values can not be set,
   *	perhaps because of sequencing requirements (which could
   *	be met by using SAX2 directly)
   */
  public abstract void setFeature (String name, boolean value) 
    throws	ParserConfigurationException, SAXNotRecognizedException, 
  SAXNotSupportedException;

  /**
   * Retrieves a current factory feature flag setting.
   *
   * @param name identifies the feature flag
   *
   * @exception SAXNotRecognizedException
   *	if the specified SAX2 feature flag is not recognized
   * @exception SAXNotSupportedException
   *	if the specified SAX2 feature flag values can not be
   *	accessed before parsing begins.
   */
  public abstract boolean getFeature (String name) 
    throws	ParserConfigurationException, SAXNotRecognizedException, 
  SAXNotSupportedException;

  // -- JAXP 1.3 methods --

  /**
   * Returns the schema.
   * @since 1.3
   * @see #setSchema
   */
  public Schema getSchema()
  {
    return schema;
  }

  /**
   * Sets the schema.
   * @since 1.3
   * @see #getSchema
   */
  public void setSchema(Schema schema)
  {
    this.schema = schema;
  }

  public boolean isXIncludeAware()
  {
    return xIncludeAware;
  }

  public void setXIncludeAware(boolean state)
  {
    xIncludeAware = state;
  }

} // SAXParserFactory

