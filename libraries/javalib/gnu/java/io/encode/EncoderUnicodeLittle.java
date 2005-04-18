/* EncoderUnicodeLittle.java -- Encoder for the UTF-16LE encoding, with byte-order markers.
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


package gnu.java.io.encode;

import java.io.IOException;
import java.io.OutputStream;
import java.math.BigInteger;

/**
 * This class implements character encoding in the UCS Transformation Format 16
 * Big Endian (UTF-16BE) encoding scheme.
 * 
 * @version 0.0
 * @author Sven de Marothy
 */
public class EncoderUnicodeLittle extends Encoder
{

  public EncoderUnicodeLittle(OutputStream out)
  {
    super(out, "UnicodeLittle");
  }

  /**
   * Returns the number of bytes the specified char array will be encoded
   * into
   */
  public int bytesInCharArray(char[] buf, int offset, int len)
  {
      return 2 + 2 * len; // 2 extra bytes for the byte-order marker
  }

  /**
   * This method converts a char array to bytes
   */
  public byte[] convertToBytes(char[] buf, int buf_offset, int len,
			       byte[] bbuf, int bbuf_offset)
  {
    int val;
    // Write byte-order marker
    bbuf[bbuf_offset++] = (byte)0xFF;
    bbuf[bbuf_offset++] = (byte)0xFE;

    // Scan the buffer with full validation checks
    for (int i = buf_offset; i < buf_offset + len; i++) {
      byte [] newArray = new BigInteger("" + ((int) buf[i])).toByteArray();
      if (newArray.length < 2)
	{
	  bbuf[bbuf_offset++] = newArray[0];
	  bbuf[bbuf_offset++] = 0;
	}
      else
	{
	  bbuf[bbuf_offset++] = newArray[1];
	  bbuf[bbuf_offset++] = newArray[0];
	}
    }
    return bbuf;
  }

  /**
   * Writes a char array as bytes to the underlying stream.
   */
  public void write(char[] buf, int offset, int len) throws IOException
  {
    byte[] bbuf = new byte[bytesInCharArray(buf, offset, len)];
    convertToBytes(buf, offset, len, bbuf, 0);
    out.write(bbuf);
  }

} // class EncoderUnicodeLittle



