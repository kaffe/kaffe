/*
 * $Id: StatusResponse.java,v 1.3 2004/10/04 19:34:02 robilad Exp $
 * Copyright (C) 2002 The Free Software Foundation
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

package gnu.inet.nntp;

/**
 * An NNTP status response.
 * This represents the status code/message pair sent by the server in
 * response to client commands.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 * @version $Revision: 1.3 $ $Date: 2004/10/04 19:34:02 $
 */
public class StatusResponse
{

  /*
   * The status code.
   */
  protected short status;

  /*
   * The message.
   */
  protected String message;

  /**
   * Constructor.
   */
  protected StatusResponse (short status, String message)
  {
    this.status = status;
    this.message = message;
  }

  /**
   * Returns the status code associated with this response.
   */
  public short getStatus ()
  {
    return status;
  }
  
  /**
   * Returns the message associated with this response.
   */
  public String getMessage ()
  {
    return message;
  }
  
}
