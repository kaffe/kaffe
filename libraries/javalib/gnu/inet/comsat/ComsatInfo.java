/*
 * $Id: ComsatInfo.java,v 1.1 2004/10/10 17:57:38 robilad Exp $
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

package gnu.inet.comsat;

import java.util.Iterator;
import java.util.Map;
import java.util.TreeMap;

/**
 * A comsat notification message.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class ComsatInfo
{

  /**
   * The mailbox in which the new message appeared.
   */
  protected String mailbox;

  /**
   * The message headers.
   */
  protected Map headers;

  /**
   * The first few lines of the message body.
   */
  protected String body;

  public String getMailbox ()
  {
    return mailbox;
  }

  protected void setMailbox (String mailbox)
  {
    this.mailbox = mailbox;
  }

  public String getHeader (String key)
  {
    return (String) headers.get (key);
  }

  public Iterator getHeaderNames ()
  {
    return headers.keySet ().iterator ();
  }

  protected void setHeader (String key, String value)
  {
    headers.put (key, value);
  }

  public String getBody ()
  {
    return body;
  }

  protected void setBody (String body)
  {
    this.body = body;
  }
  
}
