/*
 * XPathImpl.java
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

package gnu.xml.xpath;

import java.io.IOException;
import javax.xml.namespace.NamespaceContext;
import javax.xml.namespace.QName;
import javax.xml.xpath.XPath;
import javax.xml.xpath.XPathExpression;
import javax.xml.xpath.XPathExpressionException;
import javax.xml.xpath.XPathFunctionResolver;
import javax.xml.xpath.XPathVariableResolver;
import org.xml.sax.InputSource;

/**
 * JAXP XPath implementation.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class XPathImpl
  implements XPath
{

  XPathParser parser;
  XPathVariableResolver variableResolver;
  XPathFunctionResolver functionResolver;
  NamespaceContext namespaceContext;

  XPathImpl(XPathVariableResolver variableResolver,
            XPathFunctionResolver functionResolver)
  {
    parser = new XPathParser();
    this.variableResolver = variableResolver;
    this.functionResolver = functionResolver;
    reset();
  }

  public void reset()
  {
    parser.variableResolver = variableResolver;
    parser.functionResolver = functionResolver;
    namespaceContext = null;
  }

  public void setXPathVariableResolver(XPathVariableResolver resolver)
  {
    parser.variableResolver = resolver;
  }

  public XPathVariableResolver getXPathVariableResolver()
  {
    return parser.variableResolver;
  }

  public void setXPathFunctionResolver(XPathFunctionResolver resolver)
  {
    parser.functionResolver = resolver;
  }

  public XPathFunctionResolver getXPathFunctionResolver()
  {
    return parser.functionResolver;
  }

  public void setNamespaceContext(NamespaceContext nsContext)
  {
    namespaceContext = nsContext;
  }

  public NamespaceContext getNamespaceContext()
  {
    return namespaceContext;
  }

  public XPathExpression compile(String expression)
    throws XPathExpressionException
  {
    XPathTokenizer tokenizer = new XPathTokenizer(expression);
    try
      {
        return (Expr) parser.yyparse(tokenizer);
      }
    catch (IOException e)
      {
        throw new XPathExpressionException(e);
      }
    catch (XPathParser.yyException e)
      {
        throw new XPathExpressionException(e);
      }
  }
  
  public Object evaluate(String expression,
                         Object item,
                         QName returnType)
    throws XPathExpressionException
  {
    XPathExpression expr = compile(expression);
    return expr.evaluate(item, returnType);
  }

  public String evaluate(String expression,
                         Object item)
    throws XPathExpressionException
  {
    XPathExpression expr = compile(expression);
    return expr.evaluate(item);
  }

  public Object evaluate(String expression,
                         InputSource source,
                         QName returnType)
    throws XPathExpressionException
  {
    XPathExpression expr = compile(expression);
    return expr.evaluate(source, returnType);
  }

  public String evaluate(String expression,
                         InputSource source)
    throws XPathExpressionException
  {
    XPathExpression expr = compile(expression);
    return expr.evaluate(source);
  }

}
