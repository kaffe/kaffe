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

  private static final int DEFAULTBUFFERSIZE = 8192;
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
    super(out);

    if (sz <= 0) {
      throw new IllegalArgumentException("buffer size <= 0");
    }

    size = sz;
    wrbuf = new char[sz];
    wr = out;
  }

/* Internal function used to check whether the
   BufferedWriter has been closed already, throws
   an IOException in that case.
*/
  private void checkIfStillOpen() throws IOException {
    if (wr == null) {
      throw new IOException("Stream closed");
    }
  }

  public void write(int c) throws IOException
  {
    super.write(c);
  }

  public void write(char cbuf[], int off, int len) throws IOException
  {
    if (len < 0 || off < 0 || off + len > cbuf.length) {
      throw new IndexOutOfBoundsException();
    }

    synchronized(lock) {
      checkIfStillOpen();
      if (len > size) {
	flush();
	wr.write(cbuf, off, len);
      }
      else {
	System.arraycopy(cbuf, off, wrbuf, pos, len);
	pos += len;
        size -= len;
        if (size == 0) {
	  flush();
	}
      }
    }
  }

  public void write(String s, int off, int len) throws IOException
  {
    /* small optimization to avoid double argument checking */
    write(s.toCharArray(), off, len);
  }

  public void newLine() throws IOException
  {
    write(newline);
  }

  public void flush() throws IOException
  {
    synchronized(lock) {
      checkIfStillOpen();
      wr.write(wrbuf, 0, pos);
      wr.flush();
      pos = 0;
      size = wrbuf.length;
    }
  }

  public void close() throws IOException
  {
    synchronized(lock) {
      if (wr != null) {
	flush();
	wr.close();
	wr = null;
	wrbuf = null;
      }
    }
  }
}
