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

  private static final int DEFAULT_PUSHBACK_BUFFER_SIZE = 1;

  private PushbackBuffer pbuf;

  public PushbackReader(Reader i, int size)
  {
    super(i);

    if (size <= 0) {
      throw new IllegalArgumentException("size <= 0");
    }

    pbuf = new PushbackBuffer(size);
  }

  public PushbackReader(Reader i)
  {
    this(i, DEFAULT_PUSHBACK_BUFFER_SIZE);
  }

  private void checkIfStillOpen() throws IOException 
  {
    if (pbuf == null) {
      throw new IOException("Stream closed");
    }
  }

  public void close() throws IOException
  {
    synchronized(lock) {
      /* Release pushback buffer. */
      pbuf = null;
    }

    super.close();
  }

  public void mark(int readAheadLimit) throws IOException 
  {
    throwMarkException();
  }

  public boolean markSupported()
  {
    return (false);
  }

  public int read() throws IOException
  {
    synchronized(lock) {

      checkIfStillOpen();

      if (pbuf.isEmpty()) {
	return (super.read());
      }
      return ((int) pbuf.read());
    }
  }

  public int read(char [] cbuf, int off, int len) throws IOException
  {
    int cnt = 0;
    int r   = 0;

    if (!validArguments(cbuf, off, len)) {
      throw new IndexOutOfBoundsException();
    }

    synchronized(lock) {

      checkIfStillOpen();

      /* Read from pushback buffer first.
       * If it is empty, it will return 0.
       */
      cnt += pbuf.read(cbuf, off, len);
      len -= cnt;
      off += cnt;

      /* If we need to read more characters,
       * get them from the underlying reader.
       */
      if (len > 0) {
        r = super.read(cbuf, off, len);
      }
    }

    /* Calculate return code. If pushback buffer was
     * empty, and the underlying reader returned -1,
     * we need to return -1. Otherwise, we return the
     * total number of characters read.
     */
    if (r == -1) {
      if (cnt == 0) {
	cnt = -1;
      }
    }
    else {
      cnt += r;
    }
    
    return (cnt);
  }

  public boolean ready() throws IOException 
  {
    synchronized(lock) {

      checkIfStillOpen();

      return (!pbuf.isEmpty() || super.ready());
    }
  }

  public void reset() throws IOException 
  {
    throwMarkException();
  }

  private static void throwMarkException() throws IOException
  {
    throw new IOException("mark/reset not supported");
  }

  public void unread(int c) throws IOException
  {
    synchronized(lock) {

      checkIfStillOpen();

      pbuf.unread((char) c);
    }
  }

  public void unread(char [] cbuf, int off, int len) throws IOException
  {
    if (!validArguments(cbuf, off, len)) {
      throw new ArrayIndexOutOfBoundsException();
    }

    synchronized(lock) {

      checkIfStillOpen();

      pbuf.unread(cbuf, off, len);
    }
  }

  public void unread(char [] cbuf) throws IOException
  {
    unread(cbuf, 0, cbuf.length);
  }

  private static boolean validArguments(char [] cbuf, int off, int len) 
  {
    if (0 <= off && off < cbuf.length && 0 <= len && len <= cbuf.length - off) {
      return true;
    }

    return false;
  }

  private class PushbackBuffer {

    private char[] buf;
    private int pos;

    private PushbackBuffer(int size) {
      buf = new char[size];
      pos = buf.length;
    }

    private int capacity() {
      return pos;
    }

    private boolean isEmpty() {
      return (capacity() == buf.length);
    }

    private boolean isFull() {
      return (capacity() == 0);
    }

    /* length() returns the number of pushed-back
     * characters left to read.
     */
    private int length() {
      return (buf.length - capacity());
    }

    private char read() {
      return (buf[pos++]);
    }

    private int read(char [] cbuf, int off, int len) {
      len = Math.min(len, length());
      System.arraycopy(buf, pos, cbuf, off, len);
      pos += len;
      return len;
    }

    private void throwOverflowException() throws IOException {
      throw new IOException("Pushback buffer overflow");
    }

    private void unread(char c) throws IOException {
      if (this.isFull()) {
	throwOverflowException();
      }

      buf[--pos] = c;
    }

    private void unread(char [] cbuf, int off, int len) throws IOException {
      if (len > capacity()) {
	throwOverflowException();
      }

      pos -= len;
      System.arraycopy(cbuf, off, buf, pos, len);
    } 
  }
}

