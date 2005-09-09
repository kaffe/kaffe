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

public class Adler32 implements Checksum {

  static {
    System.loadLibrary("zip");
  }

  private int adler;

  public Adler32()
  {
    adler = 1;
  }

  public void update(int b)
  {
    update1(b);
  }

  public void update(byte b[])
  {
    update(b, 0, b.length);
  }

  public void reset()
  {
    adler = 1;
  }

  public long getValue()
  {
    return (adler & 0xFFFFFFFFL);
  }

  public native void update(byte[] buf, int from, int len);
  private native void update1(int val);

}
