/*
 * Step.java
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
import org.w3c.dom.Node;

/**
 * A transition step between components in a location path.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public final class Step
  extends Path
{

  final Expr lhs;
  final Path rhs;

  public Step(Expr lhs, Path rhs)
  {
    this.lhs = lhs;
    this.rhs = rhs;
  }

  public Object evaluate(Node context, int pos, int len)
  {
    Object left = lhs.evaluate(context, pos, len);
    if (left instanceof Collection)
      {
        return rhs.evaluate(context, (Collection) left);
      }
    return Collections.EMPTY_SET;
  }

  Collection evaluate(Node context, Collection ns)
  {
    if (lhs instanceof Path)
      {
        ns = ((Path) lhs).evaluate(context, ns);
      }
    else
      {
        Set acc = new LinkedHashSet();
        int pos = 1, len = ns.size();
        for (Iterator i = ns.iterator(); i.hasNext(); )
          {
            Node node = (Node) i.next();
            Object ret = lhs.evaluate(node, pos++, len);
            if (ret instanceof Collection)
              {
                acc.addAll((Collection) ret);
              }
          }
        ns = acc;
      }
    ns = rhs.evaluate(context, ns);
    return ns;
  }

  public String toString()
  {
    return lhs + ((lhs instanceof Root) ? "" : "/") + rhs;
  }

}
