/*
 * Java core library component.
 *
 * Copyright (c) 2004
 *	Ito Kazumitsu <kaz@maczuka.gcd.org>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package gnu.java.io.decode;

import java.io.IOException;
import java.io.InputStream;

import kaffe.io.ByteToCharConverter;

/**
  * This class behaves as gnu.java.io.decode.Decoder
  * wrapping Kaffe's kaffe.io.ByteToCharConverter.
  *
  * @version 0.0
  *
  * @author Ito Kazumitsu (kaz@maczuka.gcd.org)
  */
public class KaffeDecoder extends Decoder
{

/*************************************************************************/

/*
 * Class Variables
 */
 
/*************************************************************************/

/*
 * Constructors
 */
public
KaffeDecoder(InputStream in, String enc)
   throws java.io.UnsupportedEncodingException
{
  super(in, enc);
  // try {
      converter = ByteToCharConverter.getConverter(enc);
  // }
  // catch (java.io.UnsupportedEncodingException _) {
  //     converter = null;
  // }
}

/*
 * Instance Variables
 */

ByteToCharConverter converter;

/*************************************************************************/

/*
 * Instance Methods
 */

/**
  * This method is supposed to return the number of chars that can be
  * converted out of the <code>len</code> bytes in the specified array
  * starting at <code>offset</code>.  But it is often the case that
  * it is impossible to calculate such number without actually executing
  * the conversion, and the converter may return a wrong value.
  * So, you should avoid using this method.
  */

public int
charsInByteArray(byte[] buf, int offset, int len)
{
  return (converter.getNumberOfChars(buf, offset, len));
}

/**
  * Convert the requested bytes to chars
  */
public char[]
convertToChars(byte[] buf, int buf_offset, int len)
{
  char[] cbuf = new char[len];
  int olen = converter.convert(buf, buf_offset, len,
                    cbuf, 0, cbuf.length);
  char[] ret = new char[olen];
  System.arraycopy(cbuf, 0, ret, 0, olen);
  return(ret);
}

/**
  * Convert the requested bytes to chars
  */
public char[]
convertToChars(byte[] buf, int buf_offset, int len,
               char[] cbuf, int cbuf_offset)
{
  converter.convert(buf, buf_offset, len,
                    cbuf, cbuf_offset, cbuf.length - cbuf_offset);
  return(cbuf);
}

/**
  * Read the requested number of chars from the underlying stream.
  * Some byte fragments may remain in the converter and they are
  * used by the following read.  So read and convertToChars must
  * not be used for the same converter instance.
  */
// copied from kaffe's java/io/InputStreamReader.java
public int
read ( char cbuf[], int off, int len ) throws IOException
{
    if (len < 0 || off < 0 || off + len > cbuf.length) {
            throw new IndexOutOfBoundsException();
    }

    int outlen = 0;
    boolean seenEOF = false;

    byte[] inbuf = new byte[2048];

    while (len > outlen) {
        // First we retreive anything left in the converter
        int inpos = converter.withdraw(inbuf, 0, inbuf.length);
        int n = len - outlen;
        int m = inbuf.length - inpos;
        if (n > m) {
            n = m;
        }
        int inlen = in.read(inbuf, inpos, n);
        if (inlen < 0) {
            inlen = 0;
            seenEOF = true;
        }
        outlen += converter.convert(inbuf, 0, inpos+inlen, cbuf,
                           off+outlen, len-outlen);
        if (inlen < n || !ready()) {
            break;
        }
    }
    if (seenEOF && !converter.havePending()) {
        return (-1);
    }
    return (outlen);
}


/**
  * Checks if bytes are available in the underlying inputstream. If this 
  * is true at least one character can be read without blocking. 
  * 
  * @return <code>true</code> iff there are > 0 bytes available for
  * the underlying InputStream.
  *
  * @exception IOException If an error occurs
  */
public boolean
ready() throws IOException
{
  return (in.available() > 0);
}

} // class KaffeDecoder
