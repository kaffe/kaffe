/*
 * $Id: ByteArrayRequestBodyWriter.java,v 1.3 2004/10/04 19:33:58 robilad Exp $
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
 * A simple request body writer using a byte array.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class ByteArrayRequestBodyWriter
implements RequestBodyWriter
{

  /**
   * The content.
   */
  protected byte[] content;

  /**
   * The position within the content at which the next read will occur.
   */
  protected int pos;

  /**
   * Constructs a new byte array request body writer with the specified
   * content.
   * @param content the content buffer
   */
  public ByteArrayRequestBodyWriter (byte[] content)
  {
    this.content = content;
    pos = 0;
  }

  /**
   * Returns the total number of bytes that will be written in a single pass
   * by this writer.
   */
  public int getContentLength ()
  {
    return content.length;
  }

  /**
   * Initialises the writer.
   * This will be called before each pass.
   */
  public void reset ()
  {
    pos = 0;
  }

  /**
   * Writes body content to the supplied buffer.
   * @param buffer the content buffer
   * @return the number of bytes written
   */
  public int write (byte[] buffer)
  {
    int len = content.length - pos;
    len = (buffer.length < len) ? buffer.length : len;
    if (len > -1)
      {
        System.arraycopy (content, pos, buffer, 0, len);
        pos += len;
      }
    return len;
  }
  
}
