/*
 * $Id: Logger.java,v 1.4 2004/10/04 19:34:03 robilad Exp $
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
 * Provides a simple logging mechanism for GNU inetlib and providers.
 * It is not possible to use the java.logging API as this doesn't exist
 * prior to Java 1.4.
 *
 * @author <a href="mailto:dog@gnu.org">Chris Burdess</a>
 */
public abstract class Logger
{

  /**
   * The singleton instance of this class.
   */
  private static Logger instance = new StderrLogger ();

  /**
   * Returns the singleton instance of this class.
   */
  public static Logger getInstance ()
    {
      return instance;
    }

  /**
   * Sets the singleton instance of this class.
   * @param logger the new logger to use
   */
  public static void setInstance (Logger logger)
    {
      if (logger == null)
        {
          throw new NullPointerException ();
        }
      instance = logger;
    }

  /**
   * Logs a configuration message.
   * This is used by the inetlib implementation itself.
   * @param message the message
   */
  public abstract void config (String message);

  /**
   * Logs a provider-specific debugging message.
   * @param protocol the provider protocol
   * @param message the message
   */
  public abstract void log (String protocol, String message);

  /**
   * Logs an error.
   * @param protocol the provider protocol
   * @param message the message
   */
  public abstract void error (String protocol, Throwable t);

}
