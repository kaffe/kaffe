/* Decoder.java -- Base class for byte->char decoders
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


package gnu.java.io.decode;

import java.io.IOException;
import java.io.InputStream;
import java.io.Reader;

/**
  * This class is the base class for decoding bytes into character. 
  *
  * @version 0.0
  *
  * @author Aaron M. Renn (arenn@urbanophile.com)
  */
public abstract class Decoder extends Reader
{

/*************************************************************************/

/*
 * Class Variables
 */

/**
  * This is the name of the current encoding. Set in the constructor.
  */
private final String scheme_name;

/*************************************************************************/

/*
 * Instance Variables
 */

/**
  * This is the <code>InputStream</code> bytes are read from
  */
protected final InputStream in;

/*************************************************************************/

/*
 * Constructors
 */

/**
  * This method initializes a new <code>Decoder</code> to read from the
  * specified <code>InputStream</code>.
  *
  * @param in The <code>InputStream</code> to read from
  * @param name The character scheme name
  */
protected
Decoder(InputStream in, String name)
{
  this.in = in;
  this.scheme_name = name;
}

/*************************************************************************/

/*
 * Instance Methods
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
/**
  * For a given set of bytes, this method returns the number of characters
  * that byte array will translate into.  If the bytes do not all translate 
  * into an even number of charcters, an exception will be thrown.    
  * Additionally, an exception may be thrown if any of the bytes are not
  * valid for the given encoding.  (This is not guaranteed to happen however).
  *
  * @param buf The array of bytes to determine the number of characters from.
  *
  * @return The number of characters than can be decoded from the byte array
  *
  */
public int
charsInByteArray(byte[] buf)
{
  return(charsInByteArray(buf, 0, buf.length));
}

/*************************************************************************/

/**
  * For a <code>len</code> bytes in the specified array, starting from
  * index <code>offset</code>, this method returns the number of characters
  * that byte array will translate into.  If the bytes do not all translate 
  * into an even number of charcters, an exception will be thrown.    
  * Additionally, an exception may be thrown if any of the bytes are not
  * valid for the given encoding.  (This is not guaranteed to happen however).
  *
  * @param buf The array of bytes to determine the number of characters from.
  * @param offset The index to start examining bytes from
  * @param len The number of bytes to be converted
  *
  * @return The number of characters than can be decoded from the byte array
  *
  */
public abstract int
charsInByteArray(byte[] buf, int offset, int len);

/*************************************************************************/

/**
  * This method converts an array of bytes to chars, returning the result in
  * a newly allocated char array.
  *
  * @param buf The byte array to convert
  *
  * @return The converted char array
  *
  */
public char[]
convertToChars(byte[] buf)
{
  return(convertToChars(buf, 0, buf.length));
}

/*************************************************************************/

/**
  * This method converts <code>len</code> bytes from a specified array to
  * characters starting at index <code>offset</code> into the array.  The
  * results are returned in a newly allocated char array.
  *
  * @param buf The byte array to convert
  * @param offset The index into the array to start converting from
  * @param len The number of bytes to convert
  *
  * @return The converted char array
  *
  */
public char[]
convertToChars(byte[] buf, int offset, int len)
{
  char[] cbuf = new char[charsInByteArray(buf, offset, len)];

  return(convertToChars(buf, offset, len, cbuf, 0));
}

/*************************************************************************/

/**
  * This method converts all the bytes in the specified array to characters
  * and stores them into the supplied character array starting at index
  * <code>cbuf_offset</code> into the destination char array.  The array itself
  * is returned as a convenience for passing to other methods.
  *
  * Note that there must be enough space in the destination array to hold
  * all the converted bytes, or an exception will be thrown.
  *
  * @param buf The byte array to convert
  * @param cbuf The char array to store converted characters into
  * @param cbuf_offset The index into the char array to start storing converted characters.
  *
  * @return The char array passed by the caller as a param, now filled with converted characters.
  *
  * @exception ArrayIndexOutOfBoundsException If the destination char array is not big enough to hold all the converted characters
  */
public char[]
convertToChars(byte[] buf, char[] cbuf, int cbuf_offset)
{
  return(convertToChars(buf, 0, buf.length, cbuf, cbuf_offset));
}

/*************************************************************************/

/**
  * This method converts <code>len</code> bytes in the specified array to 
  * characters starting at position <code>buf_offset</code> in the array
  * and stores them into the supplied character array starting at index
  * <code>cbuf_offset</code> into the destination char array.  The array itself
  * is returned as a convenience for passing to other methods.
  *
  * Note that there must be enough space in the destination array to hold
  * all the converted bytes, or an exception will be thrown.
  *
  * @param buf The byte array to convert
  * @param buf_offset The index into the byte array to start converting from
  * @param len The number of bytes to convert
  * @param cbuf The char array to store converted characters into
  * @param cbuf_offset The index into the char array to start storing converted characters.
  *
  * @return The char array passed by the caller as a param, now filled with converted characters.
  *
  * @exception ArrayIndexOutOfBoundsException If the destination char array is not big enough to hold all the converted characters
  */
public abstract char[]
convertToChars(byte[] buf, int buf_offset, int len, char[] cbuf,
               int cbuf_offset);

/*************************************************************************/

/**
  * Closes this stream and the underlying <code>InputStream</code>
  * 
  * @exception IOException If an error occurs
  */
public void
close() throws IOException
{
  in.close();
}

/*************************************************************************/

/**
  * This method returns <code>false</code> to indicate that there is no
  * guarantee this stream can be read successfully without blocking.  This
  * is because even if bytes are available from the underlying
  * <code>InputStream</code>, this method does not know if a particular
  * encoding requires more than that number of bytes or not.  Subclasses
  * that can make that determination should override this method.
  * 
  * @return <code>false</code> since there is no guarantee this stream is ready to be read
  *
  * @exception IOException If an error occurs
  */
public boolean
ready() throws IOException
{
  return(false);
}

} // class Decoder

