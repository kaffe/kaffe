/*
 * DOMSerializer.java
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

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;
import org.w3c.dom.Attr;
import org.w3c.dom.CDATASection;
import org.w3c.dom.Comment;
import org.w3c.dom.Document;
import org.w3c.dom.DocumentFragment;
import org.w3c.dom.DocumentType;
import org.w3c.dom.Element;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.w3c.dom.Text;

/**
 * Serializer for a DOM node.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class DOMSerializer
{
  
  static final int SPACE = 0x20;
  static final int BANG = 0x21; // !
  static final int APOS = 0x27; // '
  static final int SLASH = 0x2f; // /
  static final int BRA = 0x3c; // <
  static final int KET = 0x3e; // >
  static final int EQ = 0x3d; // =

  static void serialize(Node node, OutputStream out)
    throws IOException
  {
    final String charset = "UTF-8";
    NodeList children;
    int len;
    String qName, value;
    byte[] buf;
    switch (node.getNodeType())
      {
      case Node.ATTRIBUTE_NODE:
        Attr attr = (Attr) node;
        // TODO namespaces
        qName = attr.getNodeName();
        buf = qName.getBytes(charset);
        out.write(SPACE);
        out.write(buf);
        out.write(EQ);
        value = "'" + encode(attr.getValue()) + "'";
        buf = value.getBytes(charset);
        out.write(buf);
        break;
      case Node.ELEMENT_NODE:
        Element element = (Element) node;
        // TODO namespaces
        qName = element.getNodeName();
        buf = qName.getBytes(charset);
        out.write(BRA);
        out.write(buf);
        NamedNodeMap attrs = element.getAttributes();
        if (attrs != null)
          {
            len = attrs.getLength();
            for (int i = 0; i < len; i++)
              {
                Node child = attrs.item(i);
                serialize(child, out);
              }
          }
        children = element.getChildNodes();
        len = 0;
        if (children != null)
          {
            len = children.getLength();
            if (len > 0)
              {
                out.write('>');
                for (int i = 0; i < len; i++)
                  {
                    Node child = children.item(i);
                    serialize(child, out);
                  }
              }
          }
        if (len == 0)
          {
            out.write(SLASH);
            out.write(KET);
          }
        else
          {
            out.write(BRA);
            out.write(SLASH);
            out.write(buf);
            out.write(KET);
          }
        break;
      case Node.TEXT_NODE:
        Text text = (Text) node;
        value = encode(text.getData());
        buf = value.getBytes(charset);
        out.write(buf);
        break;
      case Node.CDATA_SECTION_NODE:
        CDATASection cdata = (CDATASection) node;
        value = "<![CDATA[" + cdata.getData() + "]]>";
        buf = value.getBytes(charset);
        out.write(buf);
        break;
      case Node.COMMENT_NODE:
        Comment comment = (Comment) node;
        value = "<!--" + encode(comment.getData()) + "-->";
        buf = value.getBytes(charset);
        out.write(buf);
        break;
      case Node.DOCUMENT_NODE:
        Document doc = (Document) node;
        // TODO XML declaration?
        children = doc.getChildNodes();
        if (children != null)
          {
            len = children.getLength();
            for (int i = 0; i < len; i++)
              {
                Node child = children.item(i);
                serialize(child, out);
              }
          }
        break;
      case Node.DOCUMENT_FRAGMENT_NODE:
        DocumentFragment docFrag = (DocumentFragment) node;
        children = docFrag.getChildNodes();
        if (children != null)
          {
            len = children.getLength();
            for (int i = 0; i < len; i++)
              {
                Node child = children.item(i);
                serialize(child, out);
              }
          }
        break;
      case Node.DOCUMENT_TYPE_NODE:
        DocumentType doctype = (DocumentType) node;
        out.write(BRA);
        out.write(BANG);
        qName = doctype.getNodeName();
        buf = qName.getBytes(charset);
        out.write(buf);
        String publicId = doctype.getPublicId();
        if (publicId != null)
          {
            qName = " PUBLIC ";
            buf = qName.getBytes(charset);
            out.write(buf);
            out.write(APOS);
            buf = publicId.getBytes(charset);
            out.write(buf);
            out.write(APOS);
          }
        String systemId = doctype.getSystemId();
        if (systemId != null)
          {
            qName = " SYSTEM ";
            buf = qName.getBytes(charset);
            out.write(buf);
            out.write(APOS);
            buf = systemId.getBytes(charset);
            out.write(buf);
            out.write(APOS);
          }
        String internalSubset = doctype.getInternalSubset();
        if (internalSubset != null)
          {
            buf = internalSubset.getBytes(charset);
            out.write(buf);
          }
        out.write(KET);
        break;
      case Node.ENTITY_REFERENCE_NODE:
        qName = "&" + node.getNodeValue() + ";";
        buf = qName.getBytes(charset);
        out.write(buf);
        break;
      }
  }

  static String encode(String text)
  {
    int len = text.length();
    StringBuffer buf = null;
    for (int i = 0; i < len; i++)
      {
        char c = text.charAt(i);
        if (c == '<')
          {
            if (buf == null)
              {
                buf = new StringBuffer(text.substring(0, i));
              }
            buf.append("&lt;");
          }
        else if (c == '>')
          {
            if (buf == null)
              {
                buf = new StringBuffer(text.substring(0, i));
              }
            buf.append("&gt;");
          }
        else if (c == '&')
          {
            if (buf == null)
              {
                buf = new StringBuffer(text.substring(0, i));
              }
            buf.append("&amp;");
          }
        else if (c == '\'')
          {
            if (buf == null)
              {
                buf = new StringBuffer(text.substring(0, i));
              }
            buf.append("&apos;");
          }
        else if (c == '"')
          {
            if (buf == null)
              {
                buf = new StringBuffer(text.substring(0, i));
              }
            buf.append("&quot;");
          }
        else if (buf != null)
          {
            buf.append(c);
          }
      }
    return (buf == null) ? text : buf.toString();
  }

  static String toString(Node node)
  {
    ByteArrayOutputStream out = new ByteArrayOutputStream();
    try
      {
        serialize(node, out);
        return new String(out.toByteArray(), "UTF-8");
      }
    catch (IOException e)
      {
        throw new RuntimeException(e.getMessage());
      }
  }
  
}
