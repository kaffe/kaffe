/* InputStreamReader.java -- Reader than transforms bytes to chars
   Copyright (C) 1998, 1999, 2001, 2003, 2004, 2005  Free Software Foundation, Inc.

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
Free Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
02111-1307 USA.

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


package java.io;

import gnu.java.io.EncodingManager;
import gnu.java.io.decode.Decoder;

/**
 * This class reads characters from a byte input stream.   The characters
 * read are converted from bytes in the underlying stream by a 
 * decoding layer.  The decoding layer transforms bytes to chars according
 * to an encoding standard.  There are many available encodings to choose 
 * from.  The desired encoding can either be specified by name, or if no
 * encoding is selected, the system default encoding will be used.  The
 * system default encoding name is determined from the system property
 * <code>file.encoding</code>.  The only encodings that are guaranteed to 
 * be availalbe are "8859_1" (the Latin-1 character set) and "UTF8".
 * Unforunately, Java does not provide a mechanism for listing the
 * ecodings that are supported in a given implementation.
 * <p>
 * Here is a list of standard encoding names that may be available:
 * <p>
 * <ul>
 * <li>8859_1 (ISO-8859-1/Latin-1)</li>
 * <li>8859_2 (ISO-8859-2/Latin-2)</li>
 * <li>8859_3 (ISO-8859-3/Latin-3)</li>
 * <li>8859_4 (ISO-8859-4/Latin-4)</li>
 * <li>8859_5 (ISO-8859-5/Latin-5)</li>
 * <li>8859_6 (ISO-8859-6/Latin-6)</li>
 * <li>8859_7 (ISO-8859-7/Latin-7)</li>
 * <li>8859_8 (ISO-8859-8/Latin-8)</li>
 * <li>8859_9 (ISO-8859-9/Latin-9)</li>
 * <li>ASCII (7-bit ASCII)</li>
 * <li>UTF8 (UCS Transformation Format-8)</li>
 * <li>More later</li>
 * </ul>
 * <p>
 * It is recommended that applications do not use 
 * <code>InputStreamReader</code>'s
 * directly.  Rather, for efficiency purposes, an object of this class
 * should be wrapped by a <code>BufferedReader</code>.
 * <p>
 * Due to a deficiency the Java class library design, there is no standard
 * way for an application to install its own byte-character encoding.
 *
 * @see BufferedReader
 * @see InputStream
 *
 * @author Aaron M. Renn (arenn@urbanophile.com)
 * @author Per Bothner (bothner@cygnus.com)
 * @date April 22, 1998.  
 */
public class InputStreamReader extends Reader
{
  /*
   * This is the byte-character decoder class that does the reading and
   * translation of bytes from the underlying stream.
   */
  private Decoder in;

  /**
   * This method initializes a new instance of <code>InputStreamReader</code>
   * to read from the specified stream using the default encoding.
   *
   * @param in The <code>InputStream</code> to read from 
   */
  public InputStreamReader(InputStream in)
  {
    if (in == null)
      throw new NullPointerException();
    this.in = EncodingManager.getDecoder(in);
  }

  /**
   * This method initializes a new instance of <code>InputStreamReader</code>
   * to read from the specified stream using a caller supplied character
   * encoding scheme.  Note that due to a deficiency in the Java language
   * design, there is no way to determine which encodings are supported.
   * 
   * @param in The <code>InputStream</code> to read from
   * @param encoding_name The name of the encoding scheme to use
   *
   * @exception UnsupportedEncodingException If the encoding scheme 
   * requested is not available.
   */
  public InputStreamReader(InputStream in, String encoding_name)
    throws UnsupportedEncodingException
  {
    if (in == null
        || encoding_name == null)
      throw new NullPointerException();
    
    this.in = EncodingManager.getDecoder(in, encoding_name);
  }

  /**
   * This method closes this stream, as well as the underlying 
   * <code>InputStream</code>.
   *
   * @exception IOException If an error occurs
   */
  public void close() throws IOException
  {
    synchronized (lock)
      {
	if (in != null)
	  in.close();
	in = null;
      }
  }

  /**
   * This method returns the name of the encoding that is currently in use
   * by this object.  If the stream has been closed, this method is allowed
   * to return <code>null</code>.
   *
   * @return The current encoding name
   */
  public String getEncoding()
  {
    return in != null ? in.getSchemeName() : null;
  }

  /**
   * This method checks to see if the stream is read to be read.  It
   * will return <code>true</code> if is, or <code>false</code> if it is not.
   * If the stream is not ready to be read, it could (although is not required
   * to) block on the next read attempt.
   *
   * @return <code>true</code> if the stream is ready to be read, 
   * <code>false</code> otherwise
   *
   * @exception IOException If an error occurs
   */
  public boolean ready() throws IOException
  {
    if (in == null)
      throw new IOException("Reader has been closed");
    
    return in.ready();
  }

  /**
   * This method reads up to <code>length</code> characters from the stream into
   * the specified array starting at index <code>offset</code> into the
   * array.
   *
   * @param buf The character array to recieve the data read
   * @param offset The offset into the array to start storing characters
   * @param length The requested number of characters to read.
   *
   * @return The actual number of characters read, or -1 if end of stream.
   *
   * @exception IOException If an error occurs
   */
  public int read (char[] buf, int offset, int length) throws IOException
  {
    if (in == null)
      throw new IOException("Reader has been closed");
    
    return in.read(buf, offset, length);
  }

  /**
   * This method reads a single character of data from the stream.
   *
   * @return The char read, as an int, or -1 if end of stream.
   *
   * @exception IOException If an error occurs
   */
  public int read() throws IOException
  {
    if (in == null)
      throw new IOException("Reader has been closed");
    
    return in.read();
  }

   /**
    * Skips the specified number of chars in the stream.  It
    * returns the actual number of chars skipped, which may be less than the
    * requested amount.
    *
    * @param count The requested number of chars to skip
    *
    * @return The actual number of chars skipped.
    *
    * @exception IOException If an error occurs
    */
   public long skip(long count) throws IOException
   {
     if (in == null)
       throw new IOException("Reader has been closed");
     
     return super.skip(count);
  }
}
