/* Encoder.java -- Base class for char->byte encoders
   Copyright (C) 1998, 2004 Free Software Foundation, Inc.

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
import java.io.Writer;

/**
  * This class is the base class for encoding characters into bytes. 
  *
  * @version 0.0
  *
  * @author Aaron M. Renn (arenn@urbanophile.com)
  */
public abstract class Encoder extends Writer
{

/*************************************************************************/

/*
 * Class Variables
 */

/**
  * This is the name of the current encoding. MUST be overriden by
  * subclasses.
  */
private final String scheme_name;

/*************************************************************************/

/*
 * Instance Variables
 */

/**
  * This is the <code>OutputStream</code> bytes are written to
  */
protected final OutputStream out;

/*************************************************************************/

/*
 * Class Methods
 */

/**
  * This method returns the name of the encoding scheme in use
  *
  * @return The name of the encoding scheme
  */
public String
getSchemeName()
{
  return(scheme_name);
}

/*************************************************************************/

/*
 * Constructors
 */

/**
  * This method initializes a new <code>Encoder</code> to write to the
  * specified <code>OutputStream</code>.
  *
  * @param name The character scheme name
  * @param out The <code>OutputStream</code> to read from
  */
protected
Encoder(OutputStream out, String name)
{
  this.out = out;
  this.scheme_name = name;
}

/*************************************************************************/

/*
 * Instance Methods
 */

/**
  * For a given set of chars, this method returns the number of bytes
  * that array will translate into.  
  *
  * If a Unicode character that is not valid in the current encoding
  * scheme is encountered, this method may throw an exception.  But is
  * is not required to.  This method cannot be used to validate an
  * array of chars for a particular encoding.
  *
  * @param buf The array of chars to determine the number of bytes from.
  *
  * @return The number of bytes than will be encoded from the char array
  *
  */
public int
bytesInCharArray(char[] buf)
{
  return(bytesInCharArray(buf, 0, buf.length));
}

/*************************************************************************/

/**
  * For <code>len</code> chars in the specified array, starting from
  * index <code>offset</code>, this method returns the number of bytes
  * that char array will translate into.  
  *
  * If a Unicode character that is not valid in the current encoding
  * scheme is encountered, this method may throw an exception.  But is
  * is not required to.  This method cannot be used to validate an
  * array of chars for a particular encoding.
  *
  * @param buf The array of chars to determine the number of bytes from.
  * @param offset The index to start examining chars from
  * @param len The number of chars to be converted
  *
  * @return The number of bytes than can be encoded from the char array
  *
  */
public abstract int
bytesInCharArray(char[] buf, int offset, int len);

/*************************************************************************/

/**
  * This method converts an array of chars to bytes, returning the result in
  * a newly allocated byte array.
  *
  * @param buf The char array to convert
  *
  * @return The converted bytes array
  *
  */
public byte[]
convertToBytes(char[] buf)
{
  return(convertToBytes(buf, 0, buf.length));
}

/*************************************************************************/

/**
  * This method converts <code>len</code> chars from a specified array to
  * bytes starting at index <code>offset</code> into the array.  The
  * results are returned in a newly allocated byte array.
  *
  * @param buf The char array to convert
  * @param offset The index into the array to start converting from
  * @param len The number of chars to convert
  *
  * @return The converted byte array
  *
  */
public byte[]
convertToBytes(char[] buf, int offset, int len)
{
  byte[] bbuf = new byte[bytesInCharArray(buf, offset, len)];

  return(convertToBytes(buf, offset, len, bbuf, 0));
}

/*************************************************************************/

/**
  * This method converts all the chars in the specified array to bytes
  * and stores them into the supplied byte array starting at index
  * <code>bbuf_offset</code> into the destination byte array.  The array itself
  * is returned as a convenience for passing to other methods.
  *
  * Note that there must be enough space in the destination array to hold
  * all the converted chars, or an exception will be thrown.
  *
  * @param buf The char array to convert
  * @param bbuf The byte array to store converted characters into
  * @param bbuf_offset The index into the byte array to start storing converted bytes.
  *
  * @return The byte array passed by the caller as a param, now filled with converted bytes.
  *
  * @exception ArrayIndexOutOfBoundsException If the destination byte array is not big enough to hold all the converted bytes
  */
public byte[]
convertToBytes(char[] buf, byte[] bbuf, int bbuf_offset)
{
  return(convertToBytes(buf, 0, buf.length, bbuf, bbuf_offset));
}

/*************************************************************************/

/**
  * This method converts <code>len</code> chars in the specified array to 
  * bytes starting at position <code>buf_offset</code> in the array
  * and stores them into the supplied byte array starting at index
  * <code>bbuf_offset</code> into the destination bytes array.  The array itself
  * is returned as a convenience for passing to other methods.
  *
  * Note that there must be enough space in the destination array to hold
  * all the converted bytes, or an exception will be thrown.
  *
  * @param buf The char array to convert
  * @param buf_offset The index into the char array to start converting from
  * @param len The number of chars to convert
  * @param bbuf The byte array to store converted bytes into
  * @param bbuf_offset The index into the byte array to start storing converted bytes.
  *
  * @return The byte array passed by the caller as a param, now filled with converted bytes.
  *
  * @exception ArrayIndexOutOfBoundsException If the destination byte array is not big enough to hold all the converted bytes.
  */
public abstract byte[]
convertToBytes(char[] buf, int buf_offset, int len, byte[] bbuf,
               int bbuf_offset);

/*************************************************************************/

/**
  * Closes this stream and the underlying <code>OutputStream</code>
  * 
  * @exception IOException If an error occurs
  */
public void
close() throws IOException
{
  out.close();
}

/*************************************************************************/

/**
  * This method flushes any buffered bytes to the underlying stream.
  *
  * @exception IOException If an error occurs
  */
public void
flush() throws IOException
{
  out.flush();
}

/*************************************************************************/

} // class Encoder

