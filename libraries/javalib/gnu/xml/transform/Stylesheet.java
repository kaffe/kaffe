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
import javax.xml.transform.TransformerConfigurationException;
import javax.xml.transform.TransformerException;
import javax.xml.xpath.XPathConstants;
import javax.xml.xpath.XPathExpressionException;
import org.w3c.dom.Attr;
import org.w3c.dom.Document;
import org.w3c.dom.DOMException;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import gnu.xml.xpath.XPathImpl;

/**
 * An XSL stylesheet.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class Stylesheet
{

  static final String XSL_NS = "http://www.w3.org/1999/XSL/Transform";
  
  static final int OUTPUT_XML = 0;
  static final int OUTPUT_HTML = 1;
  static final int OUTPUT_TEXT = 2;

  final XPathImpl xpath;

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

  // TODO keys
  // TODO decimal-format
  // TODO namespace-alias
  // TODO attribute-set

  /**
   * Variables (cannot be overridden by parameters)
   */
  Map variables;

  /**
   * Parameters (can be overridden)
   */
  Map parameters;

  /**
   * Templates.
   */
  List templates;

  Stylesheet(XPathImpl xpath)
    throws TransformerConfigurationException
  {
    this(xpath, null, 0);
  }

  Stylesheet(XPathImpl xpath, Document doc, int precedence)
    throws TransformerConfigurationException
  {
    this.xpath = xpath;
    stripSpace = new LinkedHashSet();
    preserveSpace = new LinkedHashSet();
    variables = new LinkedHashMap();
    parameters = new LinkedHashMap();
    templates = new LinkedList();
    parse(xpath, doc, precedence);
  }

  void parse(XPathImpl xpath, Document doc, int precedence)
    throws TransformerConfigurationException
  {
    try
      {
        if (doc == null)
          {
            // Identity transformation
            templates.add(new Template(xpath,
                                       "/descendant-or-self::node()",
                                       null,
                                       precedence, Template.DEFAULT_PRIORITY,
                                       null));
            return;
          }
        Element root = doc.getDocumentElement();
        String namespaceUri = root.getNamespaceURI();
        if (XSL_NS.equals(namespaceUri) &&
            "stylesheet".equals(root.getLocalName()))
          {
            // Stylesheet element
            version = root.getAttribute("version");

            NodeList rc = root.getChildNodes();
            int len = rc.getLength();
            for (int i = 0; i < len; i++)
              {
                Node child = rc.item(i);
                if (Node.ELEMENT_NODE == child.getNodeType())
                  {
                    Element element = (Element) child;
                    namespaceUri = element.getNamespaceURI();
                    if (XSL_NS.equals(namespaceUri))
                      {
                        String name = element.getLocalName();
                        if ("template".equals(name))
                          {
                            Collection nodeset = getNodeSet(element);
                            String match = element.getAttribute("match");
                            String priority = element.getAttribute("priority");
                            String mode = element.getAttribute("mode");
                            double p = (priority == null ||
                                        priority.length() == 0) ?
                              Template.DEFAULT_PRIORITY :
                              Double.parseDouble(priority);
                            templates.add(new Template(xpath,
                                                       match,
                                                       nodeset,
                                                       precedence,
                                                       p,
                                                       mode));
                          }
                        else if ("param".equals(name) ||
                                 "variable".equals(name))
                          {
                            Map target = "variable".equals(name) ?
                              variables : parameters;
                            Collection nodeset = getNodeSet(element);
                            String paramName = element.getAttribute("name");
                            String select = element.getAttribute("select");
                            if (select != null)
                              {
                                if (!nodeset.isEmpty())
                                  {
                                    throw new TransformerConfigurationException("parameter has both select and content", new DOMSourceLocator(element));
                                  }
                                target.put(paramName, xpath.compile(select));
                              }
                            else if (!nodeset.isEmpty())
                              {
                                target.put(paramName, nodeset);
                              }
                            else
                              {
                                target.put(paramName, "");
                              }
                          }
                        else if ("include".equals(name))
                          {
                            // TODO
                          }
                        else if ("import".equals(name))
                          {
                            // TODO
                          }
                        else if ("output".equals(name))
                          {
                            String method = element.getAttribute("method");
                            if ("xml".equals(method))
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
                          }
                        else if ("preserve-space".equals(name))
                          {
                            String elements = element.getAttribute("elements");
                            StringTokenizer st = new StringTokenizer(elements,
                                                                     " ");
                            while (st.hasMoreTokens())
                              {
                                preserveSpace.add(QName.valueOf(st.nextToken()));
                              }
                          }
                        else if ("strip-space".equals(name))
                          {
                            String elements = element.getAttribute("elements");
                            StringTokenizer st = new StringTokenizer(elements,
                                                                     " ");
                            while (st.hasMoreTokens())
                              {
                                stripSpace.add(QName.valueOf(st.nextToken()));
                              }
                          }
                        // TODO keys
                        // TODO decimal-format
                        // TODO namespace-alias
                        // TODO attribute-set
                      }
                  }
              }
          }
        else
          {
            // Literal document element
            Attr versionNode = root.getAttributeNodeNS(XSL_NS, "version");
            if (versionNode == null)
              {
                throw new TransformerConfigurationException("no xsl:version attribute on literal result node", new DOMSourceLocator(root));
              }
            version = versionNode.getValue();
            Node rootClone = root.cloneNode(true);
            NamedNodeMap attrs = rootClone.getAttributes();
            attrs.removeNamedItemNS(XSL_NS, "version");
            templates.add(new Template(xpath, "/",
                                       Collections.singletonList(rootClone),
                                       precedence, Template.DEFAULT_PRIORITY,
                                       null));
          }
      }
    catch (DOMException e)
      {
        throw new TransformerConfigurationException(e);
      }
    catch (XPathExpressionException e)
      {
        throw new TransformerConfigurationException(e);
      }
  }

  Collection getNodeSet(Node parent)
  {
    Collection nodeset = new LinkedList();
    NodeList tc = parent.getChildNodes();
    int tl = tc.getLength();
    for (int j = 0; j < tl; j++)
      {
        nodeset.add(tc.item(j));
      }
    return nodeset;
  }

  void applyTemplates(Node context, String select, String mode,
                      Node parent, Node nextSibling)
    throws TransformerException
  {
    if (select == null || select.length() == 0)
      {
        select = "child::node()";
      }
    try
      {
        Object ret = xpath.evaluate(select, context,
                                    XPathConstants.NODESET);
        //System.out.println("applyTemplates: "+select+" selected "+ret);
        if (ret != null && ret instanceof Collection)
          {
            Collection ns = (Collection) ret;
            // TODO sort
            for (Iterator i = ns.iterator(); i.hasNext(); )
              {
                Node subject = (Node) i.next();
                applyTemplates(context, subject, mode, parent, nextSibling);
              }
          }
      }
    catch (XPathExpressionException e)
      {
        throw new TransformerException(select, e);
      }
  }

  void applyTemplates(Node context, Node subject, String mode,
                      Node parent, Node nextSibling)
    throws TransformerException
  {
    //System.out.println("applyTemplates: subject="+subject);
    Set candidates = new TreeSet();
    for (Iterator j = templates.iterator(); j.hasNext(); )
      {
        Template t = (Template) j.next();
        if (t.matches(context, subject, mode))
          {
            candidates.add(t);
          }
      }
    //System.out.println("applyTemplates: candidates="+candidates);
    if (!candidates.isEmpty())
      {
        Template t =
          (Template) candidates.iterator().next();
        //System.out.println("applyTemplates: template="+t.expr+" subject="+subject);
        t.apply(this, subject, mode, parent, nextSibling);
      }
  }
    
}
