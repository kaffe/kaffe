/* DomHTMLDocument.java -- 
   Copyright (C) 2005 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

package gnu.xml.dom.html2;

import gnu.xml.dom.DomDocument;
import gnu.xml.dom.DomDOMException;
import java.lang.reflect.Constructor;
import java.net.MalformedURLException;
import java.net.URL;
import java.util.Collections;
import java.util.HashMap;
import java.util.Map;
import org.w3c.dom.DOMException;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.html2.HTMLCollection;
import org.w3c.dom.html2.HTMLDocument;
import org.w3c.dom.html2.HTMLElement;

/**
 * An HTML document.
 * This is the factory object used to create HTML elements.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class DomHTMLDocument
  extends DomDocument
  implements HTMLDocument
{

  private static final Class[] ELEMENT_PT = new Class[] {
    DomHTMLDocument.class,
    String.class,
    String.class
  };

  private static Map ELEMENT_CLASSES;
  static
  {
    Map map = new HashMap();
    map.put("a", DomHTMLAnchorElement.class);
    map.put("applet", DomHTMLAppletElement.class);
    map.put("area", DomHTMLAreaElement.class);
    map.put("base", DomHTMLBaseElement.class);
    map.put("basefont", DomHTMLBaseFontElement.class);
    map.put("body", DomHTMLBodyElement.class);
    map.put("br", DomHTMLBRElement.class);
    map.put("button", DomHTMLButtonElement.class);
    // TODO others
    ELEMENT_CLASSES = Collections.unmodifiableMap(map);
  }

  private Node getChildNodeByName(Node parent, String name)
  {
    for (Node ctx = parent.getFirstChild(); ctx != null;
         ctx = ctx.getNextSibling())
      {
        if (name.equalsIgnoreCase(ctx.getNodeName()))
          {
            return ctx;
          }
      }
    return null;
  }

  public String getTitle()
  {
    Node html = getDocumentElement();
    if (html != null)
      {
        Node head = getChildNodeByName(html, "head");
        if (head != null)
          {
            Node title = getChildNodeByName(head, "title");
            if (title != null)
              {
                return title.getTextContent();
              }
          }
      }
    return null;
  }

  public void setTitle(String title)
  {
    Node html = getDocumentElement();
    if (html == null)
      {
        html = createElement("html");
        appendChild(html);
      }
    Node head = getChildNodeByName(html, "head");
    if (head == null)
      {
        head = createElement("head");
        Node first = html.getFirstChild();
        if (first != null)
          {
            html.insertBefore(first, head);
          }
        else
          {
            html.appendChild(head);
          }
      }
    Node titleNode = getChildNodeByName(head, "title");
    if (titleNode == null)
      {
        titleNode = createElement("title");
        Node first = head.getFirstChild();
        if (first != null)
          {
            head.insertBefore(first, titleNode);
          }
        else
          {
            head.appendChild(titleNode);
          }
      }
    titleNode.setTextContent(title);
  }

  public String getReferrer()
  {
    // TODO
    return null;
  }

  public String getDomain()
  {
    try
      {
        URL url = new URL(getDocumentURI());
        return url.getHost();
      }
    catch (MalformedURLException e)
      {
        return null;
      }
  }

  public String getURL()
  {
    return getDocumentURI();
  }

  public HTMLElement getBody()
  {
    Node html = getDocumentElement();
    if (html != null)
      {
        Node body = getChildNodeByName(html, "body");
        if (body == null)
          {
            body = getChildNodeByName(html, "frameset");
          }
        return (HTMLElement) body;
      }
    return null;
  }

  public void setBody(HTMLElement body)
  {
    // TODO
  }

  public HTMLCollection getImages()
  {
    DomHTMLCollection ret = new DomHTMLCollection(this);
    ret.addNodeName("img");
    return ret;
  }

  public HTMLCollection getApplets()
  {
    DomHTMLCollection ret = new DomHTMLCollection(this);
    ret.addNodeName("object");
    ret.addNodeName("applet");
    return ret;
  }

  public HTMLCollection getLinks()
  {
    DomHTMLCollection ret = new DomHTMLCollection(this);
    ret.addNodeName("area");
    ret.addNodeName("a");
    return ret;
  }

  public HTMLCollection getForms()
  {
    DomHTMLCollection ret = new DomHTMLCollection(this);
    ret.addNodeName("form");
    return ret;
  }

  public HTMLCollection getAnchors()
  {
    DomHTMLCollection ret = new DomHTMLCollection(this);
    ret.addNodeName("a");
    ret.addAttributeName("name");
    return ret;
  }

  public String getCookie()
  {
    // TODO
    return null;
  }

  public void setCookie(String cookie)
  {
    // TODO
  }

  public void open()
  {
    // TODO
  }

  public void close()
  {
    // TODO
  }

  public void write(String text)
  {
    // TODO
  }

  public void writeln(String text)
  {
    // TODO
  }

  public NodeList getElementsByName(String name)
  {
    DomHTMLCollection ret = new DomHTMLCollection(this);
    ret.addNodeName(name);
    return ret;
    // TODO xhtml: return only form controls (?)
  }

  public Element createElement(String tagName)
  {
    return createElementNS(null, tagName);
  }

  public Element createElementNS(String uri, String qName)
  {
    String localName = qName.toLowerCase();
    int ci = qName.indexOf(':');
    if (ci != -1)
      {
        localName = qName.substring(ci + 1);
      }
    Class t = (Class) ELEMENT_CLASSES.get(localName);
    if (t == null)
      {
        return super.createElementNS(uri, qName);
      }
    try
      {
        Constructor c = t.getConstructor(ELEMENT_PT);
        Object[] args = new Object[] { this, uri, qName };
        return (Element) c.newInstance(args);
      }
    catch (Exception e)
      {
        DOMException e2 = new DomDOMException(DOMException.TYPE_MISMATCH_ERR);
        e2.initCause(e);
        throw e2;
      }
  }

}

