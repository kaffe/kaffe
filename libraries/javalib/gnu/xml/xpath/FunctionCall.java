/*
 * FunctionCall.java
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
import java.util.List;
import javax.xml.namespace.QName;
import javax.xml.xpath.XPathFunction;
import javax.xml.xpath.XPathFunctionException;
import javax.xml.xpath.XPathFunctionResolver;
import org.w3c.dom.Node;

/**
 * Executes an XPath core or extension function.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class FunctionCall
extends Expr
{

  final XPathFunctionResolver resolver;
  final String name;
  final List args;

  FunctionCall (XPathFunctionResolver resolver, String name)
  {
    this (resolver, name, Collections.EMPTY_LIST);
  }

  FunctionCall (XPathFunctionResolver resolver, String name, List args)
  {
    this.resolver = resolver;
    this.name = name;
    this.args = args;
  }

  public Object evaluate (Node context)
  {
    if ("last".equals (name))
      {
        return new Double (_last (context));
      }
    else if ("position".equals (name))
      {
        return new Double (_position (context));
      }
    else if ("count".equals (name))
      {
        if (args.size () == 1)
          {
            Expr arg = (Expr) args.get (0);
            Object val = arg.evaluate (context);
            if (val instanceof Collection)
              {
                Collection ns = (Collection) val;
                return new Double (_count (context, ns));
              }
          }
      }
    else if ("id".equals (name))
      {
        if (args.size () == 1)
          {
            Expr arg = (Expr) args.get (0);
            Object val = arg.evaluate (context);
            return _id (context, val);
          }
      }
    else if ("local-name".equals (name))
      {
        
        switch (args.size ())
          {
          case 0:
            return _local_name (context, null);
          case 1:
            Expr arg = (Expr) args.get (0);
            Object val = arg.evaluate (context);
            if (val instanceof Collection)
              {
                Collection ns = (Collection) val;
                return _local_name (context, ns);
              }
          }
      }
    else if ("namespace-uri".equals (name))
      {
        switch (args.size ())
          {
          case 0:
            return _namespace_uri (context, null);
          case 1:  
            Expr arg = (Expr) args.get (0);
            Object val = arg.evaluate (context);
            if (val instanceof Collection)
              {
                Collection ns = (Collection) val;
                return _namespace_uri (context, ns);
              }
          }
      }
    else if ("name".equals (name))
      {
        switch (args.size ())
          {
          case 0:
            return _name (context, null);
          case 1:
            Expr arg = (Expr) args.get (0);
            Object val = arg.evaluate (context);
            if (val instanceof Collection)
              {
                Collection ns = (Collection) val;
                return _name (context, ns);
              }
          }
      }
    else if ("string".equals (name))
      {
        switch (args.size ())
          {
          case 0:
            return _string (context, null);
          case 1:
            Expr arg = (Expr) args.get (0);
            Object val = arg.evaluate (context);
            return _string (context, val);
          }
      }
    else if ("concat".equals (name))
      {
        int len = args.size ();
        if (len >= 2)
          {
            StringBuffer buf = new StringBuffer ();
            for (int i = 0; i < len; i++)
              {
                Expr arg = (Expr) args.get (i);
                Object val = arg.evaluate (context);
                if (val instanceof String)
                  {
                    buf.append ((String) val);
                  }
              }
            return buf.toString ();
          }
      }
    else if ("starts-with".equals (name))
      {
        if (args.size () == 2)
          {
            Expr arg1 = (Expr) args.get (0);
            Expr arg2 = (Expr) args.get (1);
            Object val1 = arg1.evaluate (context);
            Object val2 = arg2.evaluate (context);
            if (val1 instanceof String && val2 instanceof String)
              {
                return _starts_with (context, (String) val1, (String) val2) ?
                  Boolean.TRUE : Boolean.FALSE;
              }
          }
      }
    else if ("contains".equals (name))
      {
        if (args.size () == 2)
          {
            Expr arg1 = (Expr) args.get (0);
            Expr arg2 = (Expr) args.get (1);
            Object val1 = arg1.evaluate (context);
            Object val2 = arg2.evaluate (context);
            if (val1 instanceof String && val2 instanceof String)
              {
                return _contains (context, (String) val1, (String) val2) ?
                  Boolean.TRUE : Boolean.FALSE;
              }
          }
      }
    else if ("substring-before".equals (name))
      {
        if (args.size () == 2)
          {
            Expr arg1 = (Expr) args.get (0);
            Expr arg2 = (Expr) args.get (1);
            Object val1 = arg1.evaluate (context);
            Object val2 = arg2.evaluate (context);
            if (val1 instanceof String && val2 instanceof String)
              {
                return _substring_before (context, (String) val1, (String) val2);
              }
          }
      }
    else if ("substring-after".equals (name))
      {
        if (args.size () == 2)
          {
            Expr arg1 = (Expr) args.get (0);
            Expr arg2 = (Expr) args.get (1);
            Object val1 = arg1.evaluate (context);
            Object val2 = arg2.evaluate (context);
            if (val1 instanceof String && val2 instanceof String)
              {
                return _substring_after (context, (String) val1, (String) val2);
              }
          }
      }
    else if ("substring".equals (name))
      {
        int len = args.size ();
        if (len == 2 || len == 3)
          {
            Expr arg1 = (Expr) args.get (0);
            Expr arg2 = (Expr) args.get (1);
            Object val1 = arg1.evaluate (context);
            Object val2 = arg2.evaluate (context);
            if (val1 instanceof String && val2 instanceof Double)
              {
                String s = (String) val1;
                double p = ((Double) val2).doubleValue ();
                double l = (double) (s.length () + 1);
                if (len == 3)
                  {
                    Expr arg3 = (Expr) args.get (2);
                    Object val3 = arg3.evaluate (context);
                    if (val3 instanceof Double)
                      {
                        l = ((Double) val3).doubleValue ();
                      }
                  }
                return _substring (context, s, p, l);
              }
          }
      }
    else if ("string-length".equals (name))
      {
        switch (args.size ())
          {
          case 0:
            return new Double (_string_length (context, null));
          case 1:
            Expr arg = (Expr) args.get (0);
            Object val = arg.evaluate (context);
            if (val instanceof String)
              {
                return new Double (_string_length (context, (String) val));
              }
          }
      }
    else if ("normalize-space".equals (name))
      {
        switch (args.size ())
          {
          case 0:
            return _normalize_space (context, null);
          case 1:
            Expr arg = (Expr) args.get (0);
            Object val = arg.evaluate (context);
            if (val instanceof String)
              {
                return _normalize_space (context, (String) val);
              }
          }
      }
    else if ("translate".equals (name))
      {
        if (args.size () == 3)
          {
            Expr arg1 = (Expr) args.get (0);
            Expr arg2 = (Expr) args.get (1);
            Expr arg3 = (Expr) args.get (2);
            Object val1 = arg1.evaluate (context);
            Object val2 = arg2.evaluate (context);
            Object val3 = arg3.evaluate (context);
            if (val1 instanceof String && val2 instanceof String &&
                val3 instanceof String)
              {
                return _translate (context, (String) val1, (String) val2,
                                   (String) val3);
              }
          }
      }
    else if ("boolean".equals (name))
      {
        if (args.size () == 1)
          {
            Expr arg = (Expr) args.get (0);
            Object val = arg.evaluate (context);
            return _boolean (context, val) ? Boolean.TRUE : Boolean.FALSE;
          }
      }
    else if ("not".equals (name))
      {
        if (args.size () == 1)
          {
            Expr arg = (Expr) args.get (0);
            Object val = arg.evaluate (context);
            if (val instanceof Boolean)
              {
                return ((Boolean) val).booleanValue () ?
                  Boolean.FALSE : Boolean.TRUE;
              }
          }
      }
    else if ("true".equals (name))
      {
        if (args.size () == 0)
          {
            return Boolean.TRUE;
          }
      }
    else if ("false".equals (name))
      {
        if (args.size () == 0)
          {
            return Boolean.FALSE;
          }
      }
    else if ("lang".equals (name))
      {
        if (args.size () == 1)
          {
            Expr arg = (Expr) args.get (0);
            Object val = arg.evaluate (context);
            if (val instanceof String)
              {
                return _lang (context, (String) val) ? Boolean.TRUE :
                  Boolean.FALSE;
              }
          }
      }
    else if ("number".equals (name))
      {
        switch (args.size ())
          {
          case 0:
            return new Double (_number (context, null));
          case 1:
            Expr arg = (Expr) args.get (0);
            Object val = arg.evaluate (context);
            return new Double (_number (context, val));
          }
      }
    else if ("sum".equals (name))
      {
        if (args.size () == 1)
          {
            Expr arg = (Expr) args.get (0);
            Object val = arg.evaluate (context);
            if (val instanceof Collection)
              {
                Collection ns = (Collection) val;
                return new Double (_sum (context, ns));
              }
          }
      }
    else if ("floor".equals (name))
      {
        if (args.size () == 1)
          {
            Expr arg = (Expr) args.get (0);
            Object val = arg.evaluate (context);
            if (val instanceof Double)
              {
                double n = ((Double) val).doubleValue ();
                return new Double (_floor (context, n));
              }
          }
      }
    else if ("ceiling".equals (name))
      {
        if (args.size () == 1)
          {
            Expr arg = (Expr) args.get (0);
            Object val = arg.evaluate (context);
            if (val instanceof Double)
              {
                double n = ((Double) val).doubleValue ();
                return new Double (_ceiling (context, n));
              }
          }
      }
    else if ("round".equals (name))
      {
        if (args.size () == 1)
          {
            Expr arg = (Expr) args.get (0);
            Object val = arg.evaluate (context);
            if (val instanceof Double)
              {
                double n = ((Double) val).doubleValue ();
                return new Double (_round (context, n));
              }
          }
      }
    if (resolver != null)
      {
        QName qname = QName.valueOf(name);
        XPathFunction function = resolver.resolveFunction(qname, args.size());
        if (function != null)
          {
            try
              {
                return function.evaluate(args);
              }
            catch (XPathFunctionException e)
              {
                throw new RuntimeException("Error executing XPath function",
                                           e);
              }
          }
      }
    throw new IllegalArgumentException ("Invalid function call: " +
                                        toString ());
  }

  public String toString ()
  {
    StringBuffer buf = new StringBuffer ();
    buf.append (name);
    buf.append ('(');
    int len = args.size ();
    for (int i = 0; i < len; i++)
      {
        if (i > 0)
          {
            buf.append (',');
          }
        buf.append (args.get (i));
      }
    buf.append (')');
    return buf.toString ();
  }
  
}
