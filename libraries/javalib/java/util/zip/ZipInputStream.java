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

import java.lang.Integer;
import java.io.InputStream;
import java.io.IOException;
import kaffe.util.zip.SwitchInflater;

public class ZipInputStream extends InflaterInputStream implements ZipConstants {

  private byte sigbuf[] = new byte[4];
  private byte zheader[] = new byte[LOC_RECSZ];
  private boolean closed;
  private SwitchInflater sinf;

  public ZipInputStream(InputStream in) {
    super(in, new SwitchInflater(true, true));
    closed = false;
    sinf = (SwitchInflater)inf;
  }

  public ZipEntry getNextEntry() throws IOException {

    closeEntry();

    synchronized (this) {

      for (;;) {
	if (readFully(sigbuf, 0, sigbuf.length) != sigbuf.length) {
	  throw new IOException("signature not found");
	}
	long sig = get32(sigbuf, 0);

	if (sig == CEN_HEADSIG) {
	  return (null);
	}
	else if (sig == DATA_HEADSIG) {
	  skip(DATA_RECSZ - sigbuf.length);
	  continue;
	}
	else if (sig == LOC_HEADSIG) {
	  if (readFully(zheader, sigbuf.length, zheader.length - sigbuf.length) == zheader.length - sigbuf.length) {
	    break;
	  }
	}
        throw new IOException("LOC header signature bad: " + Long.toHexString(sig));
      }
  
      char[] name = new char[get16(zheader, LOC_FILENAMELEN)];
      for (int i = 0; i < name.length; i++) {
        name[i] = (char)read();	// So much for Unicode ...!
      }
  
      byte[] extra = new byte[get16(zheader, LOC_EXTRAFIELDLEN)];
      readFully(extra, 0, extra.length);
  
      ZipEntry entry = new ZipEntry(new String(name));
      entry.time = -1;
      entry.crc = get32(zheader, LOC_CRC);
      entry.size = (int)get32(zheader, LOC_UNCOMPRESSEDSIZE);
      entry.method = get16(zheader, LOC_METHOD);
      entry.extra = extra;
      entry.comment = "";
      entry.flag = (int)get16(zheader, LOC_FLAGS);
      entry.version = get16(zheader, LOC_VERSIONEXTRACT);
      entry.csize = (int)get32(zheader, LOC_COMPRESSEDSIZE);
      entry.offset = 0;

      // Select the loader, simple or inflater.
      if (entry.method == ZipConstants.STORED) {
	sinf.setMode(true);
	sinf.setLength((int)entry.csize);
      }
      else {
	sinf.setMode(false);
      }

// System.out.println("ZipEntry: " + entry + ", meth=" + entry.method + ", size=" + entry.size + ", csize=" + entry.csize);

      closed = false;

      return (entry);
    }
  }

  public void closeEntry() throws IOException {
	if (closed == false) {
		closed = true;
		sinf.reset();
		sinf.setMode(true);
		sinf.setLength(Integer.MAX_VALUE);
	}
  }

  public int read(byte b[], int off, int len) throws IOException {
    int r;
    r = super.read(b, off, len);
    return r;
  }

  public long skip(long n) throws IOException {
    return (super.skip(n));
  }

  public void close() throws IOException {
    closeEntry();
    super.close();
  }

  private int get16(byte[] buf, int base) {
    int val = (int)buf[base] & 0xFF;
    val |= ((int)buf[base+1] & 0xFF) << 8;
    return (val);
  }

  private long get32(byte[] buf, int base) {
    long val = (long)buf[base] & 0xFF;
    val |= ((long)buf[base+1] & 0xFF) << 8;
    val |= ((long)buf[base+2] & 0xFF) << 16;
    val |= ((long)buf[base+3] & 0xFF) << 24;
    return (val);
  }

  private int readFully(byte[] buf, int off, int len) throws IOException {
    int count = 0;
    while (len > 0) {
      int i = read(buf, off, len);
      if (i == -1) {
	break;
      }
      count += i;
      off += i;
      len -= i;
    }
    return (count);
  }

  protected ZipEntry createZipEntry(String name) {
	// XXX FIXME what is this supposed to do?
	return null;
  }
}

