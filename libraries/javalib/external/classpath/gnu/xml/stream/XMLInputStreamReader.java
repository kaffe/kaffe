/* XMLInputStreamReader.java -- 
   Copyright (C) 2005  Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */

package gnu.xml.stream;

import java.io.FilterReader;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.Reader;
import java.io.UnsupportedEncodingException;

/**
 * A mark-capable reader that can copy its buffer state into another
 * instance with a different encoding.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class XMLInputStreamReader
  extends FilterReader
{

  final InputStream is;
  char[] buf;
  int pos, count, markpos, marklimit, bufferSize;

  XMLInputStreamReader(InputStream is, String encoding)
    throws UnsupportedEncodingException
  {
    super(new InputStreamReader(is, encoding));
    this.is = is;
    final int size = 2048;
    buf = new char[size];
    pos = count = bufferSize = size;
  }

  XMLInputStreamReader(XMLInputStreamReader reader, String encoding)
    throws UnsupportedEncodingException
  {
    this(reader.is, encoding);
    buf = reader.buf;
    pos = reader.pos;
    count = reader.count;
    markpos = reader.markpos;
    marklimit = reader.marklimit;
    bufferSize = reader.bufferSize;
  }

  public void close()
    throws IOException
  {
    buf = null;
    pos = count = 0;
    markpos = -1;
    super.close();
  }

  public void mark(int readlimit)
    throws IOException
  {
    marklimit = readlimit;
    markpos = pos;
  }

  public boolean markSupported()
  {
    return true;
  }

  public int read()
    throws IOException
  {
    if (pos >= count && !refill())
      return -1;
    //System.out.println("read1:"+new String(buf, pos, 1));
    return (int) buf[pos++];
  }

  public int read(char[] b)
    throws IOException
  {
    return read(b, 0, b.length);
  }

  public int read(char[] b, int off, int len)
    throws IOException
  {
    if (off < 0 || len < 0 || b.length - off < len)
      throw new IndexOutOfBoundsException();
    
    if (len == 0)
      return 0;
    
    if (pos >= count && !refill())
      return -1;
                
    int ret = Math.min(count - pos, len);
    System.arraycopy(buf, pos, b, off, ret);
    //System.out.println("read2:"+new String(b, off, ret));
    pos += ret;
    off += ret;
    len -= ret;
    
    while (len > 0 && refill())
      {
        int remain = Math.min(count - pos, len);
        System.arraycopy(buf, pos, b, off, remain);
        //System.out.println("read3:"+new String(b, off, remain));
        pos += remain;
        off += remain;
        len -= remain;
        ret += remain;
      }
    
    return ret;
  }

  public void reset()
    throws IOException
  {
    if (markpos == -1)
      throw new IOException(buf == null ? "Stream closed." : "Invalid mark.");
    pos = markpos;
  }

  public long skip(long n)
    throws IOException
  {
    if (buf == null)
      throw new IOException("Stream closed.");
    final long origN = n;
    while (n > 0L)
      {
        if (pos >= count && !refill())
          break;
        int numread = (int) Math.min((long) (count - pos), n);
        pos += numread;
        n -= numread;
      }
    return origN - n;
  }

  private boolean refill()
    throws IOException
  {
    if (buf == null)
      throw new IOException("Stream closed.");

    if (markpos == -1 || count - markpos >= marklimit)
      {
        markpos = -1;
        pos = count = 0;
      }
    else
      {
        char[] newbuf = buf;
        if (markpos < bufferSize)
          { 
            newbuf = new char[count - markpos + bufferSize];
          }
        System.arraycopy(buf, markpos, newbuf, 0, count - markpos);
        buf = newbuf;
        count -= markpos;
        pos -= markpos;
        markpos = 0;
      }

    int numread = super.read(buf, count, bufferSize);

    if (numread <= 0)
      return false;

    count += numread;
    return true;
  }

}
