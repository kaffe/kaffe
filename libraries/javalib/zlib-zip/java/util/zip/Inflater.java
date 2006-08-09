/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util.zip;

public class Inflater {

  static {
    System.loadLibrary("zip");
  }

  private int strm;
  protected byte[] buf;
  protected int off;
  protected int len;
  private boolean finished;
  private boolean needsDictionary;

  public Inflater(boolean nowrap)
  {
    init(nowrap);
  }

  public Inflater()
  {
    this(false);
  }

  public synchronized void setInput(byte b[], int o, int l)
  {
    buf = b;
    off = o;
    len = l;
  }

  public void setInput(byte b[])
  {
    setInput(b, 0, b.length);
  }

  public void setDictionary(byte b[])
  {
    setDictionary(b, 0, b.length);
  }

  public synchronized int getRemaining()
  {
    return (len);
  }

  public synchronized boolean needsInput()
  {
    return (len == 0 && !finished());
  }

  public synchronized boolean needsDictionary()
  {
    return (needsDictionary);
  }

  public synchronized boolean finished()
  {
    return (finished);
  }

  public int inflate(byte b[]) throws DataFormatException
  {
    try {
      return (inflate(b, 0, b.length));
    }
    catch (NullPointerException _) {
      throw new DataFormatException("null buffer");
    }
  }

  public synchronized int inflate(byte b[], int off, int length) throws DataFormatException {
    if (getRemaining() == 0) {
      return 0;
    }

    return inflate0(b, off, length);
  }

  protected void finalize() throws Throwable
  {
    /* We don't need to call super.finalize(),
     * since super class is java.lang.Object, and
     * java.lang.Object.finalize() just returns
     * to caller.
     */

    end();
  }

  public native synchronized void setDictionary(byte b[], int off, int len);
  private native synchronized int inflate0(byte b[], int off, int len) throws DataFormatException;
  public native synchronized int getAdler();
  public native synchronized int getTotalIn();
  public native synchronized int getTotalOut();
  public native synchronized void reset();
  private native synchronized void init(boolean nowrap);
  public native synchronized void end();

}
