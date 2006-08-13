/*
 * LineIterator.java
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

package gnu.inet.nntp;

import java.io.IOException;
import java.util.Iterator;
import java.util.NoSuchElementException;

/**
 * An iterator over an NNTP multi-line response.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class LineIterator
  implements Iterator, PendingData
{

  static final String DOT = ".";

  boolean doneRead = false;
  NNTPConnection connection;
  String current;

  LineIterator(NNTPConnection connection)
  {
    this.connection = connection;
  }

  void doRead()
    throws IOException
  {
    if (doneRead)
      {
        return;
      }
    String line = connection.read();
    if (DOT.equals(line))
      {
        current = null;
      }
    else
      {
        current = line;
      }
    doneRead = true;
  }

  /**
   * Indicates whether there are more lines to be read.
   */
  public boolean hasNext()
  {
    try
      {
        doRead();
      }
    catch (IOException e)
      {
        return false;
      }
    return(current != null);
  }

  /**
   * Returns the next line.
   */
  public Object next()
  {
    try
      {
        return nextLine();
      }
    catch (IOException e)
      {
        throw new NoSuchElementException("I/O error: " + e.getMessage());
      }
  }

  /**
   * Returns the next line.
   */
  public String nextLine()
    throws IOException
  {
    doRead();
    if (current == null)
      {
        throw new NoSuchElementException();
      }
    doneRead = false;
    return current;
  }

  /**
   * This iterator is read-only.
   */
  public void remove()
  {
    throw new UnsupportedOperationException();
  }

  /**
   * Read to the end of this iterator.
   */
  public void readToEOF()
    throws IOException
  {
    do
      {
        doRead();
      }
    while (current != null);
  }

}

