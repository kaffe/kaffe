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

/* These three vars are used for the general buffer management */
private int ptr = 0;
private int end = 0;
private char[] buffer = new char[4096];

/* This array is a temporary used during the conversion process. */
private byte[] inbuf = new byte[4096];

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


public int
read() throws IOException
{
    synchronized (lock) {
        if (ptr < end) return buffer[ptr++];
        int r = _read(buffer, 0, buffer.length);
        if (r == -1) return -1;
        ptr = 1;
        end = r;
        return buffer[0];
    }
}

public int
read(char cbuf[], int off, int len) throws IOException
{
    synchronized (lock) {
        int bytesRead = 0;
        if (len < end - ptr) {
            System.arraycopy(buffer, ptr, cbuf, off, len);
            ptr += len;
            return len;
        }

        int preCopy = end - ptr;
        if (preCopy > 0) {
            System.arraycopy(buffer, ptr, cbuf, off, preCopy);
            off += preCopy;
            len -= preCopy;
            bytesRead += preCopy;
        }
        ptr = 0;
        end = 0; 

        int remainder = len % buffer.length;
        int bulkCopy = len - remainder;
        if (bulkCopy > 0) {
            int r = _read(cbuf, off, bulkCopy);
            if (r == -1) {
                return bytesRead == 0 ? -1 : bytesRead;
            }
            off += r;
            len -= r;
            bytesRead += r;
        }

        if (remainder > 0) {
            int r = _read(buffer, 0, buffer.length);
            if (r == -1) {
                return bytesRead == 0 ? -1 : bytesRead;
            }
            end = r;
            int remainderCopy = r < remainder ? r : remainder;
            System.arraycopy(buffer, 0, cbuf, off, remainderCopy);
            off += remainderCopy;
            len -= remainderCopy;
            ptr = remainderCopy;
            bytesRead += remainderCopy;
	}

        return bytesRead;
    }
}

/*
 * Read the requested number of chars from the underlying stream.
 * Some byte fragments may remain in the converter and they are
 * used by the following read.  So read and convertToChars must
 * not be used for the same converter instance.
 *
 * This method *must* be called with lock held, as it uses the
 * instance variable inbuf.
 */
// copied from kaffe's java/io/InputStreamReader.java
private int
_read ( char cbuf[], int off, int len ) throws IOException
{
    if (len < 0 || off < 0 || off + len > cbuf.length) {
            throw new IndexOutOfBoundsException();
    }

    int outlen = 0;
    boolean seenEOF = false;

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
