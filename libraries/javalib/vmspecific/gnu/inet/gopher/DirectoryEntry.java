/*
 * DirectoryEntry.java
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
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this library with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this library, you may extend
 * this exception to your version of the library, but you are not
 * obliged to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 */

package gnu.inet.gopher;

import java.io.InputStream;
import java.io.IOException;
import java.util.Iterator;
import java.util.NoSuchElementException;

/**
 * A gopher directory entry.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public final class DirectoryEntry
{

  /**
   * Item is a file.
   */
  public static final int FILE = 0x30;

  /**
   * Item is a directory.
   */
  public static final int DIRECTORY = 0x31;

  /**
   * Item is a CSO phone-book server.
   */
  public static final int CSO_PHONE_BOOK = 0x32;

  /**
   * Error.
   */
  public static final int ERROR = 0x33;

  /**
   * Item is a BinHex Macintosh file.
   */
  public static final int BINHEX = 0x34;

  /**
   * Item is a DOS binary archive of some sort.
   */
  public static final int DOS_ARCHIVE = 0x35;

  /**
   * Item is a UNIX uuencoded file.
   */
  public static final int UUENCODED = 0x36;

  /**
   * Item is an Index-Search server.
   */
  public static final int INDEX_SEARCH = 0x37;

  /**
   * Item points to a text-based Telnet session.
   */
  public static final int TELNET = 0x38;

  /**
   * Item is a binary file.
   */
  public static final int BINARY = 0x39;

  /**
   * Item is a redundant server.
   */
  public static final int REDUNDANT = 0x2b;

  /**
   * Item points to a text-based tn3270 session.
   */
  public static final int TN3270 = 0x54;

  /**
   * Item is a GIF format graphics file.
   */
  public static final int GIF = 0x67;

  /**
   * Item is some kind of image file.
   */
  public static final int IMAGE = 0x49;

  final int type;
  final String title;
  final String selector;
  final String hostname;
  final int port;

  DirectoryEntry(int type, String title, String selector, String hostname,
                 int port)
  {
    this.type = type;
    this.title = title;
    this.selector = selector;
    this.hostname = hostname;
    this.port = port;
  }

  /**
   * Returns the type of this entry.
   */
  public int getType()
  {
    return type;
  }
  
  /**
   * Returns the title for this entry.
   */
  public String getTitle()
  {
    return title;
  }
  
  /**
   * Returns the selector for this entry.
   * This is used to retrieve the content for the entry.
   */
  public String getSelector()
  {
    return selector;
  }
  
  /**
   * Returns the hostname for the content of this entry.
   */
  public String getHostname()
  {
    return hostname;
  }
  
  /**
   * Returns the port on which the content for this entry can be retrieved.
   */
  public int getPort()
  {
    return port;
  }
  
}

