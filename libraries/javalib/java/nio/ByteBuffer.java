/* ByteBuffer.java -- 
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

import gnu.java.nio.ByteBufferImpl;

/**
 * @since 1.4
 */
public abstract class ByteBuffer extends Buffer
{
  private ByteOrder endian = ByteOrder.BIG_ENDIAN;
  
  protected byte [] backing_buffer;
  
  /**
   * Allocates a new direct byte buffer.
   */ 
  public static ByteBuffer allocateDirect (int capacity)
  {
    ByteBuffer b = new gnu.java.nio. ByteBufferImpl(capacity, 0, capacity);
    return b;
  }

  /**
   * Allocates a new byte buffer.
   */
  public static ByteBuffer allocate (int capacity)
  {
    return new ByteBufferImpl (capacity, 0, capacity);
  }
 
  /**
   * Wraps a byte array into a buffer.
   * 
   * @exception IndexOutOfBoundsException If the preconditions on the offset
   * and length parameters do not hold
   */
  final public static ByteBuffer wrap (byte[] array, int offset, int length)
  {
    return new ByteBufferImpl (array, offset, length);
  }
 
  /**
   * Wraps a byte array into a buffer.
   */
  final public static ByteBuffer wrap (byte[] array)
  {
    return wrap (array, 0, array.length);
  }

  /**
   * This method transfers bytes from this buffer into
   * the given destination array.
   *
   * @param dst The destination array
   * @param offset The offset within the array of the first byte to be written;
   * must be non-negative and no larger than dst.length.
   * @param length The maximum number of bytes to be written to the given array;
   * must be non-negative and no larger than dst.length - offset.
   *
   * @exception BufferUnderflowException If there are fewer than length bytes
   * remaining in this buffer.
   * @exception IndexOutOfBoundsException - If the preconditions on the offset
   * and length parameters do not hold.
   */
  final public ByteBuffer get (byte[] dst, int offset, int length)
  {
    if ((offset < 0) ||
        (offset > dst.length) ||
        (length < 0) ||
        (length > (dst.length - offset)))
      throw new IndexOutOfBoundsException ();

    for (int i = offset; i < offset + length; i++)
      {
        dst [i] = get();
      }

    return this;
  }

  /**
   * This method transfers bytes from this buffer into the given
   * destination array.
   *
   * @param dst The byte array to write into.
   *
   * @exception BufferUnderflowException If there are fewer than dst.length
   * bytes remaining in this buffer.
   */
  final public ByteBuffer get (byte[] dst)
  {
    return get (dst, 0, dst.length);
  }
 
  /**
   * Writes the content of src into the buffer.
   *
   * @param src The source data.
   *
   * @exception BufferOverflowException If there is insufficient space in this
   * buffer for the remaining bytes in the source buffer.
   * @exception IllegalArgumentException If the source buffer is this buffer.
   * @exception ReadOnlyBufferException If this buffer is read only.
   */
  final public ByteBuffer put (ByteBuffer src)
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
   * Writes the content of the the array src into the buffer.
   *
   * @param src The array to copy into the buffer.
   * @param offset The offset within the array of the first byte to be read;
   * must be non-negative and no larger than src.length.
   * @param length The number of bytes to be read from the given array;
   * must be non-negative and no larger than src.length - offset.
   *
   * @exception BufferOverflowException If there is insufficient space in this
   * buffer for the remaining bytes in the source buffer.
   * @exception IndexOutOfBoundsException If the preconditions on the offset
   * and length parameters do not hold.
   * @exception ReadOnlyBufferException If this buffer is read only.
   */
  final public ByteBuffer put (byte[] src, int offset, int length)
  {
    if ((offset < 0) ||
        (offset > src.length) ||
        (length < 0) ||
        (length > src.length - offset))
      throw new IndexOutOfBoundsException ();

    for (int i = offset; i < offset + length; i++)
      {
        put (src [i]);
      }

    return this;
  }

  /**
   * Writes the content of the the array src into the buffer.
   *
   * @param src The array to copy into the buffer.
   *
   * @exception BufferOverflowException If there is insufficient space in this
   * buffer for the remaining bytes in the source buffer.
   * @exception ReadOnlyBufferException If this buffer is read only.
   */
  public final ByteBuffer put (byte[] src)
  {
    return put (src, 0, src.length);
  }

  /**
   * Tells whether or not this buffer is backed by an accessible byte array.
   */
  public final boolean hasArray()
  {
    return (backing_buffer != null);
  }

