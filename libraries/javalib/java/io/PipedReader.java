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

public class PipedReader extends Reader {

  private static final int DEFAULT = 1024;

  private PipedWriter wr = null;
  private char buf[] = new char[DEFAULT];
  private int inpos = 0;
  private int outpos = 0;
  private int size = 0;

  public PipedReader()
  {
  }

  public PipedReader(PipedWriter src) throws IOException
  {
    connect(src);
  }

  public void connect(PipedWriter src) throws IOException
  {
    if (wr != null || src.rd != null) {
      throw new IOException("already connected");
    }
    wr = src;
    wr.rd = this;
    lock = (Object)this;
  }

  public int read(char cbuf[], int off, int len) throws IOException
  {
    int i;

    synchronized(lock) {

      while (size == 0) {
        if (wr == null) {
          return (-1);
        }
	try {
          lock.wait();
	}
	catch (InterruptedException _) {
	}
      }

      for (i = 0; i < len && size > 0; i++) {
	cbuf[off++] = buf[outpos++];
	size--;
	if (outpos == buf.length) {
	  outpos = 0;
	}
      }
    }
    return (i);
  }

  void write(char cbuf[], int off, int len) throws IOException
  {
    synchronized(lock) {

      for (int i = 0; i < len; i++) {
        for (;;) {
          if (size < buf.length) {
            break;
          }
          try {
            lock.wait();
          }
          catch (InterruptedException _) {
          }
        }
        buf[inpos++] = cbuf[off++];
        size++;
        if (inpos == buf.length) {
          inpos = 0;
        }
      }

    }
  }

  public void close() throws IOException
  {
    synchronized(lock) {
      if (wr != null) {
	wr.rd = null;
        wr = null;
      }
    }
  }

}
