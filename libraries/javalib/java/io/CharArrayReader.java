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

  protected char buf[];
  protected int pos;
  protected int markedPos;
  protected int count;

  public CharArrayReader(char bf[])
  {
    this(bf, 0, bf.length);
  }

  public CharArrayReader(char bf[], int offset, int length)
  {
    buf = bf;
    pos = offset;
    count = length;
    markedPos = -1;
  }

  public int read() throws IOException
  {
    synchronized(lock) {

      if (count > 0) {
        char ch = buf[pos++];
        count--;
        return ((int)ch);
      }
      else {
       return (-1);
      }

    }
  }

  public int read(char b[], int off, int len) throws IOException
  {
     synchronized(lock) {
       if (count == 0) {
	 return (-1);
       }
       int cnt = (len < count ? len : count);
       System.arraycopy(buf, pos, b, off, cnt);
       pos += cnt;
       count -= cnt;
       return (cnt);
     }
  }

  public long skip(long n) throws IOException
  {
     synchronized(lock) {
       int cnt = (int)(n < (long)count ? n : (long)count);
       pos += cnt;
       count -= cnt;
       return (cnt);
     }
  }

  public boolean ready() throws IOException
  {
    return (true);
  }

  public boolean markSupported()
  {
    return (true);
  }

  public void mark(int readAheadLimit) throws IOException
  {
    markedPos = pos;
  }

  public void reset() throws IOException
  {
    synchronized(lock) {
      count += (pos - markedPos);
      pos = markedPos;
    }
  }

  public void close()
  {
    // Nothing to close as such.
  }

};
