/*
 * Cookie.java
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

package gnu.inet.http;

import java.text.ParseException;
import java.util.Date;

/**
 * An HTTP cookie, as specified in RFC 2109.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class Cookie
{

  /**
   * The name of the cookie.
   */
  protected final String name;

  /**
   * The value of the cookie.
   */
  protected final String value;

  /**
   * Optional documentation of the intended use of the cookie.
   */
  protected final String comment;

  /**
   * The domain for which the cookie is valid.
   */
  protected final String domain;

  /**
   * Optional subset of URL paths within the domain for which the cookie is
   * valid.
   */
  protected final String path;

  /**
   * Indicates that the user-agent should only use secure means to transmit
   * this cookie to the server.
   */
  protected final boolean secure;

  /**
   * The date at which this cookie expires.
   */
  protected final Date expires;

  public Cookie (String name, String value, String comment, String domain,
                 String path, boolean secure, Date expires)
  {
    this.name = name;
    this.value = value;
    this.comment = comment;
    this.domain = domain;
    this.path = path;
    this.secure = secure;
    this.expires = expires;
  }

  public String getName ()
  {
    return name;
  }

  public String getValue ()
  {
    return value;
  }

  public String getComment ()
  {
    return comment;
  }

  public String getDomain ()
  {
    return domain;
  }

  public String getPath ()
  {
    return path;
  }

  public boolean isSecure ()
  {
    return secure;
  }

  public Date getExpiryDate ()
  {
    return expires;
  }

  public String toString ()
  {
    return toString (true, true);
  }
  
  public String toString (boolean showPath, boolean showDomain)
  {
    StringBuffer buf = new StringBuffer ();
    buf.append (name);
    buf.append ('=');
    buf.append (value);
    if (showPath)
      {
        buf.append ("; $Path=");
        buf.append (path);
      }
    if (showDomain)
      {
        buf.append ("; $Domain=");
        buf.append (domain);
      }
    return buf.toString ();
  }

}
