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

  String name;
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

  /**
   * this constructor is for internal library use only (makeZipEntry) 
   */
  ZipEntry()
  {
  }

  public ZipEntry(String nm)
  {
    if (nm == null) {
      throw new NullPointerException();
    }
    if (nm.length() > 0xFFFF) {
      throw new IllegalArgumentException("name length > 0xFFFF");
    }
    name = nm;
    time = -1;
    crc = 0;
    size = -1;
    method = -1;
    extra = null;
    comment = null;
    csize = -1;
  }

  /**
   * Create a new ZipEntry with fields taken from the specified zip entry.
   */
  public ZipEntry(ZipEntry entry)
  {
    name = entry.name;
    time = entry.time;
    crc  = entry.crc;
    size = entry.size;
    method = entry.method;
    extra = entry.extra;
    comment = entry.comment;
    flag = entry.flag;
    version = entry.version;
    csize = entry.csize;
    offset = entry.offset;
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

  public void setSize(long size)
  {
    if (size < 0 || size > 0xFFFFFFFFL) {
      throw new IllegalArgumentException("size < 0 or size > 0xFFFFFFFF");
    }
    this.size = size;
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

  public void setCompressedSize(long csize)
  {
    if (csize < 0 || csize > 0xFFFFFFFFL) {
	throw new IllegalArgumentException("csize < 0 or csize > 0xFFFFFFFF");
    }

    this.csize = csize;
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
