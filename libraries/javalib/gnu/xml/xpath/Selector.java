/*
 * Selector.java
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

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Set;
import org.w3c.dom.Attr;
import org.w3c.dom.NamedNodeMap;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;

/**
 * A single component of a location path.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public final class Selector
  extends Path
{

  public static final int ANCESTOR = 0;
  public static final int ANCESTOR_OR_SELF = 1;
  public static final int ATTRIBUTE = 2;
  public static final int CHILD = 3;
  public static final int DESCENDANT = 4;
  public static final int DESCENDANT_OR_SELF = 5;
  public static final int FOLLOWING = 6;
  public static final int FOLLOWING_SIBLING = 7;
  public static final int NAMESPACE = 8;
  public static final int PARENT = 9;
  public static final int PRECEDING = 10;
  public static final int PRECEDING_SIBLING = 11;
  public static final int SELF = 12;

  /**
   * Axis to select nodes in.
   */
  final int axis;

  /**
   * List of tests to perform on candidates.
   */
  final Test[] tests;

  public Selector(int axis, List tests)
  {
    this.axis = axis;
    this.tests = new Test[tests.size()];
    tests.toArray(this.tests);
  }

  /**
   * Returns the list of tests to perform on candidates.
   */
  public Test[] getTests()
  {
    return tests;
  }

  public Object evaluate(Node context, int pos, int len)
  {
    Set acc = new HashSet();
    addCandidates(context, acc);
    List candidates = new ArrayList(acc);
    Collections.sort(candidates, documentOrderComparator);
    return filterCandidates(candidates);
  }

  Collection evaluate(Node context, Collection ns)
  {
    Set acc = new HashSet();
    for (Iterator i = ns.iterator(); i.hasNext(); )
      {
        addCandidates((Node) i.next(), acc);
      }
    List candidates = new ArrayList(acc);
    Collections.sort(candidates, documentOrderComparator);
    return filterCandidates(candidates);
  }

  /**
   * Filter the given list of candates according to the node tests.
   */
  List filterCandidates(List candidates)
  {
    int len = candidates.size();
    int tlen = tests.length;
    if (tlen > 0 && len > 0)
      {
        // Present the result of each successful generation to the next test
        for (int j = 0; j < tlen && len > 0; j++)
          {
            Test test = tests[j];
            List successful = new ArrayList();
            for (int i = 0; i < len; i++)
              {
                Node node = (Node) candidates.get(i);
                if (test.matches(node, i + 1, len))
                  {
                    successful.add(node);
                  }
              }
            candidates = successful;
            len = candidates.size();
          }
      }
    return candidates;
  }

  void addCandidates(Node context, Collection candidates)
  {
    // Build list of candidates
    switch (axis)
      {
      case CHILD:
        addChildNodes(context, candidates, false);
        break;
      case DESCENDANT:
        addChildNodes(context, candidates, true);
        break;
      case DESCENDANT_OR_SELF:
        candidates.add (context);
        addChildNodes(context, candidates, true);
        break;
      case PARENT:
        addParentNode(context, candidates, false);
        break;
      case ANCESTOR:
        addParentNode(context, candidates, true);
        break;
      case ANCESTOR_OR_SELF:
        candidates.add(context);
        addParentNode(context, candidates, true);
        break;
      case FOLLOWING_SIBLING:
        addFollowingNodes(context, candidates, false);
        break;
      case PRECEDING_SIBLING:
        addPrecedingNodes(context, candidates, false);
        break;
      case FOLLOWING:
        addFollowingNodes(context, candidates, true);
        break;
      case PRECEDING:
        addPrecedingNodes(context, candidates, true);
        break;
      case ATTRIBUTE:
        addAttributes(context, candidates);
        break;
      case NAMESPACE:
        // TODO
        break;
      case SELF:
        candidates.add(context);
        break;
      }
  }

  void addChildNodes(Node context, Collection acc, boolean recurse)
  {
    Node child = context.getFirstChild();
    while (child != null)
      {
        acc.add(child);
        if (recurse)
          {
            addChildNodes(child, acc, recurse);
          }
        child = child.getNextSibling();
      }
  }

  void addParentNode(Node context, Collection acc, boolean recurse)
  {
    Node parent = (context.getNodeType() == Node.ATTRIBUTE_NODE) ?
      ((Attr) context).getOwnerElement() : context.getParentNode();
    if (parent != null)
      {
        acc.add(parent);
        if (recurse)
          {
            addParentNode(parent, acc, recurse);
          }
      }
  }

  void addFollowingNodes(Node context, Collection acc, boolean recurse)
  {
    Node cur = context.getNextSibling();
    while (cur != null)
      {
        acc.add(cur);
        cur = cur.getNextSibling();
      }
    if (recurse)
      {
        context = context.getParentNode();
        if (context != null)
          {
            addFollowingNodes(context, acc, recurse);
          }
      }
  }

  void addPrecedingNodes(Node context, Collection acc, boolean recurse)
  {
    Node cur = context.getPreviousSibling();
    while (cur != null)
      {
        acc.add(cur);
        cur = cur.getPreviousSibling();
      }
    if (recurse)
      {
        // FIXME This is probably not correct
        context = context.getParentNode();
        if (context != null)
          {
            addPrecedingNodes(context, acc, recurse);
          }
      }
  }

  void addAttributes(Node context, Collection acc)
  {
    NamedNodeMap attrs = context.getAttributes();
    if (attrs != null)
      {
        int attrLen = attrs.getLength();
        for (int i = 0; i < attrLen; i++)
          {
            acc.add(attrs.item(i));
          }
      }
  }

  public String toString()
  {
    StringBuffer buf = new StringBuffer();
    switch (axis)
      {
      case ANCESTOR:
        buf.append("ancestor");
        break;
      case ANCESTOR_OR_SELF:
        buf.append("ancestor-or-self");
        break;
      case ATTRIBUTE:
        buf.append("attribute");
        break;
      case CHILD:
        buf.append("child");
        break;
      case DESCENDANT:
        buf.append("descendant");
        break;
      case DESCENDANT_OR_SELF:
        buf.append("descendant-or-self");
        break;
      case FOLLOWING:
        buf.append("following");
        break;
      case FOLLOWING_SIBLING:
        buf.append("following-sibling");
        break;
      case NAMESPACE:
        buf.append("namespace");
        break;
      case PARENT:
        buf.append("parent");
        break;
      case PRECEDING:
        buf.append("preceding");
        break;
      case PRECEDING_SIBLING:
        buf.append("preceding-sibling");
        break;
      case SELF:
        buf.append("self");
        break;
      }
    buf.append("::");
    if (tests.length == 0)
      {
        buf.append('*');
      }
    else
      {
        for (int i = 0; i < tests.length; i++)
          {
            buf.append(tests[i]);
          }
      }
    return buf.toString();
  }
  
}
