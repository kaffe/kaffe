/*
 * Template.java
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

import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
import javax.xml.transform.TransformerException;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpression;
import javax.xml.xpath.XPathExpressionException;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.Text;
import gnu.xml.xpath.Expr;
import gnu.xml.xpath.NameTest;
import gnu.xml.xpath.NodeTypeTest;
import gnu.xml.xpath.Root;
import gnu.xml.xpath.Selector;
import gnu.xml.xpath.Step;
import gnu.xml.xpath.Test;
import gnu.xml.xpath.XPathImpl;

/**
 * A template in an XSL stylesheet.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class Template
  implements Comparable
{

  static final double DEFAULT_PRIORITY = 0.5d;

  final XPathImpl xpath;
  final Expr expr;
  final Collection nodeset;
  final double priority;
  final int precedence;
  final String mode;

  Template(XPathImpl xpath, String expression, Collection nodeset,
           int precedence, double priority, String mode)
    throws XPathExpressionException
  {
    this.xpath = xpath;
    Expr e = (Expr) xpath.compile(expression);
    if (e instanceof Selector || e instanceof Step)
      {
        Expr root = new Root();
        Test nt = new NodeTypeTest((short) 0);
        Selector s = new Selector(Selector.DESCENDANT_OR_SELF,
                                  Collections.singletonList(nt));
        e = new Step(s, e);
        e = new Step(root, e);
      }
    expr = e;
    // adjust priority if necessary
    // see XSLT section 5.5
    Test test = getNodeTest(expr);
    if (test != null)
      {
        if (test instanceof NameTest)
          {
            NameTest nameTest = (NameTest) test;
            if (nameTest.matchesAny() ||
                nameTest.matchesAnyLocalName())
              {
                priority = -0.25d;
              }
            else
              {
                priority = 0.0d;
              }
          }
        else
          {
            NodeTypeTest nodeTypeTest = (NodeTypeTest) test;
            if (nodeTypeTest.getNodeType() ==
                Node.PROCESSING_INSTRUCTION_NODE &&
                nodeTypeTest.getData() != null)
              {
                priority = 0.0d;
              }
            else
              {
                priority = -0.5d;
              }
          }
      }
    this.nodeset = nodeset;
    this.precedence = precedence;
    this.priority = priority;
    this.mode = mode;
  }

  public int compareTo(Object other)
  {
    if (other instanceof Template)
      {
        Template t = (Template) other;
        int d = t.precedence - precedence;
        if (d != 0)
          {
            return d;
          }
        double d2 = t.priority - priority;
        if (d2 != 0.0d)
          {
            return (int) Math.round(d2 * 1000.0d);
          }
      }
    return 0;
  }

  Test getNodeTest(XPathExpression expr)
  {
    if (expr instanceof Selector)
      {
        Selector selector = (Selector) expr;
        Test[] tests = selector.getTests();
        if (tests.length > 0)
          {
            return tests[0];
          }
      }
    return null;
  }

  boolean matches(Node context, Node node, String mode)
  {
    if (mode != null && !mode.equals(this.mode))
      {
        return false;
      }
    try
      {
        Collection ns =
          (Collection) expr.evaluate(context, XPathConstants.NODESET);
        if (ns == null)
          {
            return false;
          }
        return ns.contains(node);
      }
    catch (XPathExpressionException e)
      {
        e.printStackTrace();
        return false;
      }
  }

  /**
   * @param stylesheet the stylesheet
   * @param context the context node in the source document
   * @param parent the parent of result nodes
   * @param nextSibling if non-null, add result nodes before this node
   */
  void apply(Stylesheet stylesheet, Node context, String mode,
             Node parent, Node nextSibling)
    throws TransformerException
  {
    if (nodeset == null)
      {
        // Identity transform
        applyNode(context, stylesheet, context, mode, parent, nextSibling);
        return;
      }
    for (Iterator i = nodeset.iterator(); i.hasNext(); )
      {
        applyNode((Node) i.next(), stylesheet, context, mode,
                  parent, nextSibling);
      }
  }

  private void applyNode(Node node, Stylesheet stylesheet,
                         Node context, String mode,
                         Node parent, Node nextSibling)
    throws TransformerException
  {
    String namespaceUri = node.getNamespaceURI();
    if (Stylesheet.XSL_NS.equals(namespaceUri) &&
        Node.ELEMENT_NODE == node.getNodeType())
      {
        try
          {
            String name = node.getLocalName();
            if ("apply-templates".equals(name))
              {
                String select = ((Element) node).getAttribute("select");
                stylesheet.applyTemplates(context, select, mode,
                                          parent, nextSibling);
              }
            else if ("value-of".equals(name))
              {
                String select = ((Element) node).getAttribute("select");
                Object ret = xpath.evaluate(select, context);
                //System.out.println("apply: value-of returned "+ret);
                if (ret instanceof Collection)
                  {
                    Collection ns = (Collection) ret;
                    for (Iterator i = ns.iterator(); i.hasNext(); )
                      {
                        Node child = (Node) i.next();
                        if (nextSibling != null)
                          {
                            parent.insertBefore(child, nextSibling);
                          }
                        else
                          {
                            parent.appendChild(child);
                          }
                      }
                  }
                else if (ret != null)
                  {
                    String text = ret.toString(); // FIXME number formatting
                    Document doc = (parent instanceof Document) ?
                      (Document) parent : parent.getOwnerDocument();
                    Text textNode = doc.createTextNode(text);
                    if (nextSibling != null)
                      {
                        parent.insertBefore(textNode, nextSibling);
                      }
                    else
                      {
                        parent.appendChild(textNode);
                      }
                  }
              }
        // TODO if
        // TODO choose
        // TODO element
        // TODO attribute
          }
        catch (XPathExpressionException e)
          {
            throw new TransformerException(e.getMessage(),
                                           new DOMSourceLocator(node),
                                           e);
          }
      }
    else
      {
        // Insert result node
        Node result = node.cloneNode(false);
        Document doc = (parent instanceof Document) ? (Document) parent :
          parent.getOwnerDocument();
        result = doc.adoptNode(result);
        if (nextSibling != null)
          {
            parent.insertBefore(result, nextSibling);
          }
        else
          {
            parent.appendChild(result);
          }
        // Copy attributes
        NamedNodeMap attrs = node.getAttributes();
        if (attrs != null)
          {
            NamedNodeMap resultAttrs = result.getAttributes();
            int len = attrs.getLength();
            for (int i = 0; i < len; i++)
              {
                Node attr = attrs.item(i).cloneNode(true);
                attr = doc.adoptNode(attr);
                resultAttrs.setNamedItemNS(attr);
              }
          }
        // Process children
        NodeList children = node.getChildNodes();
        if (children != null)
          {
            int len = children.getLength();
            for (int i = 0; i < len; i++)
              {
                Node child = children.item(i);
                applyNode(child, stylesheet, context, mode, result, null);
              }
          }
      }
  }
  
}
