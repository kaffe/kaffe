/*
 * DomXPathExpression.java
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

package gnu.xml.dom;

import java.io.IOException;
import java.util.Collection;
import org.w3c.dom.DOMException;
import org.w3c.dom.Node;
import org.w3c.dom.xpath.XPathException;
import org.w3c.dom.xpath.XPathExpression;
import org.w3c.dom.xpath.XPathNSResolver;
import org.w3c.dom.xpath.XPathResult;
import gnu.xml.xpath.Expr;
import gnu.xml.xpath.XPathParser;
import gnu.xml.xpath.XPathTokenizer;

/**
 * An XPath expression.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class DomXPathExpression
implements XPathExpression
{

  final DomDocument doc;
  final Expr expression;
  final XPathNSResolver resolver;

  DomXPathExpression (DomDocument doc, String expression,
                      XPathNSResolver resolver)
    throws XPathException
  {
    this.doc = doc;
    this.resolver = resolver;
    
    XPathParser parser = new XPathParser ();
    XPathTokenizer tokenizer = new XPathTokenizer (expression);
    try
      {
        this.expression = (Expr) parser.yyparse (tokenizer);
      }
    catch (IOException e)
      {
        throw new XPathException (XPathException.INVALID_EXPRESSION_ERR,
                                  e.getMessage ());
      }
    catch (XPathParser.yyException e)
      {
        throw new XPathException (XPathException.INVALID_EXPRESSION_ERR,
                                  e.getMessage ());
      }
    System.out.println("expression="+this.expression);
  }

  public Object evaluate (Node contextNode, short type, Object result)
    throws XPathException, DOMException
  {
    Object val = expression.evaluate (contextNode);
    switch (type)
      {
      case XPathResult.BOOLEAN_TYPE:
        if (!(val instanceof Boolean))
          {
            throw new XPathException (XPathException.TYPE_ERR, null);
          }
        break;
      case XPathResult.NUMBER_TYPE:
        if (!(val instanceof Double))
          {
            throw new XPathException (XPathException.TYPE_ERR, null);
          }
        break;
      case XPathResult.STRING_TYPE:
        if (!(val instanceof String))
          {
            throw new XPathException (XPathException.TYPE_ERR, null);
          }
        break;
      case XPathResult.ANY_UNORDERED_NODE_TYPE:
      case XPathResult.FIRST_ORDERED_NODE_TYPE:
      case XPathResult.UNORDERED_NODE_ITERATOR_TYPE:
      case XPathResult.UNORDERED_NODE_SNAPSHOT_TYPE:
        if (!(val instanceof Collection))
          {
            throw new XPathException (XPathException.TYPE_ERR, null);
          }
        break;
      case XPathResult.ORDERED_NODE_ITERATOR_TYPE:
      case XPathResult.ORDERED_NODE_SNAPSHOT_TYPE:
        /* We don't support ordered node-sets */
        throw new XPathException (XPathException.TYPE_ERR, null);
      }
    return new DomXPathResult (val, type);
  }
  
}
