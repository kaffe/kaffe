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
    count = 0;
    buf = new char[initialSize];
  }

  public void write(int c)
  {
    char buf[] = new char[1];
    buf[0] = (char)c;
    write(buf, 0, 1);
  }

  public void write(char c[], int off, int len)
  {
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
    out.write(buf, 0, count);
  }

  public void reset()
  {
    synchronized(lock) {
      count = 0;
    }
  }

  public char[] toCharArray()
  {
    /* note that the spec asks us to "Return a copy of the input data." */
    final char result[] = new char[count];
    System.arraycopy(buf, 0, result, 0, count);
    return result;
  }

  public int size()
  {
    return (count);
  }

  public String toString()
  {
    return (new String(buf, 0, count));
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
