/*
 * $Id: StderrLogger.java,v 1.2 2004/08/09 14:38:10 dalibor Exp $
 * Copyright (C) 2003 The Free Software Foundation
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

package gnu.inet.util;

/**
 * A logger that simply writes to <code>System.err</code>.
 *
 * @author <a href="mailto:dog@gnu.org">Chris Burdess</a>
 * @version $Revision: 1.2 $ $Date: 2004/08/09 14:38:10 $
 */
public class StderrLogger extends Logger
{

  /**
   * Logs a configuration message.
   * This is used by the inetlib implementation itself.
   * @param message the message
   */
  public void config (String message)
    {
      System.err.print ("DEBUG: ");
      System.err.println (message);
    }

  /**
   * Logs a provider-specific debugging message.
   * @param protocol the provider protocol
   * @param message the message
   */
  public void log (String protocol, String message)
    {
      System.err.print (protocol);
      System.err.print (": ");
      System.err.println (message);
    }

}
