/*
 * TransformerFactoryImpl.java
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

import javax.xml.transform.ErrorListener;
import javax.xml.transform.Source;
import javax.xml.transform.Templates;
import javax.xml.transform.Transformer;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.transform.TransformerFactory;
import javax.xml.transform.URIResolver;
import javax.xml.transform.dom.DOMResult;
import javax.xml.transform.dom.DOMSource;
import javax.xml.transform.sax.SAXResult;
import javax.xml.transform.sax.SAXSource;
import javax.xml.transform.stream.StreamResult;
import javax.xml.transform.stream.StreamSource;
import javax.xml.xpath.XPathFactory;
import org.w3c.dom.Document;
import org.w3c.dom.Node;

/**
 * GNU transformer factory implementation.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class TransformerFactoryImpl
  extends TransformerFactory
{

  final XPathFactory xpathFactory;
	final XSLURIResolver resolver;
  ErrorListener userListener;
  URIResolver userResolver;

  public TransformerFactoryImpl()
  {
    xpathFactory = new gnu.xml.xpath.XPathFactoryImpl();
		resolver = new XSLURIResolver();
  }

  public Transformer newTransformer(Source source)
    throws TransformerConfigurationException
  {
    return newTemplates(source).newTransformer();
  }

  public Transformer newTransformer()
    throws TransformerConfigurationException
  {
    return new TransformerImpl(this, null);
  }

  public Templates newTemplates(Source source)
    throws TransformerConfigurationException
  {
    Stylesheet stylesheet = newStylesheet(source, 0, null);
    return new TemplatesImpl(this, stylesheet);
  }

  Stylesheet newStylesheet(Source source, int precedence, Stylesheet parent)
    throws TransformerConfigurationException
  {
    Document doc = null;
    String systemId = null;
    if (source != null)
      {
				try
				  {
						DOMSource ds;
						synchronized (resolver)
						{
							resolver.setUserResolver(userResolver);
							resolver.setUserListener(userListener);
							ds = resolver.resolveDOM(source, null, null);
						}
						Node node = ds.getNode();
						if (node == null)
						{
							throw new TransformerConfigurationException("no source document");
						}
						doc = (node instanceof Document) ? (Document) node :
							node.getOwnerDocument();
						systemId = ds.getSystemId();
					}
				catch (TransformerException e)
				  {
						throw new TransformerConfigurationException(e);
					}
      }
    return new Stylesheet(this, parent, doc, systemId, precedence);
  }

  public Source getAssociatedStylesheet(Source source,
                                        String media,
                                        String title,
                                        String charset)
    throws TransformerConfigurationException
  {
    // TODO
    throw new TransformerConfigurationException("not supported");
  }

  public void setURIResolver(URIResolver resolver)
  {
    userResolver = resolver;
  }

  public URIResolver getURIResolver()
  {
    return userResolver;
  }

  public void setFeature(String name, boolean value)
    throws TransformerConfigurationException
  {
    throw new TransformerConfigurationException("not supported");
  }

  public boolean getFeature(String name)
  {
    if (SAXSource.FEATURE.equals(name) ||
        SAXResult.FEATURE.equals(name) ||
        StreamSource.FEATURE.equals(name) ||
        StreamResult.FEATURE.equals(name) ||
        DOMSource.FEATURE.equals(name) ||
        DOMResult.FEATURE.equals(name))
      {
        return true;
      }
    return false;
  }

  public void setAttribute(String name, Object value)
    throws IllegalArgumentException
  {
    throw new IllegalArgumentException("not supported");
  }

  public Object getAttribute(String name)
    throws IllegalArgumentException
  {
    throw new IllegalArgumentException("not supported");
  }

  public void setErrorListener(ErrorListener listener)
    throws IllegalArgumentException
  {
    userListener = listener;
  }

  public ErrorListener getErrorListener()
  {
    return userListener;
  }
  
}
