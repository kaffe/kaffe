/*
 * NodeNumberNode.java
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

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import javax.xml.transform.TransformerException;
import org.w3c.dom.Document;
import org.w3c.dom.Node;
import gnu.xml.xpath.Expr;
import gnu.xml.xpath.NodeTypeTest;
import gnu.xml.xpath.Selector;
import gnu.xml.xpath.Test;
import gnu.xml.xpath.UnionExpr;

/**
 * A template node representing the XSL <code>number</code> instruction
 * with no <code>value</code> expression, i.e. the value is computed from
 * the document position of the context node.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
final class NodeNumberNode
  extends AbstractNumberNode
{

  static final int SINGLE = 0;
  static final int MULTIPLE = 1;
  static final int ANY = 2;

  final int level;
  final Expr count;
  final Expr from;

  NodeNumberNode(TemplateNode children, TemplateNode next,
                 int level, Expr count, Expr from,
                 String format, String lang,
                 int letterValue, String groupingSeparator, int groupingSize)
  {
    super(children, next, format, lang, letterValue, groupingSeparator,
          groupingSize);
    this.level = level;
    this.count = Template.patternToXPath(count);
    this.from = from;
  }

  int[] compute(Stylesheet stylesheet, Node context)
    throws TransformerException
  {
    if (from != null)
      {
        Object ret = from.evaluate(context, 1, 1);
        if (ret instanceof Collection)
          {
            Collection ns = (Collection) ret;
            if (ns.size() > 0)
              {
                List list = new ArrayList(ns);
                Collections.sort(list, documentOrderComparator);
                context = (Node) list.get(0);
              }
            else
              {
                return new int[0];
              }
          }
        else
          {
            return new int[0];
          }
      }
    switch (level)
      {
      case SINGLE:
        while (context != null && !countMatches(context))
          {
            context = context.getParentNode();
          }
        return (context == null) ? new int[0] :
          new int[] { getIndex(context) };
      case MULTIPLE:
        List ancestors = new ArrayList();
        while (context != null)
          {
            if (countMatches(context))
              {
                ancestors.add(context);
              }
            context = context.getParentNode();
          }
        int[] ret = new int[ancestors.size()];
        for (int i = 0; i < ret.length; i++)
          {
            ret[i] = getIndex((Node) ancestors.get(i));
          }
        return ret;
      case ANY:
        Expr preceding = new Selector(Selector.PRECEDING,
                                      Collections.EMPTY_LIST);
        Expr ancestorOrSelf = new Selector(Selector.ANCESTOR_OR_SELF,
                                           Collections.EMPTY_LIST);
        Expr any = new UnionExpr(preceding, ancestorOrSelf);
        Object eval = any.evaluate(context, 1, 1);
        if (eval instanceof Collection)
          {
            Collection ns = (Collection) eval;
            List candidates = new ArrayList();
            for (Iterator i = ns.iterator(); i.hasNext(); )
              {
                Node candidate = (Node) i.next();
                if (countMatches(candidate))
                  {
                    candidates.add(candidate);
                  }
              }
            int[] ret2 = new int[candidates.size()];
            for (int i = 0; i < ret2.length; i++)
              {
                ret2[i] = getIndex((Node) candidates.get(i));
              }
            return ret2;
          }
        return new int[0];
      default:
        throw new TransformerException("invalid level");
      }
  }

  boolean countMatches(Node node)
  {
    Object ret = count.evaluate(node, 1, 1);
    return (ret instanceof Collection) && ((Collection) ret).contains(node);
  }

  int getIndex(Node node)
  {
    int index = 0;
    do
      {
        node = node.getPreviousSibling();
        index++;
      }
    while (node != null);
    return index;
  }
  
}
