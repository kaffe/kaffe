/* Encoder.java -- Base class for char->byte encoders
   Copyright (C) 1998 Free Software Foundation, Inc.

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

import java.io.OutputStream;
import java.io.Writer;
import java.io.IOException;
import java.io.CharConversionException;

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
protected static String scheme_name = "undefined";

/**
  * This is a description of the current encoding.  MUST be overridden
  * by subclasses.
  */
protected static String scheme_description = "undefined";

/*************************************************************************/

/*
 * Instance Variables
 */

/**
  * This is the <code>OutputStream</code> bytes are written to
  */
protected OutputStream out;

/**
  * This is the value that is substituted for bad characters that can't
  * be encoded.
  */
protected char bad_char;

/**
  * This indicates whether or not the bad char is set or not
  */
protected boolean bad_char_set;

/*************************************************************************/

/*
 * Class Methods
 */

/**
  * This method returns the name of the encoding scheme in use
  *
  * @return The name of the encoding scheme
  */
public static String
getSchemeName()
{
  return(scheme_name);
}

/*************************************************************************/

/**
  * This method returns a description of the encoding scheme in use
  *
  * @param A description of the decoding scheme.
  */
public static String
getSchemeDescription()
{
  return(scheme_description);
}

/*************************************************************************/

/*
 * Constructors
 */

/**
  * This method initializes a new <code>Encoder</code> to write to the
  * specified <code>OutputStream</code>.
  *
  * @param out The <code>OutputStream</code> to read from
  */
public
Encoder(OutputStream out)
{
  this.out = out;
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
  * @exception CharConversionException If bad char value are encountered for this encoding
  */
public int
bytesInCharArray(char[] buf) throws CharConversionException
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
  * @exception CharConversionException If bad char value are encountered for this encoding
  */
public abstract int
bytesInCharArray(char[] buf, int offset, int len) throws CharConversionException;

/*************************************************************************/

/**
  * This method converts an array of chars to bytes, returning the result in
  * a newly allocated byte array.
  *
  * @param buf The char array to convert
  *
  * @return The converted bytes array
  *
  * @exception CharConversionException If an error occurs
  */
public byte[]
convertToBytes(char[] buf) throws CharConversionException
{
  return(convertToBytes(buf, 0, buf.length));
}

/*************************************************************************/

/**
  * This method converts <code>len<code> chars from a specified array to
  * bytes starting at index <code>offset</code> into the array.  The
  * results are returned in a newly allocated byte array.
  *
  * @param buf The char array to convert
  * @param offset The index into the array to start converting from
  * @param len The number of chars to convert
  *
  * @return The converted byte array
  *
  * @exception CharConversionException If an error occurs.
  */
public byte[]
convertToBytes(char[] buf, int offset, int len) throws CharConversionException
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
  * @exception CharConversionException If any other error occurs.
  */
public byte[]
convertToBytes(char[] buf, byte[] bbuf, int bbuf_offset) throws
                  CharConversionException
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
  * @exception CharConversionException If any other error occurs.
  */
public abstract byte[]
convertToBytes(char[] buf, int buf_offset, int len, byte[] bbuf,
               int bbuf_offset) throws CharConversionException;

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

/**
  * This method sets that character that will be used when converting
  * a Unicode character that is invalid in the current encoding.  If this 
  * is set, it will be substituted for the bad value.  Otherwise, an
  * exception will be thrown.  Note that this character itself must be
  * valid for the current encoding.  If it is not, an exception is thrown.
  *
  * @param bad_char The substitute for any bad characters found.
  *
  * @exception CharConversionException If the bad char value is not valid in this encoding
  */
public void
setBadCharValue(char bad_char) throws CharConversionException
{
  char[] buf = new char[1];
  buf[0] = bad_char;
  convertToBytes(buf);

  this.bad_char = bad_char;
  bad_char_set = true;
}

} // class Encoder

