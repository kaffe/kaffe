/* DecoderEightBitLookup.java -- Decodes eight-bit encodings
   Copyright (C) 1998, 2001, 2004 Free Software Foundation, Inc.

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

/**
  * Numerous character encodings utilize only eight bits.  These can
  * be easily and efficiently be converted to characters using lookup tables.
  * This class is the common superclass of all <code>Decoder</code> classes
  * that use eight bit lookup tables.  All a subclass implementor has to
  * do is define an encoding name and create a class consisting of a 
  * static lookup table overriding the default. 
  *
  * @version 0.0
  *
  * @author Aaron M. Renn (arenn@urbanophile.com)
  */
public abstract class DecoderEightBitLookup extends Decoder
{

/*************************************************************************/

/*
 * Class Variables
 */
 
/**
  * This is the lookup table.  Subclasses must allocate a 255 byte char
  * array and put each Unicode character corresponding to the eight bit
  * byte in the appropriate index slot.  For example, to convert 0xE3 to
  * \u3768, put \u3768 at index 227 (0xE3) in the lookup table.
  */
private final char[] lookup_table;

/*************************************************************************/

/*
 * Constructors
 */
protected
DecoderEightBitLookup(InputStream in, String name, char[] table)
{
  super(in, name);
  this.lookup_table = table;
}

/*************************************************************************/

/*
 * Instance Methods
 */

/**
  * This method returns the number of chars that can be converted out of
  * the <code>len</code> bytes in the specified array starting at
  * <code>offset</code>.  This will be identical to the number of bytes
  * in that range, i.e., <code>len</code>.
  */
public int
charsInByteArray(byte[] buf, int offset, int len)
{
  return(len);
}

/*************************************************************************/

/**
  * Convert the requested bytes to chars
  */
public char[]
convertToChars(byte[] buf, int buf_offset, int len, char[] cbuf, 
               int cbuf_offset)
{
  for (int i = 0; i < len; i++)
    cbuf[cbuf_offset + i] = lookup_table[buf[buf_offset + i] & 0xff];

  return(cbuf);
}

/*************************************************************************/

/**
  * Read the requested number of chars from the underlying stream
  */
public int
read(char[] cbuf, int offset, int len) throws IOException
{
  byte[] buf = new byte[len];
  
  int bytes_read = in.read(buf);
  if (bytes_read == -1)
    return(-1);

  convertToChars(buf, 0, bytes_read, cbuf, offset);
  return(bytes_read); 
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

} // class DecoderEightBitLookup

