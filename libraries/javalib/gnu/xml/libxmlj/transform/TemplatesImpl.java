/* 
 * $Id: TemplatesImpl.java,v 1.1 2004/04/14 19:40:35 dalibor Exp $
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

import java.util.Properties;
import java.util.Map;

import javax.xml.transform.URIResolver;
import javax.xml.transform.ErrorListener;
import javax.xml.transform.Source;
import javax.xml.transform.Templates;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;

import javax.xml.transform.stream.StreamSource;

/**
 *  An implementation of <code>javax.xml.transform.Templates</code>
 *  producing <code>Transformer</code> objects which use
 *  <code>libxslt</code> for transformation.
 *
 *  @author Julian Scheid
 */
public class TemplatesImpl
  implements Templates
{
  private Properties outputProperties;
  private long nativeStylesheetHandle;
  private URIResolver uriResolver;
  private ErrorListener errorListener;
  private LibxsltStylesheet stylesheet;
  private Map attributes;

  TemplatesImpl (URIResolver uriResolver, ErrorListener errorListener,
                 Source xsltSource,
                 Map attributes) throws TransformerConfigurationException
  {
    this.uriResolver = uriResolver;
    this.errorListener = errorListener;
    this.attributes = attributes;
    this.outputProperties = new Properties ();
    this.stylesheet =
      new LibxsltStylesheet (xsltSource,
			     new JavaContext (uriResolver, errorListener));
  } 

  public Transformer newTransformer () 
    throws TransformerConfigurationException
  {
    return new TransformerImpl (uriResolver,
                                errorListener,
                                stylesheet, attributes);
  }

  public Properties getOutputProperties ()
  {
    return new Properties (this.outputProperties);
  }
}
