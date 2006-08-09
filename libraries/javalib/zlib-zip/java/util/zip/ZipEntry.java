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

import java.util.Calendar;
import java.util.Date;
import java.util.GregorianCalendar;

public class ZipEntry implements Cloneable, ZipConstants {

    /* Clone method taken from GNU Classpath:
     * Copyright (C) 2001, 2002, 2004, 2005 Free Software Foundation, Inc.
     */
  /**
   * Creates a copy of this zip entry.
   */
  /**
   * Clones the entry.
   */
  public Object clone()
  {
    try
      {
	// The JCL says that the `extra' field is also copied.
	ZipEntry clone = (ZipEntry) super.clone();
	if (extra != null)
	  clone.extra = (byte[]) extra.clone();
	return clone;
      }
    catch (CloneNotSupportedException ex)
      {
	throw new InternalError();
      }
  }

  public static final int STORED = Deflater.NO_COMPRESSION;
  public static final int DEFLATED = Deflater.DEFLATED;

  private String name;
  private long time = -1;
  int dosTime;
  private long crc;
  private long size;
  private int method;
  private byte[] extra;
  private String comment;
  int flag;
  int version;
  private long csize;
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
    time = new Date().getTime();
    dosTime = computeDosTime(time);
    crc = -1;
    size = -1;
    method = -1;
    extra = null;
    comment = null;
    flag = 0;
    version = 0;
    csize = -1;
    offset = 0;
  }

  /**
   * Create a new ZipEntry with fields taken from the specified zip entry.
   */
  public ZipEntry(ZipEntry entry)
  {
    name = entry.name;
    time = entry.time;
    dosTime = entry.dosTime;
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
    dosTime = computeDosTime(tm);
  }

  public long getTime()
  {
    if (time == -1 && dosTime != 0)
      setDosTime(dosTime);
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

  public void setExtra(byte extra[])
  {
    if ((extra != null) && (extra.length > 0xFFFFF)) {
      throw new IllegalArgumentException("extra length > 0xFFFFF");
    }
    this.extra = extra;
  }

  public byte[] getExtra()
  {
    return (extra);
  }

  public void setComment(String comment)
  {
    if ((comment != null) && (comment.length() > 0xFFFF)) {
      throw new IllegalArgumentException("comment length > 0xFFFF");
    }
    this.comment = comment;
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

  // Encode timestamp in DOS format
  int computeDosTime(long time) {
    time = (time + 1) & ~1L; // Round up to even seconds.
    Calendar cal = new GregorianCalendar();
    cal.setTime(new Date(time));
    return encodeDosTime(
	cal.get(Calendar.YEAR), cal.get(Calendar.MONTH) + 1,
	cal.get(Calendar.DAY_OF_MONTH), cal.get(Calendar.HOUR_OF_DAY),
	cal.get(Calendar.MINUTE), cal.get(Calendar.SECOND));
  }

  int encodeDosTime(int year, int month, int day, int hour,
      int minute, int second) {
    return (year < 1980) ? encodeDosTime(1980, 1, 1, 0, 0, 0) :
      ((year - 1980) << 25) | (month << 21) | (day << 16) |
      (hour << 11) | (minute << 5) | (second >>> 1);
  }

  void setDosTime(int date) {
      Calendar cal = new GregorianCalendar();
      cal.clear();
      cal.set (((date >> 25) & 0x7F) + 1980,
	       ((date >> 21) & 0xF) - 1,
	       (date >> 16) & 0x1F,
	       (date >> 11) & 0x1F,
	       (date >> 5) & 0x3F,
	       (date << 1) & 0x3E); 
      time = cal.getTime().getTime();
      dosTime = date;
  }

}
