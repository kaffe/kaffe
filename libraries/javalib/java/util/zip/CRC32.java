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

public class CRC32 implements Checksum {

  static {
    System.loadLibrary("zip");
  }

  private int crc;

  public CRC32()
  {
    crc = 0;
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
    crc = 0;
  }

  public long getValue()
  {
    return (((long)crc) & 0xFFFFFFFFL);
  }

  public native void update(byte b[], int off, int len);
  private native void update1(int val);

}
