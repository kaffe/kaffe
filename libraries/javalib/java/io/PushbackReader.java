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

public class PushbackReader extends FilterReader {

  private char[] buf;
  private int pos;

  public PushbackReader(Reader i, int size)
  {
    super(i);
    buf = new char[size];
    pos = buf.length;
  }

  public PushbackReader(Reader i)
  {
    this(i, 1);
  }

  public int read() throws IOException
  {
    synchronized(lock) {

      if (pos < buf.length) {
        return ((int)buf[pos++]);
      }
      return (super.read());

    }
  }

  public int read(char cbuf[], int off, int len) throws IOException
  {
    synchronized(lock) {

      int cnt = 0;
      while (pos < buf.length && len > 0) {
        cbuf[off++] = buf[pos++];
        len--;
        cnt++;
      }
      if (len > 0) {
        int r = super.read(cbuf, off, len);
        if (r == -1) {
	  if (cnt == 0) {
	    cnt = -1;
	  }
        }
        else {
	  cnt += r;
        }
      }
      return (cnt);

    }
  }

  public void unread(int c) throws IOException
  {
    synchronized(lock) {

      if (pos == 0) {
	throw new IOException("pushback buffer is full");
      }
      pos--;
      buf[pos] = (char)c;
    }
  }

  public void unread(char cbuf[], int off, int len) throws IOException
  {
    synchronized(lock) {

      if (pos < len) {
	throw new IOException("pushback buffer is too full");
      }
      pos -= len;
      System.arraycopy(cbuf, off, buf, pos, len);

    }
  }

  public void unread(char cbuf[]) throws IOException
  {
    unread(cbuf, 0, cbuf.length);
  }

  public boolean ready() throws IOException
  {
    return (pos < buf.length ? true : super.ready());
  }

  public boolean markSupported()
  {
    return (false);
  }

  public void close() throws IOException
  {
    super.close();
  }

}
