/*
 * GnomeEntity.java
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

import org.w3c.dom.Entity;

/**
 * A DOM entity node implemented in libxml2.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class GnomeEntity
extends GnomeNode
implements Entity
{

  GnomeEntity (Object id)
  {
    super (id);
  }

  public native String getPublicId ();

  public native String getSystemId ();

  public native String getNotationName ();

  // DOM Level 3 methods
  
  public String getInputEncoding ()
  {
    // TODO
    return null;    
  }
  
  public String getXmlEncoding ()
  {
    // TODO
    return null;
  }
  
  public String getXmlVersion ()
  {
    // TODO
    return null;
  }
  
  public String toString ()
  {
    String publicId = getPublicId ();
    StringBuffer buffer = new StringBuffer (getClass ().getName ());
    buffer.append ("[");
    if (publicId != null)
      {
        buffer.append ("publicId=");
        buffer.append (publicId);
        buffer.append (",");
      }
    buffer.append ("systemId=");
    buffer.append (getSystemId ());
    buffer.append (",notationName=");
    buffer.append (getNotationName ());
    buffer.append ("]");
    return buffer.toString ();
  }

}
