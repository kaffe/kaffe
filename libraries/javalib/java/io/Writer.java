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

import java.lang.String;

public abstract class Writer {

  protected Object lock;

  public abstract void write(char cbuf[], int off, int len) throws IOException;
  public abstract void flush() throws IOException;
  public abstract void close() throws IOException;

  protected Writer()
  {
    lock = this;
  }

  protected Writer(Object lk)
  {
    if (lk == null) {
      throw new NullPointerException();
    }
    lock = lk;
  }

  public void write(int c) throws IOException
  {
    synchronized(lock) {
      char[] buf = new char[1];
      buf[0] = (char)c;
      write(buf, 0, 1);
    }
  }

  public void write(char cbuf[]) throws IOException
  {
    synchronized(lock) {
      write(cbuf, 0, cbuf.length);
    }
  }

  public void write(String str) throws IOException
  {
    synchronized(lock) {
      write(str.toCharArray(), 0, str.length());
    }
  }

  public void write(String str, int off, int len) throws IOException
  {
    synchronized(lock) {
      write(str.toCharArray(), off, len);
    }
  }

}