  /**
   * Returns the byte array that backs this buffer.
   *
   * @exception ReadOnlyBufferException If this buffer is backed by an array
   * but is read-only.
   * @exception UnsupportedOperationException If this buffer is not backed
   * by an accessible array.
   */
  public final byte[] array()
  {
    if (backing_buffer == null)
      throw new UnsupportedOperationException ();

    if (isReadOnly ())
      throw new ReadOnlyBufferException ();

    return backing_buffer;
  }

  /**
   * Returns the offset within this buffer's backing array of the first element
   * of the buffer  
   *
   * @exception ReadOnlyBufferException If this buffer is backed by an array
   * but is read-only.
   * @exception UnsupportedOperationException If this buffer is not backed
   * by an accessible array.
   */
  public final int arrayOffset()
  {
    if (backing_buffer == null)
      throw new UnsupportedOperationException ();

    if (isReadOnly ())
      throw new ReadOnlyBufferException ();

    // FIXME: Return correct value
    return 0;
  }
  
  /**
   * Returns the current hash code of this buffer.
   */
  public int hashCode()
  {
    // FIXME: Check what SUN calcs here
    return super.hashCode();
  }

  /**
   * Tells whether or not this buffer is equal to another object.
   */
  public boolean equals (Object obj)
  {
    if (obj != null &&
        obj instanceof ByteBuffer)
      {
        return compareTo (obj) == 0;
      }
    
    return false;
  }
 
  /**
   * Compares this buffer to another object.
   *
   * @exception ClassCastException If the argument is not a byte buffer
   */
  public int compareTo (Object obj)
  {
    ByteBuffer a = (ByteBuffer) obj;

    if (a.remaining() != remaining())
      {
        return 1;
      }
   
    if (! hasArray() ||
        ! a.hasArray())
      {
        return 1;
      }
   
    int r = remaining();
    int i1 = position ();
    int i2 = a.position ();
   
    for (int i = 0; i < r; i++)
      {
        int t = (int) (get (i1) - a.get (i2));
   
        if (t != 0)
          {
            return (int) t;
          }
      }
  
    return 0;
  }

  /**
   * Retrieves this buffer's byte order.
   */  
  public final ByteOrder order()
  {
    return endian;
  }
  
  /**
   * Modifies this buffer's byte order.
   */
  public final ByteBuffer order (ByteOrder endian)
  {
    this.endian = endian;
    return this;
  }
  
  /**
   * Reads the byte at this buffer's current position,
   * and then increments the position.
   *
   * @exception BufferUnderflowException If the buffer's current position
   * is not smaller than its limit.
   */
  public abstract byte get ();
  
  public abstract ByteBuffer put (byte b);
  
  public abstract byte get (int index);
  
  public abstract ByteBuffer put (int index, byte b);
  
  public abstract ByteBuffer compact();

  public abstract boolean isDirect();
  
  public abstract ByteBuffer slice();
  
  public abstract ByteBuffer duplicate();
  
  public abstract ByteBuffer asReadOnlyBuffer();
  
  public abstract ShortBuffer asShortBuffer();
  
  public abstract CharBuffer asCharBuffer();
  
  public abstract IntBuffer asIntBuffer();
  
  public abstract LongBuffer asLongBuffer();
  
  public abstract FloatBuffer asFloatBuffer();
  
  public abstract DoubleBuffer asDoubleBuffer();
  
  public abstract char getChar();
  
  public abstract ByteBuffer putChar(char value);
  
  public abstract char getChar(int index);
  
  public abstract ByteBuffer putChar(int index, char value);
  
  public abstract short getShort();
  
  public abstract ByteBuffer putShort(short value);
  
  public abstract short getShort(int index);
 
  public abstract ByteBuffer putShort(int index, short value);
  
  public abstract int getInt();
  
  public abstract ByteBuffer putInt(int value);
  
  public abstract int getInt(int index);
  
  public abstract ByteBuffer putInt(int index, int value);
  
  public abstract long getLong();
  
  public abstract ByteBuffer putLong(long value);
  
  public abstract long getLong(int index);
  
  public abstract ByteBuffer putLong(int index, long value);
  
  public abstract float getFloat();
  
  public abstract ByteBuffer putFloat(float value);
  
  public abstract float getFloat(int index);
  
  public abstract ByteBuffer putFloat(int index, float value);
  
  public abstract double getDouble();
  
  public abstract ByteBuffer putDouble(double value);
  
  public abstract double getDouble(int index);
  
  public abstract ByteBuffer putDouble(int index, double value);
}
