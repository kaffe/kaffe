/*
 * Copyright (C) 2004 The Free Software Foundation, Inc.
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
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License. 
 */
package gnu.xml.dom;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Iterator;
import java.util.List;

import org.w3c.dom.DOMImplementation;
import org.w3c.dom.DOMImplementationList;
import org.w3c.dom.DOMImplementationSource;

public class ImplementationSource
implements DOMImplementationSource
{

  private static final String DIGITS = "1234567890";

  /*
   * GNU DOM implementations.
   */
  private static final DOMImplementation[] implementations;

  static
  {
    List acc = new ArrayList ();
    acc.add (new gnu.xml.dom.DomImpl ());
    try
      {
        acc.add (new gnu.xml.libxmlj.dom.GnomeDocumentBuilder ());
      }
    catch (UnsatisfiedLinkError e)
      {
        // libxmlj not available
      }
    implementations = new DOMImplementation[acc.size ()];
    acc.toArray (implementations);
  }

  public DOMImplementation getDOMImplementation (String features)
  {
    List available = getImplementations (features);
    if (available.size () == 0)
      {
        return null;
      }
    return (DOMImplementation) available.get (0);
  }

  public DOMImplementationList getDOMImplementationList (String features)
  {
    List available = getImplementations (features);
    return new ImplementationList (available);
  }

  /**
   * Returns a list of the implementations that support the specified
   * features.
   */
  private List getImplementations (String features)
  {
    List available = new ArrayList (Arrays.asList (implementations));
    for (Iterator i = parseFeatures (features).iterator (); i.hasNext (); )
      {
        String feature = (String) i.next ();
        String version = null;
        int si = feature.indexOf (' ');
        if (si != -1)
          {
            version = feature.substring (si + 1);
            feature = feature.substring (0, si);
          }
        for (Iterator j = available.iterator (); j.hasNext (); )
          {
            DOMImplementation impl = (DOMImplementation) j.next();
            if (!impl.hasFeature (feature, version))
              {
                j.remove ();
              }
          }
      }
    return available;
  }

  /**
   * Parses the feature list into feature tokens.
   */
  List parseFeatures (String features)
  {
    List list = new ArrayList ();
    int pos = 0, start = 0;
    int len = features.length ();
    for (; pos < len; pos++)
      {
        char c = features.charAt (pos);
        if (c == ' ')
          {
            if (pos + 1 < len &&
                DIGITS.indexOf (features.charAt (pos + 1)) == -1)
              {
                list.add (getFeature (features, start, pos));
                start = pos + 1;
              }
          }
      }
    if (pos > start)
      {
        list.add (getFeature (features, start, len));
      }
    return list;
  }

  String getFeature (String features, int start, int end)
  {
    if (features.length () > 0 && features.charAt (start) == '+')
      {
        return features.substring (start + 1, end);
      }
    return features.substring (start, end);
  }

}
