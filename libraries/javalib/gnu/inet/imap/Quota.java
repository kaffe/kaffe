/*
 * $Id: Quota.java,v 1.1 2004/10/04 19:34:00 robilad Exp $
 * Copyright (C) 2004 The Free Software Foundation
 * 
 * This file is part of GNU inetlib, a library.
 * 
 * GNU inetlib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * GNU inetlib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License.
 */

package gnu.inet.imap;

import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

/**
 * An IMAP quota entry.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class Quota
{

  /**
   * A quota resource entry.
   */
  public static class Resource
  {

    String name;
    int current;
    int limit;

    /**
     * Specifies a new resource with the given name and limit.
     * @param name the resource name
     * @param limit the resource limit
     */
    public Resource (String name, int limit)
    {
      this (name, -1, limit);
    }

    Resource (String name, int current, int limit)
    {
      this.name = name;
      this.current = current;
      this.limit = limit;
    }

    /**
     * Returns the resource name.
     */
    public String getName ()
    {
      return name;
    }

    /**
     * Returns the current usage of the resource, or <code>-1</code> if not
     * known.
     */
    public int getCurrentUsage ()
    {
      return current;
    }

    /**
     * Returns the resource limit.
     */
    public int getLimit ()
    {
      return limit;
    }

    /**
     * Debugging.
     */
    public String toString ()
    {
      StringBuffer buf = new StringBuffer ();
      buf.append ('(');
      buf.append (name);
      if (current >= 0)
        {
          buf.append (' ');
          buf.append (current);
        }
      buf.append (' ');
      buf.append (limit);
      buf.append (')');
      return buf.toString ();
    }
    
  }

  String quotaRoot;
  List resources;

  Quota (String text)
  {
    int len = text.length ();
    List acc = new ArrayList ();
    Namespaces.parse (text, 0, len, acc);

    quotaRoot = (String) acc.get (0);
    resources = new ArrayList ();
    len = acc.size ();
    for (int i = 1; i < len; i++)
      {
        resources.add (parseResource ((List) acc.get (i)));
      }
  }

  private Resource parseResource (List triple)
  {
    String name = (String) triple.get (0);
    String current = (String) triple.get (1);
    String limit = (String) triple.get (2);
    return new Resource (name,
                         Integer.parseInt (current),
                         Integer.parseInt (limit));
  }

  /**
   * Returns the quota root. All mailboxes that share the same named
   * quota root share the resource limits of the quota root.
   */
  public String getQuotaRoot ()
  {
    return quotaRoot;
  }

  /**
   * Returns the list of quota resources.
   */
  public Resource[] getResources ()
  {
    Resource[] ret = new Resource[resources.size ()];
    resources.toArray (ret);
    return ret;
  }

  /**
   * Debugging.
   */
  public String toString ()
  {
    StringBuffer buf = new StringBuffer ();
    buf.append (IMAPConnection.quote (UTF7imap.encode (quotaRoot)));
    int len = resources.size ();
    for (int i = 0; i < len; i++)
      {
        buf.append (' ');
        buf.append (resources.get (i));
      }
    return buf.toString ();
  }
  
}
