/*
 * Expr.java
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

import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
import java.util.LinkedHashSet;
import java.util.Set;
import java.util.StringTokenizer;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * An XPath expression.
 * This can be evaluated in the context of a node to produce a result.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public abstract class Expr
{

  public abstract Object evaluate (Node context);
  
  /* -- 4.1 Node Set Functions -- */

  /**
   * The last function returns a number equal to the context size from the
   * expression evaluation context.
   */
  final double _last (Node context)
  {
    int ret = context.hasChildNodes () ? 0 :
      context.getChildNodes ().getLength ();
    if (ret > 0)
      {
        ret++;
      }
    return (double) ret;
  }

  /**
   * The position function returns a number equal to the context position
   * from the expression evaluation context.
   */
  final double _position (Node context)
  {
    int count = 0;
    while (context != null)
      {
        context = context.getPreviousSibling ();
        count++;
      }
    return (double) count;
  }

  /**
   * The count function returns the number of nodes in the argument
   * node-set.
   */
  final double _count (Node context, Collection nodeSet)
  {
    return (double) nodeSet.size ();
  }

  /**
   * The id function selects elements by their unique ID.
   * When the argument to id is of type node-set, then the result is
   * the union of the result of applying id to the string-value of each of
   * the nodes in the argument node-set. When the argument to id is of any
   * other type, the argument is converted to a string as if by a call to
   * the string function; the string is split into a whitespace-separated
   * list of tokens (whitespace is any sequence of characters matching the
   * production S); the result is a node-set containing the elements in the
   * same document as the context node that have a unique ID equal to any of
   * the tokens in the list.
   */
  final Collection _id (Node context, Object object)
  {
    Set ret = new LinkedHashSet ();
    if (object instanceof Collection)
      {
        Collection nodeSet = (Collection) object;
        for (Iterator i = nodeSet.iterator (); i.hasNext (); )
          {
            String string = _string (context, i.next ());
            ret.addAll (_id (context, string));
          }
      }
    else
      {
        Document doc = context.getOwnerDocument ();
        String string = _string (context, object);
        StringTokenizer st = new StringTokenizer (string, " \t\r\n");
        while (st.hasMoreTokens ())
          {
            Node element = doc.getElementById (st.nextToken ());
            if (element != null)
              {
                ret.add (element);
              }
          }
      }
    return ret;
  }

  /**
   * The local-name function returns the local part of the expanded-name of
   * the node in the argument node-set that is first in document order. If
   * the argument node-set is empty or the first node has no expanded-name,
   * an empty string is returned. If the argument is omitted, it defaults to
   * a node-set with the context node as its only member.
   */
  final String _local_name (Node context, Collection nodeSet)
  {
    if (nodeSet == null || nodeSet.size () == 0)
      {
        return context.getLocalName ();
      }
    Document doc = context.getOwnerDocument ();
    Node node = firstNode (doc, nodeSet);
    return (node == null ) ? null : node.getLocalName ();
  }

  /**
   * The namespace-uri function returns the namespace URI of the
   * expanded-name of the node in the argument node-set that is first in
   * document order. If the argument node-set is empty, the first node has
   * no expanded-name, or the namespace URI of the expanded-name is null, an
   * empty string is returned. If the argument is omitted, it defaults to a
   * node-set with the context node as its only member.
   */
  final String _namespace_uri (Node context, Collection nodeSet)
  {
    if (nodeSet == null || nodeSet.size () == 0)
      {
        return context.getNamespaceURI ();
      }
    Document doc = context.getOwnerDocument ();
    Node node = firstNode (doc, nodeSet);
    return (node == null ) ? null : node.getNamespaceURI ();
  }
  
  /**
   * The name function returns a string containing a QName representing the
   * expanded-name of the node in the argument node-set that is first in
   * document order. The QName must represent the expanded-name with respect
   * to the namespace declarations in effect on the node whose expanded-name
   * is being represented. Typically, this will be the QName that occurred
   * in the XML source. This need not be the case if there are namespace
   * declarations in effect on the node that associate multiple prefixes
   * with the same namespace. However, an implementation may include
   * information about the original prefix in its representation of nodes;
   * in this case, an implementation can ensure that the returned string is
   * always the same as the QName used in the XML source. If the argument
   * node-set is empty or the first node has no expanded-name, an empty
   * string is returned. If the argument it omitted, it defaults to a
   * node-set with the context node as its only member.
   */
  final String _name (Node context, Collection nodeSet)
  {
    if (nodeSet == null || nodeSet.size () == 0)
      {
        return context.getNodeName ();
      }
    Document doc = context.getOwnerDocument ();
    Node node = firstNode (doc, nodeSet);
    return (node == null ) ? null : node.getNodeName ();
  }

  /*
   * Descend the node in document order and return the first matching node
   * in the node-set.
   */
  final Node firstNode (Node node, Collection nodeSet)
  {
    if (nodeSet.contains (node))
      {
        return node;
      }
    NodeList children = node.getChildNodes ();
    int len = children.getLength ();
    for (int i = 0; i < len; i++)
      {
        Node child = children.item (i);
        Node test = firstNode (child, nodeSet);
        if (test != null)
          {
            return test;
          }
      }
    return null;
  }

  /* -- 4.2 String Functions -- */

  /**
   * Implementation of the XPath <code>string</code> function.
   */
  final String _string (Node context, Object object)
  {
    if (object == null)
      {
        return stringValue (context);
      }
    if (object instanceof String)
      {
        return (String) object;
      }
    if (object instanceof Boolean)
      {
        return object.toString ();
      }
    if (object instanceof Double)
      {
        Double d = (Double) object;
        String ret = d.toString ();
        if (ret.endsWith (".0"))
          { 
            ret = ret.substring (0, ret.length () - 2);
          }
        return ret;
      }
    if (object instanceof Collection)
      {
        Collection nodeSet = (Collection) object;
        if (nodeSet.isEmpty ())
          {
            return "";
          }
        Document doc = context.getOwnerDocument ();
        Node node = firstNode (doc, nodeSet);
        return (node == null) ? "" : stringValue (node);
      }
    return null;
  }

  /**
   * The concat function returns the concatenation of its arguments.
   */
  final String _concat (Node context, String s1, String s2)
  {
    return s1 + s2;
  }

  /**
   * The starts-with function returns true if the first argument string
   * starts with the second argument string, and otherwise returns false.
   */
  final boolean _starts_with (Node context, String s1, String s2)
  {
    return s1.startsWith (s2);
  }

  /**
   * The contains function returns true if the first argument string
   * contains the second argument string, and otherwise returns false.
   */
  final boolean _contains (Node context, String s1, String s2)
  {
    return s1.indexOf (s2) != -1;
  }

  /**
   * The substring-before function returns the substring of the first
   * argument string that precedes the first occurrence of the second
   * argument string in the first argument string, or the empty string if
   * the first argument string does not contain the second argument string.
   * For example, substring-before("1999/04/01","/") returns 1999.
   */
  final String _substring_before (Node context, String s1, String s2)
  {
    int index = s1.indexOf (s2);
    return (index == -1) ? "" : s1.substring (0, index);
  }

  /**
   * The substring-after function returns the substring of the first
   * argument string that follows the first occurrence of the second
   * argument string in the first argument string, or the empty string if
   * the first argument string does not contain the second argument string.
   * For example, substring-after("1999/04/01","/") returns 04/01, and
   * substring-after("1999/04/01","19") returns 99/04/01.
   */
  final String _substring_after (Node context, String s1, String s2)
  {
    int index = s1.indexOf (s2);
    return (index == -1) ? "" : s1.substring (index + s2.length ());
  }

  /**
   * The substring function returns the substring of the first argument
   * starting at the position specified in the second argument with length
   * specified in the third argument. For example, substring("12345",2,3)
   * returns "234". If the third argument is not specified, it returns the
   * substring starting at the position specified in the second argument and
   * continuing to the end of the string. For example, substring("12345",2)
   * returns "2345".
   */
  final String _substring (Node context, String s1, double pos, double len)
  {
    int ipos = Math.max (((int) Math.round (pos)) - 1, 0);
    int ilen = Math.min (((int) Math.round (len)) - 1, s1.length ());
    return s1.substring (ipos, ilen);
  }

  /**
   * The string-length returns the number of characters in the string.
   * If the argument is omitted, it defaults to the context
   * node converted to a string, in other words the string-value of the
   * context node.
   */
  final double _string_length (Node context, String string)
  {
    if (string == null)
      {
        string = stringValue (context);
      }
    return (double) string.length ();
  }

  /**
   * The normalize-space function returns the argument string with
   * whitespace normalized by stripping leading and trailing whitespace and
   * replacing sequences of whitespace characters by a single space.
   * Whitespace characters are the same as those allowed by the S production
   * in XML. If the argument is omitted, it defaults to the context node
   * converted to a string, in other words the string-value of the context
   * node.
   */
  final String _normalize_space (Node context, String string)
  {
    if (string == null)
      {
        string = stringValue (context);
      }
    StringTokenizer st = new StringTokenizer (string, " \t\r\n");
    StringBuffer buf = new StringBuffer ();
    if (st.hasMoreTokens ())
      {
        buf.append (st.nextToken ());
        while (st.hasMoreTokens ())
          {
            buf.append (' ');
            buf.append (st.nextToken ());
          }
      }
    return buf.toString ();
  }

  /**
   * The translate function returns the first argument string with
   * occurrences of characters in the second argument string replaced by the
   * character at the corresponding position in the third argument string.
   * For example, translate("bar","abc","ABC") returns the string BAr. If
   * there is a character in the second argument string with no character at
   * a corresponding position in the third argument string (because the
   * second argument string is longer than the third argument string), then
   * occurrences of that character in the first argument string are removed.
   * For example, translate("--aaa--","abc-","ABC") returns "AAA". If a
   * character occurs more than once in the second argument string, then the
   * first occurrence determines the replacement character. If the third
   * argument string is longer than the second argument string, then excess
   * characters are ignored.
   */
  final String _translate (Node context, String string, String search,
                            String replace)
  {
    StringBuffer buf = new StringBuffer ();
    int l1 = string.length ();
    int l2 = search.length ();
    int l3 = replace.length ();
    for (int i = 0; i < l1; i++)
      {
        char c = string.charAt (i);
        boolean replaced = false;
        for (int j = 0; j < l2; j++)
          {
            if (c == search.charAt (j))
              {
                if (j < l3)
                  {
                    buf.append (replace.charAt (j));
                  }
                replaced = true;
              }
          }
        if (!replaced)
          {
            buf.append (c);
          }
      }
    return new String (buf);
  }

  /* -- 4.3 Boolean Functions -- */
  
  /**
   * Implementation of the XPath <code>boolean</code> function.
   */
  final boolean _boolean (Node context, Object object)
  {
    if (object instanceof Boolean)
      {
        return ((Boolean) object).booleanValue ();
      }
    if (object instanceof Double)
      {
        return ((Double) object).doubleValue () != 0.0;
      }
    if (object instanceof String)
      {
        return ((String) object).length () != 0;
      }
    if (object instanceof Collection)
      {
        return ((Collection) object).size () != 0;
      }
    return false; // TODO user defined types
  }

  /**
   * The not function returns true if its argument is false, and false
   * otherwise.
   */
  final boolean _not (Node context, boolean b)
  {
    return !b;
  }

  /**
   * The true function returns true.
   */
  final boolean _true (Node context)
  {
    return true;
  }

  /**
   * The false function returns false.
   */
  final boolean _false (Node context)
  {
    return false;
  }

  /**
   * The lang function returns true or false depending on whether the
   * language of the context node as specified by xml:lang attributes is the
   * same as or is a sublanguage of the language specified by the argument
   * string. The language of the context node is determined by the value of
   * the xml:lang attribute on the context node, or, if the context node has
   * no xml:lang attribute, by the value of the xml:lang attribute on the
   * nearest ancestor of the context node that has an xml:lang attribute. If
   * there is no such attribute, then lang returns false. If there is such
   * an attribute, then lang returns true if the attribute value is equal to
   * the argument ignoring case, or if there is some suffix starting with -
   * such that the attribute value is equal to the argument ignoring that
   * suffix of the attribute value and ignoring case.
   */
  final boolean _lang (Node context, String lang)
  {
    String clang = getLang (context);
    while (clang == null && context != null)
      {
        context = context.getParentNode ();
        clang = getLang (context);
      }
    return (clang == null) ? false :
      clang.toLowerCase ().startsWith (lang.toLowerCase ());
  }

  final String getLang (Node node)
  {
    if (node instanceof Element)
      {
        return ((Element) node).getAttribute ("xml:lang");
      }
    return null;
  }

  /* -- 4.4 Number Functions -- */

  /**
   * Implementation of the XPath <code>number</code> function.
   */
  final double _number (Node context, Object object)
  {
    if (object == null)
      {
        object = Collections.singleton (context);
      }
    if (object instanceof Double)
      {
        return ((Double) object).doubleValue ();
      }
    if (object instanceof Boolean)
      {
        return ((Boolean) object).booleanValue () ? 1.0 : 0.0;
      }
    if (object instanceof Collection)
      {
        // Convert node-set to string
        object = stringValue ((Collection) object);
      }
    if (object instanceof String)
      {
        String string = ((String) object).trim ();
        try
          {
            return Double.parseDouble (string);
          }
        catch (NumberFormatException e)
          {
            return Double.NaN;
          }
      }
    return 0.0; // TODO user-defined types
  }

  /**
   * The sum function returns the sum, for each node in the argument
   * node-set, of the result of converting the string-values of the node to
   * a number.
   */
  final double _sum (Node context, Collection nodeSet)
  {
    double ret = 0.0;
    for (Iterator i = nodeSet.iterator (); i.hasNext (); )
      {
        ret += _number (context, stringValue ((Node) i.next ()));
      }
    return ret;
  }

  /**
   * The floor function returns the largest (closest to positive infinity)
   * number that is not greater than the argument and that is an integer.
   */
  final double _floor (Node context, double number)
  {
    return Math.floor (number);
  }

  /**
   * The ceiling function returns the smallest (closest to negative
   * infinity) number that is not less than the argument and that is an
   * integer.
   */
  final double _ceiling (Node context, double number)
  {
    return Math.ceil (number);
  }

  /**
   * The round function returns the number that is closest to the argument
   * and that is an integer. If there are two such numbers, then the one
   * that is closest to positive infinity is returned. If the argument is
   * NaN, then NaN is returned. If the argument is positive infinity, then
   * positive infinity is returned. If the argument is negative infinity,
   * then negative infinity is returned. If the argument is positive zero,
   * then positive zero is returned. If the argument is negative zero, then
   * negative zero is returned. If the argument is less than zero, but
   * greater than or equal to -0.5, then negative zero is returned.
   */
  final double _round (Node context, double number)
  {
    return (double) Math.round (number);
  }

  /**
   * Computes the XPath string-value of the specified node-set.
   */
  final String stringValue (Collection nodeSet)
  {
    StringBuffer buf = new StringBuffer ();
    for (Iterator i = nodeSet.iterator (); i.hasNext (); )
      {
        buf.append (stringValue ((Node) i.next ()));
      }
    return buf.toString ();
  }

  /**
   * Computes the XPath string-value of the specified node.
   */
  final String stringValue (Node node)
  {
    return stringValue (node, false);
  }
  
  final String stringValue (Node node, boolean elementMode)
  {
    switch (node.getNodeType ())
      {
      case Node.DOCUMENT_NODE: // 5.1 Root Node
      case Node.ELEMENT_NODE: // 5.2 Element Nodes
        StringBuffer buf = new StringBuffer ();
        NodeList children = node.getChildNodes ();
        int len = children.getLength ();
        for (int i = 0; i < len; i++)
          {
            buf.append (stringValue (children.item (i), true));
          }
        return buf.toString ();
      case Node.TEXT_NODE: // 5.7 Text Nodes
      case Node.CDATA_SECTION_NODE:
        return node.getNodeValue ();
      case Node.ATTRIBUTE_NODE: // 5.3 Attribute Nodes
      case Node.PROCESSING_INSTRUCTION_NODE: // 5.5 Processing Instruction
      case Node.COMMENT_NODE: // 5.6 Comment Nodes
        if (!elementMode)
          {
            return node.getNodeValue ();
          }
      default:
        return "";
      }
  }

}
