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


public abstract class FilterWriter extends Writer {

  protected Writer out;

  protected FilterWriter(Writer o)
  {
    super(o);
    out = o;
  }

/* Internal function used to check whether the
   BufferedWriter has been closed already, throws
   an IOException in that case.
*/
  private void checkIfStillOpen() throws IOException {
    if (out == null) {
      throw new IOException("Stream closed");
    }
  }

  public void write(int c) throws IOException
  {
    synchronized(lock) {
      checkIfStillOpen();
      out.write(c);
    }
  }

  public void write(char cbuf[], int off, int len) throws IOException
  {
    if (len < 0 || off < 0 || off + len > cbuf.length) {
      throw new ArrayIndexOutOfBoundsException();
    }

    synchronized(lock) {
      checkIfStillOpen();
      out.write(cbuf, off, len);
    }
  }

  public void write(String str, int off, int len) throws IOException
  {
    if (len < 0 || off < 0 || off + len > str.length()) {
      throw new ArrayIndexOutOfBoundsException();
    }

    synchronized(lock) {
      checkIfStillOpen();
      out.write(str, off, len);
    }
  }

  public void flush() throws IOException
  {
    synchronized(lock) {
      checkIfStillOpen();
      out.flush();
    }
  }

  public void close() throws IOException
  {
    synchronized(lock) {
      if (out != null) {
	out.close();
	out = null;
      }
    }
  }
}
