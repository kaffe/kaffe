/*
 * $Id: ChunkedInputStream.java,v 1.3 2004/10/04 19:33:58 robilad Exp $
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

import java.io.FilterInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.net.ProtocolException;

/**
 * Input stream wrapper for the "chunked" transfer-coding.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class ChunkedInputStream
extends FilterInputStream
{

  private static final byte CR = 0x0d;
  private static final byte LF = 0x0a;

  int size;
  int count;
  boolean meta;
  boolean eof;
  Headers headers;

  /**
   * Constructor.
   * @param in the response socket input stream
   * @param headers the headers to receive additional header lines
   */
  public ChunkedInputStream (InputStream in, Headers headers)
  {
    super (in);
    this.headers = headers;
    size = -1;
    count = 0;
    meta = true;
  }

  public int read ()
    throws IOException
  {
    byte[] buf = new byte[1];
    int len = read (buf, 0, 1);
    if (len == -1)
      {
        return -1;
      }
    int ret = (int) buf[0];
    if (ret < 0)
      {
        ret += 0x100;
      }
    return ret;
  }

  public int read (byte[] buffer)
    throws IOException
  {
    return read (buffer, 0, buffer.length);
  }

  public int read (byte[] buffer, int offset, int length)
    throws IOException
  {
    if (eof)
      {
        return -1;
      }
    if (meta)
      {
        // Read chunk header
        int c, last = 0;
        boolean seenSemi = false;
        StringBuffer buf = new StringBuffer ();
        do
          {
            c = in.read ();
            if (c == 0x3b) // ;
              {
                seenSemi = true;
              }
            else if (c == 0x0a && last == 0x0d) // CRLF
              {
                size = Integer.parseInt (buf.toString (), 16);
                break;
              }
            else if (!seenSemi && c >= 0x30)
              {
                buf.append ((char) c);
              }
            last = c;
          }
        while (c != -1);
        count = 0;
        meta = false;
      }
    if (size == 0)
      {
        // Read trailer
        headers.parse (in);
        eof = true;
        return -1;
      }
    else
      {
        int diff = length - offset;
        int max = size - count;
        max = (diff < max) ? diff : max;
        int len = (max > 0) ? in.read (buffer, offset, max) : 0;
        count += len;
        if (count == size)
          {
            // Read CRLF
            int c1 = in.read ();
            int c2 = in.read ();
            if (c1 != 0x0d || c2 != 0x0a)
              {
                throw new ProtocolException ("expecting CRLF: " + c1 + "," + c2);
              }
            meta = true;
          }
        return len;
      }
  }
  
}
