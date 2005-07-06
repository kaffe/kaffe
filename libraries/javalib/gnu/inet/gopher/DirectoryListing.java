/*
 * DirectoryListing.java
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
import java.net.ProtocolException;
import java.util.Iterator;
import java.util.NoSuchElementException;

import gnu.inet.util.LineInputStream;

/**
 * A gopher directory listing.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public final class DirectoryListing
  implements Iterator
{

  private static final String DOT = ".";

  private LineInputStream in;
  private boolean doneRead = false;
  private DirectoryEntry current;

  DirectoryListing(InputStream in)
  {
    this.in = new LineInputStream(in);
  }
  
  /**
   * Indicates whether this listing contains more entries.
   */
  public boolean hasNext()
  {
    try
      {
        fetch();
      }
    catch (IOException e)
      {
        return false;
      }
    return (current != null);
  }

  /**
   * @see #nextEntry
   */
  public Object next()
  {
    try
      {
        return nextEntry();
      }
    catch (IOException e)
      {
        throw new NoSuchElementException("I/O error: " + e.getMessage());
      }
  }
  
  /**
   * This iterator is read-only.
   */
  public void remove()
  {
    throw new UnsupportedOperationException();
  }
  
  /**
   * Returns the next entry in the directory listing.
   */
  public DirectoryEntry nextEntry()
    throws IOException
  {
    fetch();
    if (current == null)
      {
        throw new NoSuchElementException();
      }
    doneRead = false;
    return current;
  }
  
  void fetch()
    throws IOException
  {
    if (doneRead)
      {
        return;
      }
    String line = in.readLine();
    if (DOT.equals(line))
      {
        current = null;
      }
    else
      {
        // Parse line
        int type = DirectoryEntry.ERROR;
        switch(line.charAt(0))
          {
          case '0':
            type = DirectoryEntry.FILE;
            break;
          case '1':
            type = DirectoryEntry.DIRECTORY;
            break;
          case '2':
            type = DirectoryEntry.CSO_PHONE_BOOK;
            break;
          case '3':
            type = DirectoryEntry.ERROR;
            break;
          case '4':
            type = DirectoryEntry.BINHEX;
            break;
          case '5':
            type = DirectoryEntry.DOS_ARCHIVE;
            break;
          case '6':
            type = DirectoryEntry.UUENCODED;
            break;
          case '7':
            type = DirectoryEntry.INDEX_SEARCH;
            break;
          case '8':
            type = DirectoryEntry.TELNET;
            break;
          case '9':
            type = DirectoryEntry.BINARY;
            break;
          case '+':
            type = DirectoryEntry.REDUNDANT;
            break;
          case 'T':
            type = DirectoryEntry.TN3270;
            break;
          case 'g':
            type = DirectoryEntry.GIF;
            break;
          case 'I':
            type = DirectoryEntry.IMAGE;
            break;
          }
        int start = 1;
        int end = line.indexOf('\t', start);
        if (end == -1)
          {
            throw new ProtocolException("Invalid directory entry: " + line);
          }
        String title = line.substring(start, end);
        start = end + 1;
        end = line.indexOf('\t', start);
        if (end == -1)
          {
            throw new ProtocolException("Invalid directory entry: " + line);
          }
        String selector = line.substring(start, end);
        start = end + 1;
        end = line.indexOf('\t', start);
        if (end == -1)
          {
            throw new ProtocolException("Invalid directory entry: " + line);
          }
        String hostname = line.substring(start, end);
        start = end + 1;
        int port = Integer.parseInt(line.substring(start));
        current = new DirectoryEntry(type, title, selector, hostname, port);
      }
  }
  
}

