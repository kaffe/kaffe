/* MappedCharFileBuffer.java -- 
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

import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.CharBuffer;
import java.nio.DoubleBuffer;
import java.nio.FloatBuffer;
import java.nio.IntBuffer;
import java.nio.LongBuffer;
import java.nio.ShortBuffer;
import gnu.classpath.RawData;

final public class MappedCharFileBuffer
  extends CharBuffer
{
  public RawData map_address;
  boolean ro;
  boolean direct;
  public FileChannelImpl ch;
  private ByteOrder endian = ByteOrder.BIG_ENDIAN;

  public MappedCharFileBuffer(FileChannelImpl ch)
  {
    // FIXME
    super (0, 0, 0, 0);
    this.ch = ch;
    map_address = ch.map_address;
  }

  public MappedCharFileBuffer(MappedCharFileBuffer b)
  {
    // FIXME
    super (0, 0, 0, 0);
    this.ro = b.ro;
    this.ch = b.ch;
    map_address = b.map_address;
  }

  final public ByteOrder order ()
  {
    return endian;
  }

  public boolean isReadOnly()
  {
    return ro;
  }
  
  final public char get()
  {
    char a = MappedByteFileBuffer.nio_read_Char_file_channel(ch, position(), limit(), map_address);
    position(position() + 2);
    return a;
  }

  final public CharBuffer put(char b)
  {
    MappedByteFileBuffer.nio_write_Char_file_channel(ch, position(), limit(), b, map_address);
    position(position() + 2);
    return this;
  }

  final public char get(int index)
  {
    char a = MappedByteFileBuffer.nio_read_Char_file_channel(ch, index, limit(), map_address);
    return a;
  }

  final public CharBuffer put(int index, char b)
  {
    MappedByteFileBuffer.nio_write_Char_file_channel(ch, index, limit(), b, map_address);
    return this;
  }

  final public CharBuffer compact()
  {
    return this;
  }

  final public boolean isDirect()
  {
    return direct;
  }

  final public CharSequence subSequence (int start, int end)
  {
    // FIXME
    return null;
  }

  final public CharBuffer slice()
  {
    MappedCharFileBuffer A = new MappedCharFileBuffer(this);
    return A;
  }

  public CharBuffer duplicate()
  {
    return new MappedCharFileBuffer(this);
  }

  public CharBuffer asReadOnlyBuffer()
  {
    MappedCharFileBuffer b = new MappedCharFileBuffer(this);
    b.ro = true;
    return b;
  }

  final public ByteBuffer asByteBuffer() { ByteBuffer res = new MappedByteFileBuffer(ch); res.limit((limit()*2)/1); return res; } final public byte getByte() { byte a = MappedByteFileBuffer.nio_read_Byte_file_channel(ch, position(), limit(), map_address); position(position() + 2); return a; } final public CharBuffer putByte(byte value) { MappedByteFileBuffer.nio_write_Byte_file_channel(ch, position(), limit(), value, map_address); position(position() + 2); return this; } final public byte getByte(int index) { byte a = MappedByteFileBuffer.nio_read_Byte_file_channel(ch, index, limit(), map_address); return a; } final public CharBuffer putByte(int index, byte value) { MappedByteFileBuffer.nio_write_Byte_file_channel(ch, index, limit(), value, map_address); return this; };
  final public CharBuffer asCharBuffer() { CharBuffer res = new MappedCharFileBuffer(ch); res.limit((limit()*2)/2); return res; } final public char getChar() { char a = MappedByteFileBuffer.nio_read_Char_file_channel(ch, position(), limit(), map_address); position(position() + 2); return a; } final public CharBuffer putChar(char value) { MappedByteFileBuffer.nio_write_Char_file_channel(ch, position(), limit(), value, map_address); position(position() + 2); return this; } final public char getChar(int index) { char a = MappedByteFileBuffer.nio_read_Char_file_channel(ch, index, limit(), map_address); return a; } final public CharBuffer putChar(int index, char value) { MappedByteFileBuffer.nio_write_Char_file_channel(ch, index, limit(), value, map_address); return this; };
  final public ShortBuffer asShortBuffer() { ShortBuffer res = new MappedShortFileBuffer(ch); res.limit((limit()*2)/2); return res; } final public short getShort() { short a = MappedByteFileBuffer.nio_read_Short_file_channel(ch, position(), limit(), map_address); position(position() + 2); return a; } final public CharBuffer putShort(short value) { MappedByteFileBuffer.nio_write_Short_file_channel(ch, position(), limit(), value, map_address); position(position() + 2); return this; } final public short getShort(int index) { short a = MappedByteFileBuffer.nio_read_Short_file_channel(ch, index, limit(), map_address); return a; } final public CharBuffer putShort(int index, short value) { MappedByteFileBuffer.nio_write_Short_file_channel(ch, index, limit(), value, map_address); return this; };
  final public IntBuffer asIntBuffer() { IntBuffer res = new MappedIntFileBuffer(ch); res.limit((limit()*2)/4); return res; } final public int getInt() { int a = MappedByteFileBuffer.nio_read_Int_file_channel(ch, position(), limit(), map_address); position(position() + 2); return a; } final public CharBuffer putInt(int value) { MappedByteFileBuffer.nio_write_Int_file_channel(ch, position(), limit(), value, map_address); position(position() + 2); return this; } final public int getInt(int index) { int a = MappedByteFileBuffer.nio_read_Int_file_channel(ch, index, limit(), map_address); return a; } final public CharBuffer putInt(int index, int value) { MappedByteFileBuffer.nio_write_Int_file_channel(ch, index, limit(), value, map_address); return this; };
  final public LongBuffer asLongBuffer() { LongBuffer res = new MappedLongFileBuffer(ch); res.limit((limit()*2)/8); return res; } final public long getLong() { long a = MappedByteFileBuffer.nio_read_Long_file_channel(ch, position(), limit(), map_address); position(position() + 2); return a; } final public CharBuffer putLong(long value) { MappedByteFileBuffer.nio_write_Long_file_channel(ch, position(), limit(), value, map_address); position(position() + 2); return this; } final public long getLong(int index) { long a = MappedByteFileBuffer.nio_read_Long_file_channel(ch, index, limit(), map_address); return a; } final public CharBuffer putLong(int index, long value) { MappedByteFileBuffer.nio_write_Long_file_channel(ch, index, limit(), value, map_address); return this; };
  final public FloatBuffer asFloatBuffer() { FloatBuffer res = new MappedFloatFileBuffer(ch); res.limit((limit()*2)/4); return res; } final public float getFloat() { float a = MappedByteFileBuffer.nio_read_Float_file_channel(ch, position(), limit(), map_address); position(position() + 2); return a; } final public CharBuffer putFloat(float value) { MappedByteFileBuffer.nio_write_Float_file_channel(ch, position(), limit(), value, map_address); position(position() + 2); return this; } final public float getFloat(int index) { float a = MappedByteFileBuffer.nio_read_Float_file_channel(ch, index, limit(), map_address); return a; } final public CharBuffer putFloat(int index, float value) { MappedByteFileBuffer.nio_write_Float_file_channel(ch, index, limit(), value, map_address); return this; };
  final public DoubleBuffer asDoubleBuffer() { DoubleBuffer res = new MappedDoubleFileBuffer(ch); res.limit((limit()*2)/8); return res; } final public double getDouble() { double a = MappedByteFileBuffer.nio_read_Double_file_channel(ch, position(), limit(), map_address); position(position() + 2); return a; } final public CharBuffer putDouble(double value) { MappedByteFileBuffer.nio_write_Double_file_channel(ch, position(), limit(), value, map_address); position(position() + 2); return this; } final public double getDouble(int index) { double a = MappedByteFileBuffer.nio_read_Double_file_channel(ch, index, limit(), map_address); return a; } final public CharBuffer putDouble(int index, double value) { MappedByteFileBuffer.nio_write_Double_file_channel(ch, index, limit(), value, map_address); return this; };
}
