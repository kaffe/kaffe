/*
 * NamedInputStream.java
 * Copyright (C) 2004 The Free Software Foundation
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
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License.
 */
package gnu.xml.libxmlj.util;

import java.io.FilterInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.PushbackInputStream;

/**
 * An input stream associated with an XML system ID.
 * It can report the system ID and the first few bytes of the stream
 * in order to detect the character encoding of the stream.
 *
 * @author <a href='dog@gnu.org'>Chris Burdess</a>
 */
public class NamedInputStream
extends FilterInputStream
{

  private static int DETECT_BUFFER_SIZE = 50;
  
  private String name;

  NamedInputStream (String name, InputStream in, int size)
  {
    super (new PushbackInputStream (in, size));
    this.name = name;
  }

  /**
   * Returns the name of the stream (the XML system ID).
   */
  public String getName ()
  {
    return name;
  }

  /**
   * Returns the first few bytes of the stream for character encoding
   * purposes. The entire stream can thereafter be read normally from the
   * beginning. This method is only valid if no bytes have yet been read
   * from the stream.
   */
  public byte[] getDetectBuffer ()
    throws IOException
  {
    PushbackInputStream p = (PushbackInputStream) in;
    byte[] buffer = new byte[DETECT_BUFFER_SIZE];
    int len = p.read (buffer);
    if (len < 0)
      {
        return null;
      }
    else
      {
        p.unread (buffer, 0, len);
        byte[] ret = new byte[len];
        System.arraycopy (buffer, 0, ret, 0, len);
        return ret;
      }
  }
  
}
