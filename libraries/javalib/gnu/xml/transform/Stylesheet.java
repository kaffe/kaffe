/*
 * Stylesheet.java
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
import java.util.LinkedHashMap;
import java.util.LinkedHashSet;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.TreeSet;
import javax.xml.namespace.QName;
import javax.xml.transform.Source;
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathFunction;
import javax.xml.xpath.XPathFunctionResolver;
import javax.xml.xpath.XPathExpressionException;
import org.w3c.dom.Attr;
import org.w3c.dom.Document;
import org.w3c.dom.DOMException;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.Text;
import gnu.xml.xpath.Expr;
import gnu.xml.xpath.NameTest;
import gnu.xml.xpath.Selector;
import gnu.xml.xpath.Root;
import gnu.xml.xpath.XPathImpl;

/**
 * An XSL stylesheet.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class Stylesheet
  implements XPathFunctionResolver, Cloneable
{

  static final String XSL_NS = "http://www.w3.org/1999/XSL/Transform";
  
  static final int OUTPUT_XML = 0;
  static final int OUTPUT_HTML = 1;
  static final int OUTPUT_TEXT = 2;

  final TransformerFactoryImpl factory;
  TransformerImpl transformer;
  final XPathImpl xpath;
  final String systemId;
  final int precedence;

  /**
   * Version of XSLT.
   */
  String version;

  /**
   * Set of element names for which we should strip whitespace.
   */
  Set stripSpace;

  /**
   * Set of element names for which we should preserve whitespace.
   */
  Set preserveSpace;

  /**
   * Output method.
   */
  int outputMethod;
  String outputPublicId;
  String outputSystemId;
  String outputEncoding;
  boolean outputIndent;

  // TODO keys
  // TODO decimal-format
  // TODO namespace-alias

  /**
   * Attribute-sets.
   */
  Map attributeSets;
  Map usedAttributeSets;

  /**
   * Variable and parameter bindings.
   */
  Bindings bindings;

  /**
   * Templates.
   */
  List templates;

  TemplateNode builtInNodeTemplate;
  TemplateNode builtInTextTemplate;

  /**
   * Holds the current node while parsing.
   * Necessary to associate the document function with its declaring node.
   */
  transient Node current;

  Stylesheet(TransformerFactoryImpl factory,
             Stylesheet parent,
             Document doc,
             String systemId,
             int precedence)
    throws TransformerConfigurationException
  {
    this.factory = factory;
    this.systemId = systemId;
    this.precedence = precedence;
    stripSpace = new LinkedHashSet();
    preserveSpace = new LinkedHashSet();
    attributeSets = new LinkedHashMap();
    usedAttributeSets = new LinkedHashMap();
    if (parent == null)
      {
        bindings = new Bindings();
        templates = new LinkedList();
      }
    else
      {
        bindings = parent.bindings;
        templates = parent.templates;
      }

    factory.xpathFactory.setXPathVariableResolver(bindings);
    factory.xpathFactory.setXPathFunctionResolver(this);
    xpath = (XPathImpl) factory.xpathFactory.newXPath();

    builtInNodeTemplate =
      new ApplyTemplatesNode(null, null,
                             new Selector(Selector.CHILD,
                                          Collections.EMPTY_LIST),
                             null, null, null);
    builtInTextTemplate =
      new ValueOfNode(null, null,
                      new Selector(Selector.SELF, Collections.EMPTY_LIST),
                      false);
    
    parse(doc.getDocumentElement(), true);
    
    /*for (Iterator i = templates.iterator(); i.hasNext(); )
      {
        Template t = (Template) i.next();
        t.list(System.out);
        System.out.println("--------------------");
      }
      */
  }

  public Object clone()
  {
    try
      {
        Stylesheet clone = (Stylesheet) super.clone();
        clone.bindings = (Bindings) bindings.clone();
        return clone;
      }
    catch (CloneNotSupportedException e)
      {
        throw new Error(e.getMessage());
      }
  }

  void parse(Node node, boolean root)
    throws TransformerConfigurationException
  {
    current = node;
    if (node == null)
      {
        return;
      }
    try
      {
        String namespaceUri = node.getNamespaceURI();
        if (XSL_NS.equals(namespaceUri) &&
            node.getNodeType() == Node.ELEMENT_NODE)
          {
            Element element = (Element) node;
            String name = element.getLocalName();
            if ("stylesheet".equals(name))
              {
                version = element.getAttribute("version");
                parse(element.getFirstChild(), false);
              }
            else if ("template".equals(name))
              {
                String tname = element.getAttribute("name");
                if (tname.length() == 0)
                  {
                    tname = null;
                  }
                String m = element.getAttribute("match");
                Expr match = (m != null && m.length() > 0) ?
                  (Expr) xpath.compile(m) : null;
                String priority = element.getAttribute("priority");
                String mode = element.getAttribute("mode");
                double p = (priority == null ||
                            priority.length() == 0) ?
                  Template.DEFAULT_PRIORITY :
                  Double.parseDouble(priority);
                templates.add(new Template(this,
                                           tname,
                                           match,
                                           element.getFirstChild(),
                                           precedence,
                                           p,
                                           mode));
                parse(element.getNextSibling(), false);
              }
            else if ("param".equals(name) ||
                     "variable".equals(name))
              {
                boolean global = "variable".equals(name);
                Object content = element.getFirstChild();
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
                    content = xpath.compile(select);
                  }
                if (content == null)
                  {
                    content = "";
                  }
                bindings.set(paramName, content, global);
                parse(element.getNextSibling(), false);
              }
            else if ("include".equals(name) || "import".equals(name))
              {
                int p = "import".equals(name) ? -1 : 0;
                String href = element.getAttribute("href");
								Source source;
								synchronized (factory.resolver)
								  {
										if (transformer != null)
										  {
												factory.resolver.setUserResolver(transformer.getURIResolver());
												factory.resolver.setUserListener(transformer.getErrorListener());
											}
										source = factory.resolver.resolve(systemId, href);
									}
                Stylesheet stylesheet =
                  factory.newStylesheet(source, precedence + p, this);
                parse(element.getNextSibling(), false);
              }
            else if ("output".equals(name))
              {
                String method = element.getAttribute("method");
                if ("xml".equals(method) || method.length() == 0)
                  {
                    outputMethod = OUTPUT_XML;
                  }
                else if ("html".equals(method))
                  {
                    outputMethod = OUTPUT_HTML;
                  }
                else if ("text".equals(method))
                  {
                    outputMethod = OUTPUT_TEXT;
                  }
                else
                  {
                    throw new TransformerConfigurationException("unsupported output method: " + method, new DOMSourceLocator(element));
                  }
                outputPublicId = element.getAttribute("public-id");
                outputSystemId = element.getAttribute("system-id");
                outputEncoding = element.getAttribute("encoding");
                if (outputEncoding.length() == 0)
                  {
                    outputEncoding = null;
                  }
                String indent = element.getAttribute("indent");
                outputIndent = "yes".equals(indent);
                parse(element.getNextSibling(), false);
              }
            else if ("preserve-space".equals(name))
              {
                String elements = element.getAttribute("elements");
                StringTokenizer st = new StringTokenizer(elements,
                                                         " \t\n\r");
                while (st.hasMoreTokens())
                  {
                    preserveSpace.add(parseNameTest(st.nextToken()));
                  }
                parse(element.getNextSibling(), false);
              }
            else if ("strip-space".equals(name))
              {
                String elements = element.getAttribute("elements");
                StringTokenizer st = new StringTokenizer(elements,
                                                         " \t\n\r");
                while (st.hasMoreTokens())
                  {
                    stripSpace.add(parseNameTest(st.nextToken()));
                  }
                parse(element.getNextSibling(), false);
              }
            // TODO keys
            // TODO decimal-format
            // TODO namespace-alias
            else if ("attribute-set".equals(name))
              {
                String asName = element.getAttribute("name");
                String uas = element.getAttribute("use-attribute-sets");
                attributeSets.put(asName, element.getFirstChild());
                if (uas != null)
                  {
                    usedAttributeSets.put(asName, uas);
                  }
              }
            else
              {
                // Forwards-compatible processing: ignore unknown XSL
                // elements
                parse(element.getNextSibling(), false);
              }
          }
        else if (root)
          {
            // Literal document element
            Attr versionNode =
              ((Element)node).getAttributeNodeNS(XSL_NS, "version");
            if (versionNode == null)
              {
                String msg = "no xsl:version attribute on literal result node";
                DOMSourceLocator l = new DOMSourceLocator(node);
                throw new TransformerConfigurationException(msg, l);
              }
            version = versionNode.getValue();
            Node rootClone = node.cloneNode(true);
            NamedNodeMap attrs = rootClone.getAttributes();
            attrs.removeNamedItemNS(XSL_NS, "version");
            templates.add(new Template(this,
                                       null,
                                       new Root(),
                                       rootClone,
                                       precedence,
                                       Template.DEFAULT_PRIORITY,
                                       null));
          }
        else
          {
            // Skip unknown elements, text, comments, etc
            parse(node.getNextSibling(), false);
          }
      }
    catch (TransformerException e)
      {
        DOMSourceLocator l = new DOMSourceLocator(node);
        throw new TransformerConfigurationException(e.getMessage(), l, e);
      }
    catch (DOMException e)
      {
        DOMSourceLocator l = new DOMSourceLocator(node);
        throw new TransformerConfigurationException(e.getMessage(), l, e);
      }
    catch (XPathExpressionException e)
      {
        DOMSourceLocator l = new DOMSourceLocator(node);
        throw new TransformerConfigurationException(e.getMessage(), l, e);
      }
  }

  NameTest parseNameTest(String token)
  {
    if ("*".equals(token))
      {
        return new NameTest("", true, true);
      }
    else if (token.endsWith(":*"))
      {
        return new NameTest(token.substring(0, token.length() - 2),
                            true, false);
      }
    else
      {
        return new NameTest(token, false, false);
      }
  }

  boolean isPreserved(Text text)
  {
    // Check characters in text
    String value = text.getData();
    if (value != null)
      {
        int len = value.length();
        for (int i = 0; i < len; i++)
          {
            char c = value.charAt(i);
            if (c != 0x20 && c != 0x09 && c != 0x0a && c != 0x0d)
              {
                return true;
              }
          }
      }
    // Check parent node
    Node ctx = text.getParentNode();
    for (Iterator i = preserveSpace.iterator(); i.hasNext(); )
      {
        NameTest preserveTest = (NameTest) i.next();
        if (preserveTest.matches(ctx, 1, 1))
          {
            boolean override = false;
            for (Iterator j = stripSpace.iterator(); j.hasNext(); )
              {
                NameTest stripTest = (NameTest) j.next();
                if (stripTest.matches(ctx, 1, 1))
                  {
                    override = true;
                    break;
                  }
              }
            if (!override)
              {
                return true;
              }
          }
      }
    // Check whether any ancestor specified xml:space
    while (ctx != null)
      {
        if (ctx.getNodeType() == Node.ELEMENT_NODE)
          {
            Element element = (Element) ctx;
            String xmlSpace = element.getAttribute("xml:space");
            if ("default".equals(xmlSpace))
              {
                break;
              }
            else if ("preserve".equals(xmlSpace))
              {
                return true;
              }
            else if ("text".equals(ctx.getLocalName()) &&
                     XSL_NS.equals(ctx.getNamespaceURI()))
              {
                // xsl:text implies xml:space='preserve'
                return true;
              }
          }
        ctx = ctx.getParentNode();
      }
    return false;
  }

  void applyTemplates(Expr select, String mode,
                      Node context, int pos, int len,
                      Node parent, Node nextSibling)
    throws TransformerException
  {
    Object ret = select.evaluate(context, pos, len);
    if (ret != null && ret instanceof Collection)
      {
        Collection ns = (Collection) ret;
        int l = ns.size();
        int p = 1;
        for (Iterator i = ns.iterator(); i.hasNext(); )
          {
            Node subject = (Node) i.next();
            applyTemplates(mode,
                           context, subject, p++, l,
                           parent, nextSibling);
          }
      }
  }

  void applyTemplates(String mode,
                      Node context, Node subject, int pos, int len,
                      Node parent, Node nextSibling)
    throws TransformerException
  {
    //System.out.println("applyTemplates:");
    //System.out.println("\tsubject="+subject);
    Set candidates = new TreeSet();
    for (Iterator j = templates.iterator(); j.hasNext(); )
      {
        Template t = (Template) j.next();
        boolean isMatch = t.matches(mode, context, subject, pos, len);
        //System.out.println("applyTemplates: "+subject+" "+t+"="+isMatch);
        if (isMatch)
          {
            candidates.add(t);
          }
      }
    //System.out.println("\tcandidates="+candidates);
    if (candidates.isEmpty())
      {
        // Apply built-in template
        //System.out.println("\tbuiltInTemplate subject="+subject);
        switch (subject.getNodeType())
          {
          case Node.ELEMENT_NODE:
          case Node.DOCUMENT_NODE:
          case Node.DOCUMENT_FRAGMENT_NODE:
            builtInNodeTemplate.apply(this, mode,
                                      subject, pos, len,
                                      parent, nextSibling);
            break;
          case Node.TEXT_NODE:
          case Node.ATTRIBUTE_NODE:
            builtInTextTemplate.apply(this, mode,
                                      subject, pos, len,
                                      parent, nextSibling);
            break;
          }
      }
    else
      {
        Template t =
          (Template) candidates.iterator().next();
        //System.out.println("\ttemplate="+t+" subject="+subject);
        t.apply(this, mode,
                subject, pos, len,
                parent, nextSibling);
      }
  }

  void callTemplate(String name, String mode,
                    Node context, int pos, int len,
                    Node parent, Node nextSibling)
    throws TransformerException
  {
    
    for (Iterator j = templates.iterator(); j.hasNext(); )
      {
        Template t = (Template) j.next();
        if (name.equals(t.name))
          {
            //System.err.println("*** calling "+t);
            //System.err.println("*** bindings="+bindings);
            t.apply(this, mode,
                    context, pos, len,
                    parent, nextSibling);
            return;
          }
      }
    throw new TransformerException("template '" + name + "' not found");
  }

  public XPathFunction resolveFunction(QName name, int arity)
  {
    String uri = name.getNamespaceURI();
    if (XSL_NS.equals(uri) || uri == null || uri.length() == 0)
      {
        String localName = name.getLocalName();
        if ("document".equals(localName) && (arity == 1 || arity == 2))
          {
            return new DocumentFunction(this, current);
          }
      }
    return null;
  }
    
}
