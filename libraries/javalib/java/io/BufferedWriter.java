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

public class BufferedWriter extends Writer {

  private static final int DEFAULTBUFFERSIZE = 1024;
  private static final String newline = System.getProperty("line.separator");
  private Writer wr;
  private char[] wrbuf;
  private int pos;
  private int size;

  public BufferedWriter(Writer out)
  {
    this(out, DEFAULTBUFFERSIZE);
  }

  public BufferedWriter(Writer out, int sz)
  {
    wr = out;
    wrbuf = new char[sz];
    pos = 0;
    size = sz;
  }

  public void write(int c) throws IOException
  {
    char[] buf = new char[1];
    buf[0] = (char)c;
    write(buf, 0, 1);
  }

  public void write(char cbuf[], int off, int len) throws IOException
  {
    synchronized(lock) {

      for (;;) {
        int i;
        for (i = 0; i < size && i < len; i++) {
          wrbuf[pos++] = cbuf[off++];
        }
        size -= i;
        if (i == len) {
          return;
        }
	len -= i;
        flush();
      }

    }
  }

  public void write(String s, int off, int len) throws IOException
  {
    write(s.toCharArray(), off, len);
  }

  public void newLine() throws IOException
  {
    write(newline);
  }

  public void flush() throws IOException
  {
    synchronized(lock) {
      wr.write(wrbuf, 0, pos);
      wr.flush();
      pos = 0;
      size = wrbuf.length;
    }
  }

  public void close() throws IOException
  {
    flush();
    wr.close();
  }

};
