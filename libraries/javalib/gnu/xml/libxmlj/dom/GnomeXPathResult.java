/*
 * GnomeXPathResult.java
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
package gnu.xml.libxmlj.dom;

import org.w3c.dom.DOMException;
import org.w3c.dom.Node;
import org.w3c.dom.xpath.XPathException;
import org.w3c.dom.xpath.XPathResult;

/**
 * An XPath result object implemented in libxml2.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class GnomeXPathResult
implements XPathResult
{

  /**
   * xmlXPathObjectPtr
   */
  final Object obj;

  GnomeXPathResult (Object obj)
  {
    this.obj = obj;
  }

  protected void finalize ()
  {
    free (obj);
  }

  private native void free (Object obj);
  
  public native short getResultType ();

  public native double getNumberValue ()
    throws XPathException;

  public native String getStringValue ()
    throws XPathException;

  public native boolean getBooleanValue ()
    throws XPathException;

  public native Node getSingleNodeValue ()
    throws XPathException;

  public native boolean getInvalidIteratorState();

  public native int getSnapshotLength ()
    throws XPathException;

  public native Node iterateNext ()
    throws XPathException, DOMException;

  public native Node snapshotItem (int index)
    throws XPathException;

  public String toString ()
  {
    short type = getResultType ();
    switch (type)
      {
      case STRING_TYPE:
        return getStringValue ();
      case NUMBER_TYPE:
        return new Double (getNumberValue ()).toString ();
      case BOOLEAN_TYPE:
        return Boolean.valueOf (getBooleanValue ()).toString ();
      case UNORDERED_NODE_SNAPSHOT_TYPE:
        int len = getSnapshotLength ();
        switch (len) {
        case 0:
          return "[no matches]";
        case 1:
          return getSingleNodeValue ().toString ();
        default:
          StringBuffer buffer = new StringBuffer ();
          for (int i = 0; i < len; i++)
            {
              if (i > 0)
                {
                  buffer.append (',');
                }
              buffer.append (snapshotItem (i));
            }
          return buffer.toString ();
        }
      default:
        return getClass ().getName () + "[type=" + type + ",length=" +
          getSnapshotLength () + ']';
      }
  }
  
}
