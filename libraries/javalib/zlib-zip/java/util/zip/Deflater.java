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

public class Deflater {

  static {
    System.loadLibrary("zip");
  }

  public static final int DEFLATED = 8;
  public static final int NO_COMPRESSION = 0;
  public static final int BEST_SPEED = 1;
  public static final int BEST_COMPRESSION = 9;
  public static final int DEFAULT_COMPRESSION = -1;
  public static final int FILTERED = 1;
  public static final int HUFFMAN_ONLY = 2;
  public static final int DEFAULT_STRATEGY = 0;

  private int strm;	// The number of the deflater stream.
  private byte[] buf;
  private int off;
  private int len;
  private int level;
  private int strategy;
  private boolean finish;
  private boolean finished;

  public Deflater(int lvl, boolean nowrap)
  {
    level = lvl;
    buf = null;
    len = 0;
    off = 0;
    strategy = DEFLATED;
    finish = false;
    finished = false;
    init(nowrap);
  }

  public Deflater(int level)
  {
    this(level, false);
  }

  public Deflater()
  {
    this(DEFAULT_COMPRESSION, false);
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

  public synchronized void setStrategy(int stgy)
  {
    if (stgy != DEFLATED) {
      throw new IllegalArgumentException("only support deflation");
    }
    strategy = stgy;
  }

  public synchronized void setLevel(int lvl)
  {
    if ((level != DEFAULT_COMPRESSION) && (lvl < 0 || lvl > 9)) {
      throw new IllegalArgumentException("levels 0-9 supported");
    }
    level = lvl;
  }

  public boolean needsInput()
  {
    return (len == 0 ? true : false);
  }

  public synchronized void finish()
  {
    finish = true;
  }

  public synchronized boolean finished()
  {
    return (finished);
  }

  public int deflate(byte b[])
  {
    return (deflate(b, 0, b.length));
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
  public native synchronized int deflate(byte b[], int off, int len);
  public native synchronized int getAdler();
  public native synchronized int getTotalIn();
  public native synchronized int getTotalOut();
  public native synchronized void reset();
  public native synchronized void end();
  private native synchronized void init(boolean nowrap);

}
