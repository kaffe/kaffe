/*
 * StringArrayAttributes.java
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
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License.
 */
package gnu.xml.libxmlj.sax;

import org.xml.sax.Attributes;

/**
 * An implementation of Attributes that reads values from an array of
 * strings, supplied by libxml2.
 * Each pair of elements in the array represents a key followed by a value.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class StringArrayAttributes
implements Attributes
{

  private int len;
  private XMLName[] keys;
  private String[] values;

  StringArrayAttributes (GnomeXMLReader parser, String[] pairs)
  {
    len = (pairs == null) ? 0 : pairs.length / 2;
    keys = new XMLName[len];
    values = new String[len];
    for (int i = 0; i < len; i++)
    {
      int pairIndex = i * 2;
      keys[i] = new XMLName (parser, pairs[pairIndex]);
      values[i] = pairs[pairIndex + 1];
    }
  }

  public int getLength ()
  {
    return len;
  }

  public String getURI (int index)
  {
    if (index < 0 || index >= len)
      {
        return null;
      }
    return keys[index].uri;
  }

  public String getLocalName (int index)
  {
    if (index < 0 || index >= len)
      {
        return null;
      }
    return keys[index].localName;
  }

  public String getQName (int index)
  {
    if (index < 0 || index >= len)
      {
        return null;
      }
    return keys[index].qName;
  }

  public String getType (int index)
  {
    if (index < 0 || index >= len)
      {
        return null;
      }
    // TODO can we get this information from libxml2?
    return "CDATA";
  }

  public String getValue (int index)
  {
    if (index < 0 || index >= len)
      {
        return null;
      }
    return values[index];
  }

  public int getIndex (String uri, String localName)
  {
    for (int i = 0; i < len; i++)
    {
      XMLName key = keys[i];
      if (key.localName.equals (localName))
      {
        if ((key.uri == null && uri == null) ||
            (key.uri != null && key.uri.equals(uri)))
          {
            return i;
          }
      }
    }
    return -1;
  }

  public int getIndex (String qName)
  {
    for (int i = 0; i < len; i++)
    {
      if (keys[i].qName.equals (qName))
        {
          return i;
        }
    }
    return -1;
  }

  public String getType (String uri, String localName)
  {
    return getType (getIndex (uri, localName));
  }

  public String getType (String qName)
  {
    return getType (getIndex (qName));
  }

  public String getValue (String uri, String localName)
  {
    return getValue (getIndex (uri, localName));
  }

  public String getValue (String qName)
  {
    return getValue (getIndex (qName));
  }

}

