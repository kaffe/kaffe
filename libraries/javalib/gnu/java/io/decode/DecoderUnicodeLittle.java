/* DecoderUnicodeLittle.java -- Decoder for the UTF16LE character set, with a byte order marker.
   Copyright (C) 2005 Free Software Foundation, Inc.

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


package gnu.java.io.decode;

import java.io.IOException;
import java.io.InputStream;
import java.math.BigInteger;

/**
 * This class implements character decoding in the UCS Transformation Format 16
 * Little Endian (UTF-16LE) encoding scheme with a byte-order marker.
 * 
 * @version 0.0
 * @author Sven de Marothy
 */
public class DecoderUnicodeLittle extends Decoder
{

  // If we're operating in stream mode and we encounter a surrogate pair that
  // we can't fit in the output buffer, we use this field to store the
  // second half of the surrogate pair.
  private int pendingChar = -1;

  public DecoderUnicodeLittle(InputStream in)
  {
    super(in, "UnicodeLittle");
  }

  /**
   * Counts the number of characters in a given byte array
   */
  public int charsInByteArray(byte[] buf, int offset, int len)
  {
    int num_chars = 0;
    // Scan the buffer with minimal validation checks
    for (int i = offset; i < offset + len; )
      {
	if (i + 1 < buf.length)
	  {
	    int word = new BigInteger(new byte[]
		{ buf[i + 1], buf[i] }).intValue();
	    if (word == -257)
	      {
		i += 2;
		if (i >= buf.length)
		  break;
	      }
	    else if (word >= 0xD800 && word <= 0xDBFF)
	      {
		if ((i + 3) >= buf.length)
		  break;
		int wordtrail = new BigInteger(new byte[]
		    { buf[i + 3], buf[i + 2] }).intValue();

		if (wordtrail >= 0xDC00 && wordtrail <= 0xDFFF)
		  {
		    ++num_chars;
		    i += 4;
		    if (i >= buf.length)
		      break;
		  }
		else
		  {
		    i += 2;
		    if (i >= buf.length)
		      break;
		  }
	      }
	    else if (word >= 0xDC00 && word <= 0xDFFF)
	      {
		i += 2;
		if (i >= buf.length)
		  break;
	      }
	    else
	      {
		++num_chars;
		i += 2;
		if (i >= buf.length)
		  break;
	      }
	  }
	else
	  break;
      }

    return num_chars;
  }
  
  /**
   * Transform the specified UnicodeLittle encoded buffer to Unicode characters
   */
  public char[] convertToChars(byte[] buf, int buf_offset, int len,
			       char cbuf[], int cbuf_offset)
  {
    int val;
    // Scan the buffer with full validation checks
    for (int i = buf_offset; i < buf_offset + len; )
      {
	if (i + 1 < buf.length)
	  {
	    int word = new BigInteger(new byte[]
		{ buf[i + 1], buf[i] }).intValue();

	    if (word == -257)
	      {
		i += 2;
		if (i >= buf.length)
		  break;
	      }
	    else if (word >= 0xD800 && word <= 0xDBFF)
	      {
		if ((i + 3) >= buf.length)
		  break;
		int wordtrail = new BigInteger(new byte[]
		    { buf[i + 3], buf[i + 2] }).intValue();
		
		if (wordtrail >= 0xDC00 && wordtrail <= 0xDFFF)
		  {
		    cbuf[cbuf_offset++] = (char) word;
		    cbuf[cbuf_offset++] = (char) wordtrail;
		    i += 4;
		    if (i >= buf.length)
		      break;
		  }
		else
		  {
		    // ELSE NOT LEGAL SURROGATE POSITION, IGNORE
		    i += 2;
		    if (i >= buf.length)
		      break;
		  }
	      }
	    else if (word >= 0xDC00 && word <= 0xDFFF)
	      {
		// NOT LEGAL SURROGATE POSITION, IGNORE
		i += 2;
		if (i >= buf.length)
		  break;
	      }
	    else
	      {
		cbuf[cbuf_offset++] = (char) word;
		i += 2;
		if (i >= buf.length)
		  break;
	      }
	  }
	else
	  {
	    // UNEXPECTED END, IGNORE
	    break;
	  }
      }

    return cbuf;
  }
  
  /** ********************************************************************** */
  
  /**
   * Reads chars from a UnicodeLittle encoded byte stream
   */
  public int read(char[] cbuf, int offset, int len) throws IOException
  {
    byte[] buffer = new byte[4];
    for (int i = offset; i < offset + len; )
      {
	// Read a byte
	int b = in.read();
	if (b == -1)
	  if (i == offset)
	    return -1;
	  else
	    return i - offset;
	buffer[0] = (byte) b;
	b = in.read();
	if (b == -1)
	  break;
	buffer[1] = (byte) b;
	int word = new BigInteger(new byte[]
	    { buffer[1], buffer[0] }).intValue();

	if (word == -257)
	  {
	    if (in.available() <= 0)
	      return (1 + i - offset);
	    continue;
	  }
	else if (word >= 0xD800 && word <= 0xDBFF)
	  {
	    b = in.read();
	    if (b == -1)
	      break;
	    buffer[2] = (byte) b;
	    b = in.read();
	    if (b == -1)
	      break;
	    buffer[3] = (byte) b;
	    int wordtrail = new BigInteger(new byte[]
		{ buffer[3], buffer[2] }).intValue();

	    if (wordtrail >= 0xDC00 && wordtrail <= 0xDFFF)
	      {
		cbuf[i++] = (char) word;
		if (i < offset + len)
		  cbuf[i] = (char) wordtrail;
		else
		  pendingChar = (char) wordtrail;
	      }
	    else
	      {
		cbuf[i] = (char) wordtrail;
		if (in.available() <= 0)
		  return (1 + i - offset);
		continue;
	      }
	  }
	else if (word >= 0xDC00 && word <= 0xDFFF)
	  {
	    if (in.available() <= 0)
	      return (1 + i - offset);
	    continue;
	  }
	else
	  cbuf[i] = (char) word;

	// if no more bytes available, terminate loop early, instead of
	// blocking in in.read().
	// Do not test this in the for condition: it must call in.read() at
	// least once (and thus block if "in" is empty).
	if (in.available() <= 0)
	  return 1 + i - offset;

	i++;
      }

    return len;
  }
  
} // class DecoderUnicodeLittle
