/*
 * $Id: LineInputStream.java,v 1.3 2004/07/25 22:46:24 dalibor Exp $
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
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License.
 */

package gnu.inet.util;

import java.io.ByteArrayOutputStream;
import java.io.FilterInputStream;
import java.io.InputStream;
import java.io.IOException;

/**
 * An input stream that can read lines of input.
 *
 * @author <a href="mailto:dog@gnu.org">Chris Burdess</a>
 * @version $Revision: 1.3 $ $Date: 2004/07/25 22:46:24 $
 */
public class LineInputStream extends FilterInputStream
{

  /*
   * Line buffer.
   */
  private ByteArrayOutputStream buf;

  /*
   * Encoding to use when translating bytes to characters.
   */
  private String encoding;

  /*
   * End-of-stream flag.
   */
  private boolean eof;

  /**
   * Constructor using the US-ASCII character encoding.
   * @param in the underlying input stream
   */
  public LineInputStream (InputStream in)
    {
      this (in, "US-ASCII");
    }

  /**
   * Constructor.
   * @param in the underlying input stream
   * @param encoding the character encoding to use
   */
  public LineInputStream (InputStream in, String encoding)
    {
      super (in);
      buf = new ByteArrayOutputStream ();
      this.encoding = encoding;
      eof = false;
    }

  /**
   * Read a line of input.
   */
  public String readLine () throws IOException
    {
      if (eof)
        {
          return null;
        }
      do
        {
          int c = in.read ();
          switch (c)
            {
            case -1:
              eof = true;
            case 10:                // LF
              String ret = buf.toString (encoding);
              buf.reset ();
              return ret;
            default:
              buf.write (c);
            }
        }
      while (true);
    }

}
