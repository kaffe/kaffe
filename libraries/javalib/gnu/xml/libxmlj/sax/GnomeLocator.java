/*
 * GnomeLocator.java
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

import org.xml.sax.Locator;

/**
 * SAX Locator implementation that uses libxml2.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class GnomeLocator
implements Locator
{

  // An xmlParserCtxtPtr
  private final Object ctx;

  // An xmlSAXLocatorPtr
  private final Object loc;

  GnomeLocator (Object ctx, Object loc)
  {
    this.ctx = ctx;
    this.loc = loc;
    if (ctx == null)
      {
        throw new NullPointerException ("ctx");
      }
    if (loc == null)
      {
        throw new NullPointerException ("loc");
      }
  }
  
  public String getPublicId ()
  {
    return publicId (ctx, loc);
  }

  private native String publicId (Object ctx, Object loc);

  public String getSystemId ()
  {
    return systemId (ctx, loc);
  }

  private native String systemId (Object ctx, Object loc);

  public int getLineNumber ()
  {
    return lineNumber (ctx, loc);
  }
  
  private native int lineNumber (Object ctx, Object loc);

  public int getColumnNumber ()
  {
    return columnNumber (ctx, loc);
  }
  
  private native int columnNumber (Object ctx, Object loc);

}
