
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package kaffe.util;

import java.io.ByteArrayInputStream;
import java.io.DataInput;
import java.io.DataInputStream;
import java.io.IOException;
import java.io.UTFDataFormatException;

// Indentation is set to two characters in this file for readability

public final class UTF8 {

  // Not instantiable
  private UTF8() {
  }

  /**
   * Decode a UTF-8 encoded string from a byte array. No trailing zero
   * byte is expected.
   *
   * @param	buf			Byte array
   * @exception UTFDataFormatException	If the input is invalid or truncated
   */
  public static String decode(byte[] buf) throws UTFDataFormatException {
    return decode(buf, 0, buf.length);
  }

  /**
   * Decode a UTF-8 encoded string from a byte array. No trailing zero
   * byte is expected.
   *
   * @param	buf			Byte array
   * @param	off			Offset in buf[]
   * @param	len			Exactly how many bytes to read
   * @exception UTFDataFormatException	If the input is invalid or truncated
   */
  public static String decode(byte[] buf, int off, int len)
			throws UTFDataFormatException {
    try {
      return decode(
	  new DataInputStream(new ByteArrayInputStream(buf, off, len)), len);
    } catch (IOException e) {
      Assert.fail();
      return null;
    }
  }

  /**
   * Decode a UTF-8 encoded string from a DataInput. No trailing zero
   * byte is expected.
   *
   * @param	length			Exactly how many bytes to read
   * @exception IOException 		If there is an error reading the input
   * @exception UTFDataFormatException	If the input is invalid or truncated
   */
  public static String decode(DataInput in, int length)
			throws IOException, UTFDataFormatException {
    StringBuffer buf = new StringBuffer();
    int count = 0;

    while (count < length) {
      byte byte1 = (byte)in.readByte();
      count++;

      if ((byte1 & 0x80) == 0x80) {		// Hi-bit set, multi byte char
	if ((byte1 & 0xe0) == 0xc0) {		// Valid 2 byte string '110'
	  if (count++ == length) {
	    throw new UTFDataFormatException("truncated input");
	  }
	  byte byte2 = (byte)in.readByte();
	  if ((byte2 & 0xc0) != 0x80) {		// Valid 2nd byte?
	    throw new UTFDataFormatException("byte #" +count+ " invalid");
	  }
	  buf.append((char)((((int)(byte1 & 0x1f)) << 6) + (byte2 & 0x3f)));
	} else if ((byte1 & 0xf0) == 0xe0) {	// Valid 3 byte string '1110'
	  if (count++ == length) {
	    throw new UTFDataFormatException("truncated");
	  }
	  byte byte2 = (byte)in.readByte();
	  if ((byte2 & 0xc0) != 0x80) {		// Valid 2nd byte?
	    throw new UTFDataFormatException("byte #" +count+ " invalid");
	  }
	  if (count++ == length) {
	    throw new UTFDataFormatException("truncated");
	  }
	  byte byte3 = (byte)in.readByte();
	  if ((byte3 & 0xc0) != 0x80) {		// Valid 3rd byte?
	    throw new UTFDataFormatException("byte #" +count+ " invalid");
	  }
	  buf.append((char)((((int)(byte1 & 0x0f)) << 12)
	      + (((int)(byte2 & 0x3f)) << 6)
	      + (byte3 & 0x3f)));
	} else {				// Bogus first byte
	  throw new UTFDataFormatException("byte #" +count+ " invalid");
	}
      } else {				// Single byte char
	buf.append((char) byte1);
      }
    }
    return buf.toString();
  }

  /**
   * Encode a string into a UTF-8 byte array. No trailing zero
   * byte is added.
   */
  public static byte[] encode(String str) {
    char chars[] = str.toCharArray();
    int size = 0, pos = 0;

    // Size output array
    for (int i = 0; i < chars.length; i++) {
      char ch = chars[i];
      if (ch >= 0x0001 && ch <= 0x007f) {
	size++;
      } else if (ch <= 0x07ff) {
	size += 2;
      } else {
	size += 3;
      }
    }

    // Now fill it in
    byte[] buf = new byte[size];
    for (int i = 0; i < chars.length; i++) {
      char ch = chars[i];
      if (ch >= 0x0001 && ch <= 0x007f) {
	buf[pos++] = (byte) ch;
      } else if (ch <= 0x07ff) {
	buf[pos++] = (byte) (0xc0 | (0x3f & (ch >> 6)));
	buf[pos++] = (byte) (0x80 | (0x3f &  ch));
      } else {
	buf[pos++] = (byte) (0xe0 | (0x0f & (ch >> 12)));
	buf[pos++] = (byte) (0x80 | (0x3f & (ch >>  6)));
	buf[pos++] = (byte) (0x80 | (0x3f &  ch));
      }
    }
    return buf;
  }
}

