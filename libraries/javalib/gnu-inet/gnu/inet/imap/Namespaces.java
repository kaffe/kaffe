/*
 * Namespaces.java
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

package gnu.inet.imap;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.TreeMap;

/**
 * A tuple of IMAP namespaces, as defined in RFC 2342.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class Namespaces
{

  /**
   * An individual namespace specification.
   */
  public static class Namespace
  {

    String prefix;
    char delimiter;
    Map extensions;

    /**
     * Returns the namespace prefix.
     */
    public String getPrefix()
    {
      return prefix;
    }

    /**
     * Returns the delimiter character for the namespace.
     */
    public char getDelimiter()
    {
      return delimiter;
    }

    /**
     * Returns the dictionary of extension values for the namespace,
     * or <code>null</code> if there are no extensions.
     */
    public Map getExtensions()
    {
      return extensions;
    }

    /**
     * Debugging.
     */
    public String toString()
    {
      StringBuffer buf = new StringBuffer();
      buf.append('(');
      buf.append(quote(prefix));
      buf.append(' ');
      buf.append(quote(delimiter));
      if (extensions != null)
        {
          buf.append(' ');
          for (Iterator i = extensions.entrySet().iterator(); i.hasNext(); )
            {
              Map.Entry entry = (Map.Entry) i.next();
              String key = (String) entry.getKey();
              buf.append(quote(key));
              buf.append(' ');
              buf.append(format(entry.getValue()));
            }
        }
      buf.append(')');
      return buf.toString();
    }

    static String quote(String text)
    {
      return '"' + text + '"';
    }

    static String quote(char c)
    {
      char[] chars = new char[] { '"', c, '"' };
      return new String(chars);
    }

    private String format(Object value)
    {
      if (value == null)
        {
          return IMAPConstants.NIL;
        }
      else if (value instanceof String)
        {
          return quote((String) value);
        }
      else
        {
          List list = (List) value;
          int len = list.size();
          StringBuffer buf = new StringBuffer();
          buf.append('(');
          for (int i = 0; i < len; i++)
            {
              if (i > 0)
                {
                  buf.append(' ');
                }
              buf.append(format(list.get(i)));
            }
          buf.append(')');
          return buf.toString();
        }
    }
    
  }

  List personal;
  List other;
  List shared;

  Namespaces(String text)
  {
    List acc = new ArrayList();
    int len = text.length();
    parse(text, 0, len, acc);

    len = acc.size();
    if (len > 0)
      {
        personal = parseNamespaceList(acc.get(0));
        if (len > 1)
          {
            other = parseNamespaceList(acc.get(1));
            if (len > 2)
              {
                shared = parseNamespaceList(acc.get(2));
              }
          }
      }
  }

  /**
   * Returns the list of personal namespaces.
   */
  public Namespace[] getPersonal()
  {
    return toArray(personal);
  }

  /**
   * Returns the list of other users' namespaces.
   */
  public Namespace[] getOther()
  {
    return toArray(other);
  }

  /**
   * Returns the list of shared namespaces.
   */
  public Namespace[] getShared()
  {
    return toArray(shared);
  }

  private Namespace[] toArray(List namespaceList)
  {
    if (namespaceList == null)
      {
        return null;
      }
    Namespace[] ret = new Namespace[namespaceList.size()];
    namespaceList.toArray(ret);
    return ret;
  }

  /**
   * Parse the specified text into an S-expression.
   */
  static int parse(String text, int start, int len, List acc)
  {
    StringBuffer buf = new StringBuffer();
    boolean inLiteral = false;
    for (int i = start; i < len; i++)
      {
        char c = text.charAt(i);
        if (inLiteral)
          {
            if (c == '"')
              {
                String literal = buf.toString();
                buf.setLength(0);
                inLiteral = false;
                acc.add(literal);
              }
            else
              {
                buf.append(c);
              }
          }
        else
          {
            switch (c)
              {
              case ' ':
                String token = buf.toString();
                if (IMAPConstants.NIL.equals(token))
                  {
                    acc.add(null);
                  }
                buf.setLength(0);
                break;
              case '"':
                inLiteral = true;
                buf.setLength(0);
                break;
              case '(':
                List sub = new ArrayList();
                i = parse(text, i + 1, len, sub);
                acc.add(sub);
                break;
              case ')':
                return i;
              }
          }
      }
    return len;
  }

  private List parseNamespaceList(Object ns)
  {
    if (ns == null)
      {
        return null;
      }
    List list = (List) ns;
    int len = list.size();
    List ret = new ArrayList(len);
    for (int i = 0; i < len; i++)
      {
        ret.add(parseNamespace((List) list.get(i)));
      }
    return ret;
  }

  private Namespace parseNamespace(List comps)
  {
    int len = comps.size();
    Namespace ns = new Namespace();
    ns.prefix = (String) comps.get(0);
    ns.delimiter = ((String) comps.get(1)).charAt(0);
    if (len > 2)
      {
        ns.extensions = new TreeMap();
        for (int i = 2; i < len; i += 2)
          {
            String key = (String) comps.get(i);
            Object val = (i == len - 1) ? null : comps.get(i + 1);
            ns.extensions.put(key, val);
          }
      }
    return ns;
  }

  /**
   * Debugging.
   */
  public String toString()
  {
    StringBuffer buf = new StringBuffer();
    appendNamespaceList(buf, personal);
    buf.append(' ');
    appendNamespaceList(buf, other);
    buf.append(' ');
    appendNamespaceList(buf, shared);
    return buf.toString();
  }

  private void appendNamespaceList(StringBuffer buf, List list)
  {
    if (list == null)
      {
        buf.append(IMAPConstants.NIL);
      }
    else
      {
        int len = list.size();
        buf.append('(');
        for (int i = 0; i < len; i++)
          {
            if (i > 0)
              {
                buf.append(' ');
              }
            buf.append(list.get(i));
          }
        buf.append(')');
      }
  }

}

