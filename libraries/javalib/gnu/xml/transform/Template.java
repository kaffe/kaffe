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

import java.io.PrintStream;
import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import java.util.LinkedList;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
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

  final Stylesheet stylesheet;
  final String name;
  final Expr match;
  final TemplateNode node;
  final double priority;
  final int precedence;
  final String mode;

  static Expr patternToXPath(Expr pattern)
  {
    /*
    if (pattern instanceof Selector || pattern instanceof Step)
      {
        Test nt = new NodeTypeTest((short) 0);
        Selector s = new Selector(Selector.DESCENDANT_OR_SELF,
                                  Collections.singletonList(nt));
        pattern = new Step(s, pattern);
        pattern = new Step(new Root(), pattern);
      }
      */
    return pattern;
  }

  Template(Stylesheet stylesheet, String name, Expr match, Node source,
           int precedence, double priority, String mode)
    throws TransformerConfigurationException
  {
    this.stylesheet = stylesheet;
    this.name = name;
    this.match = patternToXPath(match);
    // adjust priority if necessary
    // see XSLT section 5.5
    Test test = getNodeTest(match);
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
    node = parse(source);
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

  Test getNodeTest(Expr expr)
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

  boolean matches(String mode, Node context, Node node, int pos, int len)
  {
    if (mode != null && !mode.equals(this.mode))
      {
        return false;
      }
    if (match == null)
      {
        return false;
      }
    Object ret = match.evaluate(context, pos, len);
    if (ret != null && ret instanceof Collection)
      {
        Collection ns = (Collection) ret;
        return ns.contains(node);
      }
    return false;
  }

  /**
   * @param stylesheet the stylesheet
   * @param parent the parent of result nodes
   * @param context the context node in the source document
   * @param pos the context position
   * @param len the context size
   * @param nextSibling if non-null, add result nodes before this node
   */
  void apply(Stylesheet stylesheet, String mode,
             Node context, int pos, int len,
             Node parent, Node nextSibling)
    throws TransformerException
  {
    if (node != null)
      {
        node.apply(stylesheet, mode,
                   context, pos, len,
                   parent, nextSibling);
      }
  }

  TemplateNode parse(Node source)
    throws TransformerConfigurationException
  {
    // Hack to associate the document function with its declaring node
    stylesheet.current = source;
    if (source == null)
      {
        return null;
      }
    Node children = source.getFirstChild();
    Node next = source.getNextSibling();
    
    try
      {
        String namespaceUri = source.getNamespaceURI();
        if (Stylesheet.XSL_NS.equals(namespaceUri) &&
            Node.ELEMENT_NODE == source.getNodeType())
          {
            Element element = (Element) source;
            String name = element.getLocalName();
            if ("apply-templates".equals(name))
              {
                String mode = element.getAttribute("mode");
                String s = element.getAttribute("select");
                if (s == null || s.length() == 0)
                  {
                    s = "child::node()";
                  }
                Expr select = (Expr) stylesheet.xpath.compile(s);
                List sortKeys = parseSortKeys(children);
                List withParams = parseWithParams(children);
                return new ApplyTemplatesNode(null, parse(next),
                                              select, mode,
                                              sortKeys, withParams);
              }
            else if ("call-template".equals(name))
              {
                String tname = element.getAttribute("name");
                List withParams = parseWithParams(children);
                return new CallTemplateNode(null, parse(next), tname,
                                            withParams);
              }
            else if ("value-of".equals(name))
              {
                String s = element.getAttribute("select");
                if (s == null || s.length() == 0)
                  {
                    String msg = "select attribute is required on value-of";
                    DOMSourceLocator l = new DOMSourceLocator(element);
                    throw new TransformerConfigurationException(msg, l);
                  }
                Expr select = (Expr) stylesheet.xpath.compile(s);
                String doe = element.getAttribute("disable-output-escaping");
                boolean d = "yes".equals(doe);
                return new ValueOfNode(null, parse(next), select, d);
              }
            else if ("for-each".equals(name))
              {
                String s = element.getAttribute("select");
                if (s == null || s.length() == 0)
                  {
                    String msg = "select attribute is required on for-each";
                    DOMSourceLocator l = new DOMSourceLocator(element);
                    throw new TransformerConfigurationException(msg, l);
                  }
                Expr select = (Expr) stylesheet.xpath.compile(s);
                List sortKeys = parseSortKeys(children);
                return new ForEachNode(parse(children), parse(next), select, sortKeys);
              }
            else if ("sort".equals(name))
              {
                return new DummyNode(null, parse(next));
              }
            else if ("if".equals(name))
              {
                String t = element.getAttribute("test");
                if (t == null || t.length() == 0)
                  {
                    String msg = "test attribute is required on if";
                    DOMSourceLocator l = new DOMSourceLocator(element);
                    throw new TransformerConfigurationException(msg, l);
                  }
                Expr test = (Expr) stylesheet.xpath.compile(t);
                return new IfNode(parse(children), parse(next), test);
              }
            else if ("choose".equals(name))
              {
                return new ChooseNode(parse(children), parse(next));
              }
            else if ("when".equals(name))
              {
                String t = element.getAttribute("test");
                if (t == null || t.length() == 0)
                  {
                    String msg = "test attribute is required on when";
                    DOMSourceLocator l = new DOMSourceLocator(element);
                    throw new TransformerConfigurationException(msg, l);
                  }
                Expr test = (Expr) stylesheet.xpath.compile(t);
                return new WhenNode(parse(children), parse(next), test);
              }
            else if ("otherwise".equals(name))
              {
                return new OtherwiseNode(parse(children), parse(next));
              }
            else if ("element".equals(name))
              {
                String ename = element.getAttribute("name");
                String ns = element.getAttribute("namespace");
                String uas = element.getAttribute("used-attribute-sets");
                // TODO uas
                return new ElementNode(parse(children), parse(next),
                                       ename, ns);
              }
            else if ("attribute".equals(name))
              {
                String aname = element.getAttribute("name");
                String ns = element.getAttribute("namespace");
                return new AttributeNode(parse(children), parse(next),
                                         aname, ns);
              }
            else if ("text".equals(name))
              {
                String doe = element.getAttribute("disable-output-escaping");
                boolean d = "yes".equals(doe);
                return new TextNode(parse(children), parse(next), d);
              }
            else if ("copy".equals(name))
              {
                return new CopyNode(parse(children), parse(next));
              }
            else if ("processing-instruction".equals(name))
              {
                String piname = element.getAttribute("name");
                return new ProcessingInstructionNode(parse(children),
                                                     parse(next), piname);
              }
            else if ("comment".equals(name))
              {
                return new CommentNode(parse(children), parse(next));
              }
            else if ("number".equals(name))
              {
                String v = element.getAttribute("value");
                String format = element.getAttribute("format");
                if (format == null || format.length() == 0)
                  {
                    format = "1";
                  }
                String lang = element.getAttribute("lang");
                if (lang != null && lang.length() == 0)
                  {
                    lang = null;
                  }
                String lv = element.getAttribute("letter-value");
                int letterValue = "traditional".equals(lv) ?
                  AbstractNumberNode.TRADITIONAL :
                  AbstractNumberNode.ALPHABETIC;
                String gs = element.getAttribute("grouping-separator");
                String gz = element.getAttribute("grouping-size");
                int gz2 = Integer.parseInt(gz);
                if (v != null && v.length() > 0)
                  {
                    Expr value = (Expr) stylesheet.xpath.compile(v);
                    return new NumberNode(parse(children), parse(next),
                                          value, format, lang,
                                          letterValue, gs, gz2);
                  }
                else
                  {
                    String l = element.getAttribute("level");
                    int level =
                      "multiple".equals(l) ? NodeNumberNode.MULTIPLE :
                      "any".equals(l) ? NodeNumberNode.ANY :
                      NodeNumberNode.SINGLE;
                    String c = element.getAttribute("count");
                    if (c != null && c.length() > 0)
                      {
                        c = "countable()";
                      }
                    Expr count = (Expr) stylesheet.xpath.compile(c);
                    String f = element.getAttribute("from");
                    if (f == null || f.length() > 0)
                      {
                        f = ".";
                      }
                    Expr from = (Expr) stylesheet.xpath.compile(f);
                    return new NodeNumberNode(parse(children), parse(next),
                                              level, count, from,
                                              format, lang,
                                              letterValue, gs, gz2);
                  }
              }
            else if ("param".equals(name) ||
                     "variable".equals(name))
              {
                boolean global = "variable".equals(name);
                TemplateNode content = parse(children);
                String paramName = element.getAttribute("name");
                if (paramName.length() == 0)
                  {
                    paramName = null;
                  }
                String select = element.getAttribute("select");
                if (select != null && select.length() > 0)
                  {
                    if (content != null)
                      {
                        String msg = "parameter '" + paramName +
                          "' has both select and content";
                        DOMSourceLocator l = new DOMSourceLocator(element);
                        throw new TransformerConfigurationException(msg, l);
                      }
                    Expr expr = (Expr) stylesheet.xpath.compile(select);
                    return new ParameterNode(content, parse(next),
                                             paramName, expr, global);
                  }
                else
                  {
                    return new ParameterNode(content, parse(next),
                                             paramName, null, global);
                  }
              }
            else if ("copy-of".equals(name))
              {
                String s = element.getAttribute("select");
                if (s == null || s.length() == 0)
                  {
                    String msg = "select attribute is required on copy-of";
                    DOMSourceLocator l = new DOMSourceLocator(element);
                    throw new TransformerConfigurationException(msg, l);
                  }
                Expr select = (Expr) stylesheet.xpath.compile(s);
                return new CopyOfNode(parse(children), parse(next), select);
              }
          }
        switch (source.getNodeType())
          {
          case Node.TEXT_NODE:
            // Determine whether to strip whitespace
            Text text = (Text) source;
            if (!stylesheet.isPreserved(text))
              {
                return parse(next);
              }
            break;
          case Node.COMMENT_NODE:
            // Ignore comments
            return parse(next);
          case Node.ELEMENT_NODE:
            // Check for attribute value templates
            NamedNodeMap attrs = source.getAttributes();
            boolean convert = false;
            int len = attrs.getLength();
            for (int i = 0; i < len; i++)
              {
                Node attr = attrs.item(i);
                String value = attr.getNodeValue();
                int start = value.indexOf('{');
                int end = value.indexOf('}');
                if (start != -1 && end > start)
                  {
                    convert = true;
                    break;
                  }
              }
            if (convert)
              {
                // Create an element-producing template node instead
                // with appropriate attribute-producing child template nodes
                TemplateNode child = parse(children);
                for (int i = 0; i < len; i++)
                  {
                    Node attr = attrs.item(i);
                    String value = attr.getNodeValue();
                    int start = value.lastIndexOf('{');
                    int end = value.lastIndexOf('}');
                    TemplateNode grandchild = null;
                    Document doc = source.getOwnerDocument();
                    if (start != -1 && end > start)
                      {
                        while (value.length() > 0 &&
                               start != -1 && end > start)
                          {
                            // Verbatim text at end
                            String sub = value.substring(end + 1);
                            if (sub.length() > 0)
                              {
                                grandchild =
                                  new LiteralNode(null, grandchild,
                                                  doc.createTextNode(sub));
                              }
                            // Expression text
                            String expr = value.substring(start + 1, end);
                            if (expr.length() == 0)
                              {
                                String msg = "attribute value template " +
                                  "must contain expression";
                                DOMSourceLocator l =
                                  new DOMSourceLocator(source);
                                throw new TransformerConfigurationException(msg,
                                                                            l);
                              }
                            Expr select = (Expr) stylesheet.xpath.compile(expr);
                            grandchild = new ValueOfNode(null, grandchild,
                                                         select, false);
                            // work backwards through the text
                            value = value.substring(0, start);
                            start = value.lastIndexOf('{');
                            end = value.lastIndexOf('}');
                          }
                        if (value.length() > 0)
                          {
                            // Verbatim text at beginning
                            grandchild =
                              new LiteralNode(null, grandchild,
                                              doc.createTextNode(value));
                          }
                      }
                    else
                      {
                        // Verbatim
                        grandchild = new LiteralNode(null, grandchild,
                                                     doc.createTextNode(value));
                      }
                    child = new AttributeNode(grandchild, child,
                                              attr.getNodeName(),
                                              attr.getNamespaceURI());
                  }
                return new ElementNode(child, parse(next),
                                       source.getNodeName(),
                                       namespaceUri);
              }
            // Otherwise fall through
            break;
          }
      }
    catch (XPathExpressionException e)
      {
        DOMSourceLocator l = new DOMSourceLocator(source);
        throw new TransformerConfigurationException(e.getMessage(), l, e);
      }
    return new LiteralNode(parse(children), parse(next), source);
  }

  List parseSortKeys(Node node)
    throws XPathExpressionException
  {
    List ret = new LinkedList();
    while (node != null)
      {
        String namespaceUri = node.getNamespaceURI();
        if (Stylesheet.XSL_NS.equals(namespaceUri) &&
            Node.ELEMENT_NODE == node.getNodeType())
          {
            Element element = (Element) node;
            String name = element.getLocalName();

            if ("sort".equals(name))
              {
                String s = element.getAttribute("select");
                Expr select = (Expr) stylesheet.xpath.compile(s);
                String lang = element.getAttribute("lang");
                String dataType = element.getAttribute("data-type");
                String order = element.getAttribute("order");
                boolean descending = "descending".equals(order);
                String caseOrder = element.getAttribute("case-order");
                int co =
                  "upper-first".equals(caseOrder) ? SortKey.UPPER_FIRST :
                  "lower-first".equals(caseOrder) ? SortKey.LOWER_FIRST :
                  SortKey.DEFAULT;
                ret.add(new SortKey(select, lang, dataType, descending, co));
              }
          }
    
        node = node.getNextSibling();
      }
    return ret.isEmpty() ? null : ret;
  }

  List parseWithParams(Node node)
    throws TransformerConfigurationException, XPathExpressionException
  {
    List ret = new LinkedList();
    while (node != null)
      {
        String namespaceUri = node.getNamespaceURI();
        if (Stylesheet.XSL_NS.equals(namespaceUri) &&
            Node.ELEMENT_NODE == node.getNodeType())
          {
            Element element = (Element) node;
            String name = element.getLocalName();

            if ("with-param".equals(name))
              {
                TemplateNode content = parse(element.getFirstChild());
                String paramName = element.getAttribute("name");
                if (paramName.length() == 0)
                  {
                    paramName = null;
                  }
                String select = element.getAttribute("select");
                if (select != null && select.length() > 0)
                  {
                    if (content != null)
                      {
                        String msg = "parameter '" + paramName +
                          "' has both select and content";
                        DOMSourceLocator l = new DOMSourceLocator(element);
                        throw new TransformerConfigurationException(msg, l);
                      }
                    Expr expr = (Expr) stylesheet.xpath.compile(select);
                    ret.add(new WithParam(paramName, expr));
                  }
                else
                  {
                    ret.add(new WithParam(paramName, content));
                  }
              }
          }
        node = node.getNextSibling();
      }
    return ret.isEmpty() ? null : ret;
  }

  public String toString()
  {
    StringBuffer buf = new StringBuffer(getClass().getName());
    buf.append('[');
    if (name != null)
      {
        buf.append("name=");
        buf.append(name);
      }
    else if (match != null)
      {
        buf.append("match=");
        buf.append(match);
      }
    buf.append(']');
    return buf.toString();
  }

  void list(PrintStream out)
  {
    out.println(toString());
    if (node != null)
      {
        node.list(1, out, true);
      }
  }

}
