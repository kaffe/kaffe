/* ShortBufferImpl.java -- 
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

package gnu.java.nio;

import java.nio.ByteBuffer;
import java.nio.ShortBuffer;

public final class ShortBufferImpl extends ShortBuffer
{
  private int array_offset;
  private boolean ro;

  public ShortBufferImpl(int cap, int off, int lim)
  {
    this.backing_buffer = new short[cap];
    this.cap = cap ;
    this.limit(lim);
    this.position(off);
  }

  public ShortBufferImpl(short[] array, int off, int lim)
  {
    this.backing_buffer = array;
    this.cap = array.length;
    this.limit(lim);
    this.position(off);
  }

  public ShortBufferImpl(ShortBufferImpl copy)
  {
    backing_buffer = copy.backing_buffer;
    ro = copy.ro;
    limit(copy.limit());
    position(copy.position());
  }

  void inc_pos(int a)
  {
    position(position() + a);
  }

  private static native short[] nio_cast (byte[] copy);

  ShortBufferImpl (byte[] copy)
  {
    this.backing_buffer = copy != null ? nio_cast (copy) : null;
  }
  
  private static native byte nio_get_Byte (ShortBufferImpl b, int index, int limit);
  
  private static native void nio_put_Byte (ShortBufferImpl b, int index, int limit, byte value);
  
  public ByteBuffer asByteBuffer ()
  {
    ByteBufferImpl res = new ByteBufferImpl (backing_buffer);
    res.limit ((limit () * 1) / 2);
    return res;
  }

  public boolean isReadOnly()
  {
    return ro;
  }

  public ShortBuffer slice()
  {
    ShortBufferImpl a = new ShortBufferImpl(this);
    a.array_offset = position();
    return a;
  }

  public ShortBuffer duplicate()
  {
    return new ShortBufferImpl(this);
  }

  public ShortBuffer asReadOnlyBuffer()
  {
    ShortBufferImpl a = new ShortBufferImpl(this);
    a.ro = true;
    return a;
  }

  public ShortBuffer compact()
  {
    return this;
  }

  public boolean isDirect()
  {
    return backing_buffer != null;
  }

  final public short get()
  {
    short e = backing_buffer[position()];
    position(position()+1);
    return e;
  }

  final public ShortBuffer put(short b)
  {
    backing_buffer[position()] = b;
    position(position()+1);
    return this;
  }

  final public short get(int index)
  {
    return backing_buffer[index];
  }

  final public ShortBuffer put(int index, short b)
  {
    backing_buffer[index] = b;
    return this;
  }
}
