/*
 * TransformerImpl.java
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

import java.util.LinkedHashMap;
import java.util.Map;
import java.util.Properties;
import javax.xml.namespace.QName;
import javax.xml.transform.ErrorListener;
import javax.xml.transform.Result;
import javax.xml.transform.Source;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.URIResolver;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.dom.DOMResult;
import javax.xml.xpath.XPathVariableResolver;
import org.w3c.dom.Document;
import org.w3c.dom.Node;

class TransformerImpl
  extends Transformer
  implements XPathVariableResolver
{

  final Map parameters;
  Properties outputProperties;
  final Stylesheet stylesheet;
  URIResolver uriResolver;
  ErrorListener errorListener;

  TransformerImpl(URIResolver resolver, ErrorListener listener,
                  Stylesheet stylesheet)
    throws TransformerConfigurationException
  {
    parameters = new LinkedHashMap();
    uriResolver = resolver;
    errorListener = listener;
    this.stylesheet = stylesheet;
  }

  public void transform(Source xmlSource, Result outputTarget)
    throws TransformerException
  {
    DOMSource source =
      new DOMSourceWrapper(xmlSource, uriResolver, errorListener);
    DOMResult result = new DOMResultWrapper(outputTarget);
    Node context = source.getNode();
    Node parent = result.getNode();
    Node nextSibling = result.getNextSibling();
    boolean created = false;
    if (parent == null)
      {
        Document doc = (context instanceof Document) ? (Document) context :
          context.getOwnerDocument();
        parent = doc.createDocumentFragment();
        created = true;
      }
    stylesheet.applyTemplates(context, (String) null, null,
                              parent, nextSibling);
    if (created)
      {
        result.setNode(parent);
      }
  }

  public void setParameter(String name, Object value)
  {
    parameters.put(name, value);
  }

  public Object getParameter(String name)
  {
    return parameters.get(name);
  }

  public void clearParameters()
  {
    parameters.clear();
  }

  public void setURIResolver(URIResolver resolver)
  {
    uriResolver = resolver;
  }

  public URIResolver getURIResolver()
  {
    return uriResolver;
  }

  public void setOutputProperties(Properties oformat)
    throws IllegalArgumentException
  {
    outputProperties.putAll(oformat);
  }

  public Properties getOutputProperties()
  {
    return (Properties) outputProperties.clone();
  }

  public void setOutputProperty(String name, String value)
    throws IllegalArgumentException
  {
    outputProperties.put(name, value);
  }

  public String getOutputProperty(String name)
    throws IllegalArgumentException
  {
    return outputProperties.getProperty(name);
  }

  public void setErrorListener(ErrorListener listener)
  {
    errorListener = listener;
  }

  public ErrorListener getErrorListener()
  {
    return errorListener;
  }

  public Object resolveVariable(QName variableName)
  {
    return parameters.get(variableName.toString());
  }
  
}
