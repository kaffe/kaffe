/*
 * DocumentFunction.java
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

import java.io.InputStream;
import java.io.IOException;
import java.net.URL;
import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import java.util.TreeSet;
import javax.xml.transform.ErrorListener;
import javax.xml.transform.TransformerException;
import javax.xml.transform.URIResolver;
import javax.xml.transform.dom.DOMSource;
import javax.xml.xpath.XPathFunction;
import javax.xml.xpath.XPathFunctionException;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import gnu.xml.xpath.Expr;
import gnu.xml.xpath.FunctionCall;

/**
 * The XSLT <code>document()</code>function.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
final class DocumentFunction
  implements XPathFunction
{

  final Stylesheet stylesheet;
  final Node base;

  DocumentFunction(Stylesheet stylesheet, Node base)
  {
    this.stylesheet = stylesheet;
    this.base = base;
  }

  public Object evaluate(List args)
    throws XPathFunctionException
  {
    switch (args.size())
      {
      case 1:
        Object arg = args.get(0);
        if (arg instanceof Collection)
          {
            Collection ns = (Collection) arg;
            Collection acc = new TreeSet();
            for (Iterator i = ns.iterator(); i.hasNext(); )
              {
                Node node = (Node) i.next();
                String s = Expr.stringValue(node);
                acc.addAll(document(s, baseURI(node)));
              }
            return acc;
          }
        else
          {
            String s = Expr._string(null, arg);
            return document(s, baseURI(base));
          }
      case 2:
        Object arg1 = args.get(0);
        Object arg2 = args.get(1);
        if (!(arg2 instanceof Collection))
          {
            throw new XPathFunctionException("second argument is not a node-set");
          }
        Collection arg2ns = (Collection) arg2;
        String base2 = arg2ns.isEmpty() ? null :
          baseURI((Node) arg2ns.iterator().next());
        if (arg1 instanceof Collection)
          {
            Collection arg1ns = (Collection) arg1;
            Collection acc = new TreeSet();
            for (Iterator i = arg1ns.iterator(); i.hasNext(); )
              {
                Node node = (Node) i.next();
                String s = Expr.stringValue(node);
                acc.addAll(document(s, base2));
              }
            return acc;
          }
        else
          {
            String s = Expr._string(null, arg1);
            return document(s, base2);
          }
      default:
        throw new XPathFunctionException("invalid arity");
      }
  }

  /**
   * Returns the XSL base URI of the specified node.
   * @see XSLT 3.2
   */
  String baseURI(Node node)
  {
    if (node == null)
      {
        return null;
      }
    else if (node.getNodeType() == Node.DOCUMENT_NODE)
      {
        return ((Document) node).getDocumentURI();
      }
    else
      {
        return baseURI(node.getOwnerDocument());
      }
  }

  /**
   * The XSL <code>document</code> function.
   * @see XSLT 12.1
   * @param uri the URI from which to retrieve nodes
   * @param base the base URI for relative URIs
   */
  Collection document(String uri, String base)
    throws XPathFunctionException
  {
    if ("".equals(uri))
      {
        uri = baseURI(this.base);
      }
    
    // Get fragment
    Expr fragment = null;
    int hi = uri.indexOf('#');
    if (hi != -1)
      {
        String f = uri.substring(hi + 1);
        uri = uri.substring(0, hi);
        // TODO handle xpointer() here
        // this only handles IDs
        fragment = new FunctionCall(stylesheet, "id",
                                    Collections.singletonList(f));
      }

    // Get document source
    try
      {
				DOMSource source;
				XSLURIResolver resolver = stylesheet.factory.resolver;
				synchronized (resolver)
				  {
						if (stylesheet.transformer != null)
						  {
								resolver.setUserResolver(stylesheet.transformer.uriResolver);
								resolver.setUserListener(stylesheet.transformer.errorListener);
							}
						source = resolver.resolveDOM(null, base, uri);
					}
				Node node = source.getNode();
				if (fragment == null)
				  {
						return Collections.singleton(node);
					}
				else
				  {
						Object ret = fragment.evaluate(node, 1, 1);
						if (!(ret instanceof Collection))
						  {
								// XXX Report error?
								return Collections.EMPTY_SET;
							}
						return (Collection) ret;
					}
			}
    catch (TransformerException e)
      {
        String msg = "can't open " + uri;
        if (base != null)
          {
            msg += " with base " + base;
          }
        throw new XPathFunctionException(msg);
      }
  }
  
}
