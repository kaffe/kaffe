/* CharBuffer.java -- 
   Copyright (C) 2002 Free Software Foundation, Inc.

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

package java.nio;

import gnu.java.nio.CharBufferImpl;

/**
 * @since 1.4
 */
public abstract class CharBuffer extends Buffer
  implements Comparable, CharSequence
{
  private ByteOrder endian = ByteOrder.BIG_ENDIAN;

  protected char [] backing_buffer;

  protected int array_offset;

  /**
   * Allocats a non-direct character buffer.
   */
  public static CharBuffer allocate(int capacity)
  {
    return new CharBufferImpl (capacity, 0, capacity);
  }
  
  /**
   * Wraps a character array into a buffer.
   * 
   * @exception IndexOutOfBoundsException If the preconditions on the offset
   * and length parameters do not hold
   */
  final public static CharBuffer wrap (char[] array, int offset, int length)
  {
    return new CharBufferImpl (array, offset, offset + length);
  }

  /**
   * Wraps a character array into a buffer.
   */
  final public static CharBuffer wrap (char[] array)
  {
    return wrap (array, 0, array.length);
  }
  
  /**
   * Wraps a character sequence into a buffer.
   * 
   * @exception IndexOutOfBoundsException If the preconditions on the offset
   * and length parameters do not hold
   */
  final public static CharBuffer wrap (CharSequence cs, int offset, int length)
  {
    return wrap (cs.toString ().toCharArray (), 0, length);
  }
  
  /**
   * Wraps a character sequence into a buffer.
   */
  final public static CharBuffer wrap (CharSequence cs)
  {
    return wrap (cs, 0, cs.length ());
  }
 
  /**
   * Relative bulk get method.
   *
   * @exception BufferUnderflowException If there are fewer than length
   * characters remaining in this buffer.
   * @exception IndexOutOfBoundsException If the preconditions on the offset
   * and length parameters do not hold.
   */
  public CharBuffer get (char[] dst, int offset, int length)
  {
    if (offset < 0 ||
        offset > dst.length ||
        length < 0 ||
        length > (dst.length - offset))
      throw new IndexOutOfBoundsException ();

    for (int i = offset; i < offset + length; i++)
      dst [i] = get ();
    
    return this;
  }

  /**
   * Relative bulk get method.
   *
   * @exception BufferUnderflowException If there are fewer than length
   * characters remaining in this buffer.
   */
  public CharBuffer get (char[] dst)
  {
    return get (dst, 0, dst.length);
  }
 
  /**
   * Relative bulk put method.
   *
   * @exception BufferOverflowException If there is insufficient space in this
   * buffer for the remaining characters in the source buffer.
   * @exception IllegalArgumentException If the source buffer is this buffer.
   * @exception ReadOnlyBufferException If this buffer is read-only.
   */
  public CharBuffer put (CharBuffer src)
  {
    if (src == this)
      throw new IllegalArgumentException ();

    while (src.hasRemaining ())
      {
        put (src.get ());
      }

    return this;
  }
 
  /**
   * Relative bulk put method.
   *
   * @exception BufferOverflowException If there is insufficient space in this
   * buffer for the remaining characters in the source buffer.
   * @exception IndexOutOfBoundsException If the preconditions on the offset
   * and length parameters do not hold.
   * @exception ReadOnlyBufferException If this buffer is read-only.
   */
  public CharBuffer put (char[] src, int offset, int length)
  {
    if (offset < 0 ||
        offset > src.length ||
        length < 0 ||
        length > (src.length - offset))
      throw new IndexOutOfBoundsException ();
    
    for (int i = offset; i < offset + length; i++)
      {
        put (src[i]);
      }
    
    return this;
  }

  /**
   * Relative bulk put method.
   *
   * @exception BufferOverflowException If there is insufficient space in this
   * buffer for the remaining characters in the source buffer.
   * @exception IndexOutOfBoundsException If the preconditions on the start and
   * end parameters do not hold.
   * @exception ReadOnlyBufferException If this buffer is read-only.
   */
  public CharBuffer put(String src, int offset, int length)
  {
    return put (src.toCharArray (), offset, length);
  }

  /**
   * Relative bulk put method.
   *
   * @exception BufferOverflowException If there is insufficient space in this
   * buffer for the remaining characters in the source buffer.
   * @exception ReadOnlyBufferException If this buffer is read-only.
   */
  public final CharBuffer put(String src)
  {
    return put (src, 0, src.length ());
  }

  /**
   * This method transfers the entire content of the given
   * source character array into this buffer.
   *
   * @param src The source character array to transfer.
   *
   * @exception BufferOverflowException If there is insufficient space
   * in this buffer.
   * @exception ReadOnlyBufferException If this buffer is read-only.
   */
  public final CharBuffer put (char[] src)
  {
    return put (src, 0, src.length);
  }

  /**
   * Tells wether this buffer has a backing array or not.
   */
  public final boolean hasArray ()
  {
    return backing_buffer != null;
  }

  /**
   * Retrieves the backing buffer.
   *
   * @exception ReadOnlyBufferException If this buffer is read-only.
   * @exception UnsupportedOperationException If this buffer is not backed by an accessible array.
   */
  public final char[] array ()
  {
    if (!hasArray ())
      throw new UnsupportedOperationException ();

    if (isReadOnly ())
      throw new ReadOnlyBufferException ();

    return backing_buffer;
  }

  /**
   * Returns the offset within this buffer's backing array of the first element of the buffer.
   *
   * @exception ReadOnlyBufferException If this buffer is read-only.
   * @exception UnsupportedOperationException If this buffer is not backed by an accessible array.
   */  
  public final int arrayOffset ()
  {
    if (!hasArray ())
      throw new UnsupportedOperationException ();

    if (isReadOnly ())
      throw new ReadOnlyBufferException ();

    return array_offset;
  }
  
  /**
   * Returns the current hash code of this buffer.
   */
  public int hashCode ()
  {
    // FIXME
    return super.hashCode ();
  }
  
  /**
   * Tells whether or not this buffer is equal to another object.
   */
  public boolean equals (Object obj)
  {
    if (obj instanceof CharBuffer)
      return compareTo (obj) == 0;
    
    return false;
  }

  /**
   * Creates a new character buffer that represents the specified subsequence
   * of this buffer, relative to the current position.
   *
   * @exception IndexOutOfBoundsException If the preconditions on start and end
   * do not hold.
   */
  public abstract CharSequence subSequence (int start, int end);

  /**
   * Returns the length of this character buffer.
   */
  public final int length ()
  {
    return remaining ();
  }

  /**
   * Reads the character at the given index relative to the current position.
   *
   * @exception IndexOutOfBoundsException If the preconditions on index
   * do not hold.
   */
  public final char charAt (int index)
  {
    if (index < 0 ||
        index >= length ())
      throw new IndexOutOfBoundsException ();

    return get (position () + index);
  }

  /**
   * Retrieves the content of the character buffer as string.
   */
  public String toString()
  {
    if (hasArray ())
      {
        return new String (array (), position (), length ());
      }

    StringBuffer sb = new StringBuffer (length ());

    for (int i = position (); i < limit (); ++i)
      sb.append (get (i));

    return sb.toString ();
  }

  /**
   * Compares this buffer to another object.
   *
   * @exception ClassCastException If the argument is not a char buffer.
   */
  public int compareTo(Object obj)
  {
    CharBuffer a = (CharBuffer) obj;
    
    if (a.remaining () != remaining ())
      return 1;
    
    if (! hasArray () || ! a.hasArray ())
      return 1;
    
    int r = remaining ();
    int i1 = position ();
    int i2 = a.position ();
    
    for (int i = 0; i < r; i++)
      {
        int t = (int) (get (i1)- a.get (i2));
	
        if (t != 0)
          return (int) t;
      }
    return 0;
  }

  /**
   * Retrieves the current endianess of this buffer.
   */
  public abstract ByteOrder order();
  
  /**
   * Relative bulk get method.
   *
   * @exception BufferUnderflowException If there are fewer than length
   * characters remaining in this buffer.
   */
  public abstract char get();
  
  /**
   * Relative bulk get method.
   *
   * @exception BufferOverflowException If this buffer's current position is
   * not smaller than its limit.
   * @exception ReadOnlyBufferException If this buffer is read-only.
   */
  public abstract CharBuffer put(char b);
  
  /**
   * Absolute bulk get method.
   *
   * @exception IndexOutOfBoundsException If index is negative or not smaller
   * than the buffer's limit.
   */
  public abstract char get(int index);

  /**
   * Absolute bulk put method.
   *
   * @exception IndexOutOfBoundsException If index is negative or not smaller
   * than the buffer's limit.
   * @exception ReadOnlyBufferException If this buffer is read-only.
   */
  public abstract CharBuffer put(int index, char b);

  /**
   * Compacts this buffer.
   *
   * @exception ReadOnlyBufferException If this buffer is read-only.
   */
  public abstract CharBuffer compact();

  /**
   * Tells wether or not this is a direct buffer.
   */
  public abstract boolean isDirect();

  /**
   * Creates a new character buffer whose content is a shared subsequence of
   * this buffer's content.
   */
  public abstract CharBuffer slice();

  /**
   * Creates a new character buffer that shares this buffer's content.
   */
  public abstract CharBuffer duplicate();

  /**
   * Creates a new, read-only character buffer that shares this buffer's
   * content.
   */
  public abstract CharBuffer asReadOnlyBuffer();
}
