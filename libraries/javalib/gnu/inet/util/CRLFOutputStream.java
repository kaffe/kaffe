/*
 * $Id: CRLFOutputStream.java,v 1.4 2004/10/04 19:34:03 robilad Exp $
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

import java.io.FilterOutputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.io.UnsupportedEncodingException;

/** 
 * An output stream that filters LFs into CR/LF pairs.
 *
 * @author <a href="mailto:dog@gnu.org">Chris Burdess</a>
 * @author $Revision: 1.4 $ $Date: 2004/10/04 19:34:03 $
 */
public class CRLFOutputStream extends FilterOutputStream
{

  static final String US_ASCII = "US-ASCII";

  /**
   * The CR octet.
   */
  public static final int CR = 13;

  /**
   * The LF octet.
   */
  public static final int LF = 10;

  /**
   * The CR/LF pair.
   */
  public static final byte[] CRLF = { CR, LF };

  /**
   * The last byte read.
   */
  protected int last;

  /**
   * Constructs a CR/LF output stream connected to the specified output stream.
   */
  public CRLFOutputStream (OutputStream out)
    {
      super (out);
      last = -1;
    }

  /**
   * Writes a character to the underlying stream.
   * @exception IOException if an I/O error occurred
   */
  public void write (int ch) throws IOException
    {
      if (ch == CR)
        {
          out.write (CRLF);
        }
      else if (ch == LF)
        {
          if (last != CR)
            {
              out.write (CRLF);
            }
        }
      else
        {
          out.write (ch);
        }
      last = ch;
    }

  /**
   * Writes a byte array to the underlying stream.
   * @exception IOException if an I/O error occurred
   */
  public void write(byte[] b) throws IOException
    {
      write (b, 0, b.length);
    }

  /**
   * Writes a portion of a byte array to the underlying stream.
   * @exception IOException if an I/O error occurred
   */
  public void write (byte[] b, int off, int len) throws IOException
    {
      int d = off;
      len += off;
      for (int i = off; i < len; i++)
        {
          switch (b[i])
            {
            case CR:
              out.write (b, d, i - d);
              out.write (CRLF, 0, 2);
              d = i + 1;
              break;
            case LF:
              if (last != CR)
                {
                  out.write (b, d, i - d);
                  out.write (CRLF, 0, 2);
                }
              d = i + 1;
              break;
            }
          last = b[i];
        }
      if (len - d > 0)
        {
          out.write (b, d, len - d);
        }
    }

  /**
   * Writes the specified ASCII string to the underlying stream.
   * @exception IOException if an I/O error occurred
   */
  public void write (String text) throws IOException
    {
      try
        {
          byte[] bytes = text.getBytes (US_ASCII);
          write (bytes, 0, bytes.length);
        }
      catch (UnsupportedEncodingException e)
        {
          throw new IOException ("The US-ASCII encoding is not supported " +
                                 "on this system");
        }
    }

  /**
   * Writes a newline to the underlying stream.
   * @exception IOException if an I/O error occurred
   */
  public void writeln () throws IOException
    {
      out.write (CRLF, 0, 2);
    }

}
