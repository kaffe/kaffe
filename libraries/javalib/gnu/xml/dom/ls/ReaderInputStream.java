/*
 * ReaderInputStream.java
 * Copyright (C) 1999,2000,2001 The Free Software Foundation
 * 
 * This file is part of GNU JAXP, a library.
 *
 * GNU JAXP is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * GNU JAXP is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
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

package gnu.xml.dom.ls;

import java.io.InputStream;
import java.io.IOException;
import java.io.Reader;

/**
 * Character stream wrapper.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
class ReaderInputStream
  extends InputStream
{

  private Reader reader;
  private String encoding;

  ReaderInputStream(Reader reader)
  {
    this.reader = reader;
    encoding = "UTF16";
  }

  void setEncoding(String encoding)
  {
    this.encoding = encoding;
  }

  public int read()
    throws IOException
  {
    return reader.read();
  }

  public int read(byte[] b)
    throws IOException
  {
    return read(b, 0, b.length);
  }

  public int read(byte[] b, int off, int len)
    throws IOException
  {
    int l = len - off;
    char[] c = new char[l];
    l = reader.read(c, 0, l);
    String s = new String(c, 0, l);
    byte[] d = s.getBytes(encoding);
    // FIXME d.length may be > len
    System.arraycopy(d, 0, b, off, d.length);
    return d.length;
  }

  public void close()
    throws IOException
  {
    reader.close();
  }

  public boolean markSupported()
  {
    return reader.markSupported();
  }

  public void mark(int limit)
  {
    try
      {
        reader.mark(limit);
      }
    catch (IOException e)
      {
        throw new RuntimeException(e.getMessage());
      }
  }

  public void reset()
    throws IOException
  {
    reader.reset();
  }

  public long skip(long n)
    throws IOException
  {
    return reader.skip(n);
  }

  // TODO available
  
}

