/*
 * $Id: ByteArrayResponseBodyReader.java,v 1.3 2004/10/04 19:33:58 robilad Exp $
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

/**
 * Simple response body reader that stores content in a byte array.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class ByteArrayResponseBodyReader
implements ResponseBodyReader
{

  /**
   * The content.
   */
  protected byte[] content;

  /**
   * The position in the content at which the next write will occur.
   */
  protected int pos;

  /**
   * The length of the buffer.
   */
  protected int len;

  /**
   * Constructs a new byte array response body reader.
   */
  public ByteArrayResponseBodyReader ()
  {
    this (4096);
  }
  
  /**
   * Constructs a new byte array response body reader with the specified
   * initial buffer size.
   * @param size the initial buffer size
   */
  public ByteArrayResponseBodyReader (int size)
  {
    content = new byte[size];
    pos = len = 0;
  }

  /**
   * This reader accepts all responses.
   */ 
  public boolean accept (Request request, Response response)
  {
    return true;
  }

  public void read (byte[] buffer, int offset, int length)
  {
    int l = length - offset;
    if (pos + l > content.length)
      {
        byte[] tmp = new byte[content.length * 2];
        System.arraycopy (content, 0, tmp, 0, pos);
        content = tmp;
      }
    System.arraycopy (buffer, offset, content, pos, l);
    pos += l;
    len = pos;
  }

  public void close ()
  {
    pos = 0;
  }

  /**
   * Retrieves the content of this reader as a byte array.
   * The size of the returned array is the number of bytes read.
   */
  public byte[] toByteArray ()
  {
    byte[] ret = new byte[len];
    System.arraycopy (content, 0, ret, 0, len);
    return ret;
  }
  
}
