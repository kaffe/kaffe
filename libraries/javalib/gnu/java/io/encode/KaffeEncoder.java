/*
 * Java core library component.
 *
 * Copyright (c) 2004
 *	Ito Kazumitsu <kaz@maczuka.gcd.org>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package gnu.java.io.encode;

import java.io.ByteArrayOutputStream;
import java.io.CharConversionException;
import java.io.IOException;
import java.io.OutputStream;

import kaffe.io.CharToByteConverter;

/**
  * This class behaves as gnu.java.io.encode.Encoder
  * wrapping Kaffe's kaffe.io.CharToByteConverter.
  *
  * @version 0.0
  *
  * @author Ito Kazumitsu (kaz@maczuka.gcd.org)
  */
public class KaffeEncoder extends Encoder
{

/*************************************************************************/

/*
 * Class Variables
 */
 
/*************************************************************************/

/*
 * Class Methods
 */

/*************************************************************************/

/*
 * Constructors
 */
public
KaffeEncoder(OutputStream out, String enc)
{
  super(out, enc);
  try {
      converter = CharToByteConverter.getConverter(enc);
  }
  catch (java.io.UnsupportedEncodingException _) {
      converter = null;
  }
}

/*************************************************************************/

/*
 * Instance Variables
 */

CharToByteConverter converter;
 
/*************************************************************************/

/*
 * Instance Methods
 */

/**
  * This method is supposed to return the number of bytes that can be
  * converted out of the <code>len</code> chars in the specified array
  * starting at <code>offset</code>.  But it is often the case that
  * it is impossible to calculate such number without actually executing
  * the conversion, and the converter may return a wrong value.
  * So, you should avoid using this method.
  */
public int
bytesInCharArray(char[] buf, int offset, int len)
{
  return (converter.getNumberOfBytes(buf, offset, len));
}

/*************************************************************************/

/**
  * Convert the requested chars to bytes
  */
public byte[]
convertToBytes(char[] buf, int offset, int len)
{

  ByteArrayOutputStream tmpout = new ByteArrayOutputStream(len * 3);
  // Kaffe's java.lang.String.getBytes uses new byte[len * 7].
  // But len * 3 will be enough. 
  byte[] bbuf = new byte[len * 3];
  int bbuflen = converter.convert(buf, offset, len, bbuf, 0, bbuf.length);
  while (bbuflen > 0) {
      tmpout.write(bbuf, 0, bbuflen);
      bbuflen = converter.flush(bbuf, 0, bbuf.length);
  }
  return (tmpout.toByteArray());
}

/**
  * Convert the requested chars to bytes
  */
public byte[]
convertToBytes(char[] buf, int buf_offset, int len, byte[] bbuf, 
               int bbuf_offset)
{
  converter.convert(buf, buf_offset, len,
                    bbuf, bbuf_offset, bbuf.length - bbuf_offset);
  if (converter.flush(new byte[8], 0, 8) > 0) {
      throw new ArrayIndexOutOfBoundsException();
  }
  return(bbuf);
}

/*************************************************************************/

/**
  * Write the requested number of chars to the underlying stream
  */
public void
write(char[] buf, int offset, int len) throws IOException
{
  out.write(convertToBytes(buf, offset, len));
}

} // class KaffEncoder
