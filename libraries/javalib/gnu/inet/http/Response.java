/*
 * $Id: Response.java,v 1.3 2004/10/04 19:33:58 robilad Exp $
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

import java.util.Date;

/**
 * An HTTP response.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class Response
{

  /**
   * The HTTP major version of the server issuing the response.
   */
  protected final int majorVersion;

  /**
   * The HTTP minor version of the server issuing the response.
   */
  protected final int minorVersion;

  /**
   * The HTTP status code of the response.
   */ 
  protected final int code;

  /**
   * The class of the response. This is the most significant digit of the
   * status code.
   * <dl>
   * <dt><code>1xx</code></dt> <dd>Informational response</dd>
   * <dt><code>2xx</code></dt> <dd>Success</dd>
   * <dt><code>3xx</code></dt> <dd>Redirection</dd>
   * <dt><code>4xx</code></dt> <dd>Client error</dd>
   * <dt><code>5xx</code></dt> <dd>Server error</dd>
   * </dl>
   */
  protected final int codeClass;

  /**
   * Human-readable text of the response.
   */
  protected final String message;

  /**
   * The response headers.
   */
  protected final Headers headers;

  /**
   * Constructs a new response with the specified parameters.
   */
  protected Response (int majorVersion, int minorVersion, int code,
                      int codeClass, String message,
                      Headers headers)
  {
    this.majorVersion = majorVersion;
    this.minorVersion = minorVersion;
    this.code = code;
    this.codeClass = codeClass;
    this.message = message;
    this.headers = headers;
  }

  /**
   * Returns the HTTP major version of the server issuing the response.
   * @see #majorVersion
   */
  public int getMajorVersion ()
  {
    return majorVersion;
  }

  /**
   * Returns the HTTP minor version of the server issuing the response.
   * @see #minorVersion
   */
  public int getMinorVersion ()
  {
    return minorVersion;
  }

  /**
   * Returns the HTTP status code of the response.
   * @see #code
   */ 
  public int getCode ()
  {
    return code;
  }

  /**
   * Returns the class of the response.
   * @see #codeClass
   */
  public int getCodeClass ()
  {
    return codeClass;
  }

  /**
   * Returns the human-readable text of the response.
   * @see #message
   */
  public String getMessage ()
  {
    return message;
  }

  /**
   * Returns the headers in the response.
   */
  public Headers getHeaders ()
  {
    return headers;
  }

  /**
   * Returns the header value for the specified name.
   * @param name the header name
   */
  public String getHeader (String name)
  {
    return headers.getValue (name);
  }

  /**
   * Returns the header value for the specified name as an integer.
   * @param name the header name
   */
  public int getIntHeader (String name)
  {
    return headers.getIntValue (name);
  }

  /**
   * Returns the header value for the specified name as a date.
   * @param name the header name
   */
  public Date getDateHeader (String name)
  {
    return headers.getDateValue (name);
  }

}
