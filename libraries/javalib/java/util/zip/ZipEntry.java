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

public class ZipEntry implements ZipConstants {

  public static final int STORED = Deflater.NO_COMPRESSION;
  public static final int DEFLATED = Deflater.DEFLATED;

  private String name;
  long time;
  long crc;
  long size;
  int method;
  byte[] extra;
  String comment;
  int flag;
  int version;
  long csize;
  long offset;

  public ZipEntry(String nm)
  {
    if (nm == null) {
      throw new NullPointerException();
    }
    if (nm.length() > 0xFFFF) {
      throw new IllegalArgumentException("name length > 0xFFFF");
    }
    name = nm;
    time = 0;
    crc = 0;
    size = -1;
    method = -1;
    extra = null;
    comment = null;
    csize = -1;
  }

  public String getName()
  {
    return (name);
  }

  public void setTime(long tm)
  {
    time = tm;
  }

  public long getTime()
  {
    return (time);
  }

  public void setSize(long sz)
  {
    if (size > 0xFFFFFFFF) {
      throw new IllegalArgumentException("size > 0xFFFFFFFF");
    }
    size = sz;
  }

  public long getSize()
  {
    return (size);
  }

  public void setCrc(long crcx)
  {
    if (crcx < 0 || crcx > 0xFFFFFFFFL) {
      throw new IllegalArgumentException("crc < 0 or > 0xFFFFFFFF: " + crcx);
    }
    crc = crcx;
  }

  public long getCrc()
  {
    return (crc);
  }

  public void setMethod(int meth)
  {
    if (meth != STORED && meth != DEFLATED) {
      throw new IllegalArgumentException("unsupported compression method");
    }
    method = meth;
  }

  public int getMethod()
  {
    return (method);
  }

  public void setExtra(byte xtra[])
  {
    if (xtra.length > 0xFFFFF) {
      throw new IllegalArgumentException("extra length > 0xFFFFF");
    }
    extra = xtra;
  }

  public byte[] getExtra()
  {
    return (extra);
  }

  public void setComment(String commnt)
  {
    if (commnt.length() > 0xFFFF) {
      throw new IllegalArgumentException("comment length > 0xFFFF");
    }
    comment = commnt;
  }

  public String getComment()
  {
    return (comment);
  }

  public long getCompressedSize()
  {
    return (csize);
  }

  public boolean isDirectory()
  {
    return (name.endsWith("/"));
  }

  public String toString()
  {
    return (getName());
  }

}
