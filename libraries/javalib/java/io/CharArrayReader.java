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

public class CharArrayReader extends Reader {

  protected char [] buf;
  protected int pos;
  protected int markedPos;
  protected int count;

  public CharArrayReader(char [] bf)
  {
    /* If bf == null, bf.length will throw a
       NullPointerException.
    */
    this(bf, 0, bf.length);
  }

  public CharArrayReader(char [] bf, int offset, int length)
  {
    /* Check if arguments are legal.

       If bf == null, bf.length will throw a
       NullPointerException.

       IBM's JRE 1.3 seems to tolerate offset+length way
       out of bf's range. It just sets count to bf.length.
    */
    if (offset < 0 || offset > bf.length || length < 0) {
      throw new IllegalArgumentException();
    }

    buf       = bf;
    pos       = offset;
    count     = offset + length < bf.length ? offset + length : bf.length;
    markedPos = offset;
  }

  /* Internal function used to check whether the
     CharArrayReader has been closed already. It throws
     an IOException in that case.
  */

  private void checkIfStillOpen() throws IOException {
    /* Since close() sets buf to null, we use that
       as an indicator for a closed stream.
    */
    if (buf == null) {
      throw new IOException("Stream closed");
    }
  }

  public int read() throws IOException
  {
    synchronized(lock) {

      checkIfStillOpen();

      if (pos < count) {
        return (buf[pos++]);
      }
    }

    return (-1);
  }

  public int read(char [] b, int off, int len) throws IOException
  {
     synchronized(lock) {

       checkIfStillOpen();

       // Check if we can read safely.

       if (off < 0 || len < 0 || off + len > b.length) {
	 throw new IndexOutOfBoundsException();
       }

       if (pos < count) {
	   int cnt = Math.min(len, count - pos);
	   System.arraycopy(buf, pos, b, off, cnt);
	   pos += cnt;
	   return (cnt);
       }
     }

     return (-1);

  }

  public long skip(long n) throws IOException
  {
    /* If n < 0, don't throw an exception, as some other
       Readers do. IBM's JRE 1.3 doesn't throw one.
       Act as if n == 0.
    */

     synchronized(lock) {
       checkIfStillOpen();

       int cnt = (int)Math.min(n, (long)(count - pos));

       if (cnt < 0) {
	   cnt = 0;
       }

       pos += cnt;
       return (cnt);
     }
  }

  public boolean ready() throws IOException
  {

    checkIfStillOpen();

    /* A CharArrayReader is ready as long as there
       are any characters left unread.
    */

    if (pos < count) {
      return (true);
    }

    return (false);
  }

  public boolean markSupported()
  {
    return (true);
  }

  public void mark(int readAheadLimit) throws IOException
  {
    synchronized(lock) {
      checkIfStillOpen();

      markedPos = pos;
    }
  }

  public void reset() throws IOException
  {
    synchronized(lock) {
      checkIfStillOpen();

      pos = markedPos;
    }
  }

  public void close()
  {
    /* Nothing to close as such.
       Just release the buffer.
    */
    synchronized (lock) {
      buf = null;
    }
  }

}
