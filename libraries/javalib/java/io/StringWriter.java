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
    buf = new StringBuffer(initialSize);
  }

  public void write(int c)
  {
    buf.append((char)c);
  }

  public void write(char cbuf[], int off, int len)
  {
    buf.append(cbuf, off, len);
  }

  public void write(String str)
  {
    buf.append(str);
  }

  public void write(String str, int off, int len)
  {
    write(str.toCharArray(), off, len);
  }

  public String toString()
  {
    return (buf.toString());
  }

  public StringBuffer getBuffer()
  {
    return (buf);
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
