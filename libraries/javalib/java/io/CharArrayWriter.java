/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.io;


public class CharArrayWriter extends Writer {

  private static final int DEFAULTBUFFERSIZE = 64;
  private static final int DEFAULTBUFFERSPILL = 64;
  protected char buf[];
  protected int count;

  public CharArrayWriter()
  {
    this(DEFAULTBUFFERSIZE);
  }

  public CharArrayWriter(int initialSize)
  {
    if (initialSize < 0) {
      throw new IllegalArgumentException("Negative initial size : " + initialSize);
    }

    buf = new char[initialSize];
  }

  public void write(int c)
  {
    try {
      super.write(c);
    }
    catch (IOException e) {
      /* can't happen */
    }
  }

  public void write(char c[], int off, int len)
  {
    if (len < 0 || off < 0 || off + len > c.length) {
      throw new IndexOutOfBoundsException();
    }

    synchronized(lock) {

      // Check we have room and if not reallocate.
      if (count + len > buf.length) {
        final char[] oldbuf = buf;
        buf = new char[count + len + DEFAULTBUFFERSPILL];
        System.arraycopy(oldbuf, 0, buf, 0, count);
      }
      System.arraycopy(c, off, buf, count, len);
      count += len;

    }
  }

  public void write(String str, int off, int len)
  {
      write(str.toCharArray(), off, len);
  }

  public void writeTo(Writer out) throws IOException
  {
    synchronized(lock) {
      out.write(buf, 0, count);
    }
  }

  public void reset()
  {
    synchronized(lock) {
      count = 0;
    }
  }

  public char[] toCharArray()
  {
    synchronized(lock) {
      /* note that the spec asks us to "Return a copy of the input data." */
      final char result[] = new char[count];
      System.arraycopy(buf, 0, result, 0, count);
      return result;
    }
  }

  public int size()
  {
    synchronized(lock) {
      return (count);
    }
  }

  public String toString()
  {
    synchronized(lock) {
      return (new String(buf, 0, count));
    }
  }

  public void flush()
  {
    // Does nothing.
  }

  public void close()
  {
    // Does nothing.
  }

}
