/*
 * $Id: MessageInputStream.java,v 1.4 2004/10/04 19:34:03 robilad Exp $
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

import java.io.FilterInputStream;
import java.io.InputStream;
import java.io.IOException;

/**
 * A utility class for feeding message contents to messages.
 * This stream returns -1 from <code>read</code> when the stream termination
 * sequence LF,END,LF is read from the underlying stream.
 *
 * @author <a href="mailto:dog@gnu.org">Chris Burdess</a>
 * @version $Revision: 1.4 $ $Date: 2004/10/04 19:34:03 $
 */
public class MessageInputStream extends FilterInputStream
{

  /**
   * The stream termination octet ('.').
   */
  public static final int END = 46;

  /**
   * The line termination octet ('\n').
   */
  public static final int LF = 10;

  protected boolean eof;

  protected int buf1 = -1;
  protected int buf2 = -1;

  protected int markBuf1;
  protected int markBuf2;

  /**
   * Constructs a message input stream connected to the specified input stream.
   */
  public MessageInputStream (InputStream in)
    {
      super (in);
      eof = false;
    }

  /**
   * Reads the next byte of data from this message input stream.
   * Returns -1 if the end of the message stream has been reached.
   * @exception IOException if an I/O error occurs
   */
  public int read () throws IOException
    {
      if (eof)
        {
          return -1;
        }
      int c;
      if (buf1 != -1)
        {
          c = buf1;
          buf1 = buf2;
          buf2 = -1;
        }
      else
        {
          c = super.read ();
        }
      if (c == LF)
        {
          if (buf1 == -1)
            {
              buf1 = super.read ();
              if (buf1 == END)
                {
                  buf2 = super.read ();
                  if (buf2 == LF)
                    {
                      eof = true;
                      // Allow the final LF to be read
                    }
                }
            }
          else if (buf1 == END)
            {
              if (buf2 == -1)
                {
                  buf2 = super.read ();
                  if (buf2 == LF)
                    {
                      eof = true;
                    }
                }
              else if (buf2 == LF)
                {
                  eof = true;
                }
            }
        }
      return c;
    }

  /**
   * Reads up to b.length bytes of data from this input stream into
   * an array of bytes.
   * Returns -1 if the end of the stream has been reached.
   * @exception IOException if an I/O error occurs
   */
  public int read (byte[] b) throws IOException
    {
      return read (b, 0, b.length);
    }

  /**
   * Reads up to len bytes of data from this input stream into an
   * array of bytes, starting at the specified offset.
   * Returns -1 if the end of the stream has been reached.
   * @exception IOException if an I/O error occurs
   */
  public int read (byte[] b, int off, int len) throws IOException
    {
      if (eof)
        {
          return -1;
        }
      int c, end = off + len;
      for (int i = off; i < end; i++)
        {
          c = read ();
          if (c == -1)
            {
              len = i - off;
              break;
            }
          else
            {
              b[i] = (byte) c;
            }
        }
      return len;
    }

  public boolean markSupported ()
    {
      return in.markSupported ();
    }

  public void mark (int readlimit)
    {
      in.mark (readlimit);
      markBuf1 = buf1;
      markBuf2 = buf2;
    }

  public void reset () throws IOException
    {
      in.reset ();
      buf1 = markBuf1;
      buf2 = markBuf2;
    }

}
