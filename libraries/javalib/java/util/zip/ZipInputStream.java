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

import java.io.InputStream;
import java.io.IOException;

public class ZipInputStream extends InflaterInputStream implements ZipConstants {

  private byte zheader[] = new byte[LOC_RECSZ];
  private InflaterInputStream inflater;

  public ZipInputStream(InputStream in)
  {
    super(in);
  }

  public ZipEntry getNextEntry() throws IOException
  {
    synchronized (this) {

      super.read(zheader);
  
      long sig = get32(LOC_SIGNATURE);
      if (sig != LOC_HEADSIG) {
        if (sig == CEN_HEADSIG) {
  	return (null);
        }
        throw new IOException("LOC header signature bad");
      }
  
      char[] name = new char[get16(LOC_FILENAMELEN)];
      for (int i = 0; i < name.length; i++) {
        name[i] = (char)super.read();	// So much for Unicode ...!
      }
  
      byte[] extra = new byte[get16(LOC_EXTRAFIELDLEN)];
      for (int i = 0; i < extra.length; i++) {
        extra[i] = (byte)super.read();
      }
  
      ZipEntry entry = new ZipEntry(new String(name));
      entry.time = 0;
      entry.crc = get32(LOC_CRC);
      entry.size = (int)get32(LOC_UNCOMPRESSEDSIZE);
      entry.method = get16(LOC_METHOD);
      entry.extra = extra;
      entry.comment = "";
      entry.flag = (int)get32(LOC_FLAGS);
      entry.version = get16(LOC_VERSIONEXTRACT);
      entry.csize = (int)get32(LOC_COMPRESSEDSIZE);
      entry.offset = 0;

      // Create inflater to read data.
      inflater = new InflaterInputStream(in);
  
      return (entry);
    }
  }

  public void closeEntry() throws IOException
  {
    if (inflater != null) {
      inflater.close();
      inflater = null;
    }
  }

  public int read(byte b[], int off, int len) throws IOException
  {
    synchronized (this) {
      return (inflater.read(b, off, len));
    }
  }

  public long skip(long n) throws IOException
  {
    synchronized (this) {
      return (inflater.skip(n));
    }
  }

  public void close() throws IOException
  {
    closeEntry();
    super.close();
  }

  // -------------------------------------------------------------------------------

  private int get16(int base)
  {
    int val = ((int)zheader[base+0] << 0) | ((int)zheader[base+1] << 8);
    return (val);
  }

  private long get32(int base)
  {
    long val = ((long)zheader[base+0] << 0) | ((long)zheader[base+1] << 8) | ((long)zheader[base+2] << 16) | ((long)zheader[base+3] << 24);
    return (val);
  }
}
