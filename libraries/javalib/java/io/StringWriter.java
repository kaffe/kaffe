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


public class StringWriter extends Writer {

  private final static int DEFAULT = 128;
  private StringBuffer buf;

  public StringWriter()
  {
    this(DEFAULT);
  }

  public StringWriter(int initialSize)
  {
    if (initialSize < 0) {
	    throw new IllegalArgumentException("Negative initial size: " + initialSize);
    }

    buf = new StringBuffer(initialSize);
    lock = buf;
  }

  public void write(int c)
  {
    synchronized(lock) {
      buf.append((char)c);
    }
  }

  public void write(char cbuf[], int off, int len)
  {
    if (len < 0 || off < 0 || off + len > cbuf.length) {
      throw new IndexOutOfBoundsException();
    }

    synchronized(lock) {
      buf.append(cbuf, off, len);
    }
  }

  public void write(String str)
  {
    synchronized(lock) {
      buf.append(str);
    }
  }

  public void write(String str, int off, int len)
  {
    write(str.toCharArray(), off, len);
  }

  public String toString()
  {
    synchronized(lock) {
      return (buf.toString());
    }
  }

  public StringBuffer getBuffer()
  {
    synchronized(lock) {
      return (buf);
    }
  }

  public void flush()
  {
    // Does nothing.
  }

  public void close() throws IOException
  {
    // Does nothing.
  }

}
