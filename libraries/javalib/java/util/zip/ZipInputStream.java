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
  private byte dheader[] = new byte[DATA_RECSZ];
  private boolean closed;
  private SwitchInflater sinf;
  private ZipEntry entry = null;

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
	  //System.out.println("found CEN header");
	  return (null);
	}
	else if (sig == DATA_HEADSIG) {
	  //System.out.println("found DATA header");
	  throw new IOException("DATA header signature found at invalid location");
	}
	else if (sig == LOC_HEADSIG) {
	  //System.out.println("found LOC header");
	  if (readFully(zheader, sigbuf.length, zheader.length - sigbuf.length) == zheader.length - sigbuf.length) {
	    break;
	  }
	}
        throw new IOException("LOC header signature bad: 0x" + Long.toHexString(sig));
      }

      char[] name = new char[get16(zheader, LOC_FILENAMELEN)];
      for (int i = 0; i < name.length; i++) {
        name[i] = (char)read();	// So much for Unicode ...!
      }

      byte[] extra = new byte[get16(zheader, LOC_EXTRAFIELDLEN)];
      readFully(extra, 0, extra.length);

      entry = new ZipEntry(new String(name));
      entry.version = get16(zheader, LOC_VERSIONEXTRACT);
      entry.flag    = get16(zheader, LOC_FLAGS);

      entry.setMethod(get16(zheader, LOC_METHOD));
      entry.setDosTime((int)get32(zheader, LOC_TIME));
      entry.setCrc(get32(zheader, LOC_CRC));

      entry.setCompressedSize( get32(zheader, LOC_COMPRESSEDSIZE) );
      entry.setSize( get32(zheader, LOC_UNCOMPRESSEDSIZE) );

      entry.setExtra( extra );
      entry.setComment("");
      entry.offset = 0;

      // Select the loader, simple or inflater.
      if (entry.method == ZipConstants.STORED) {
	sinf.setMode(true);
	sinf.setLength((int)entry.csize);
      }
      else {
	sinf.setMode(false);
      }

      //System.out.println("ZipEntry (Post LOC): " + entry + ", meth=" + entry.method + ", size=" + entry.size + ", csize=" + entry.csize + ", crc=" + entry.crc);

      closed = false;

      return (entry);
    }
  }

  public void closeEntry() throws IOException {
      synchronized (this) {
	if (closed == false) {
	        // skip remaining compressed data
		byte[] buf = new byte[512];
		while (read(buf, 0, buf.length) != -1) {}

		closed = true;
		// reset inflate state machine
		sinf.reset();
		sinf.setMode(true);
		sinf.setLength(Integer.MAX_VALUE);
	}
	if ((entry != null) && (entry.method == ZipConstants.DEFLATED)) {
	    // In the case where closeEntry() is
	    // called after reading a compressed entry
	    // we need to read the DATA header from
	    // and update the ZipEntry with the info
	    // from the DATA header. This is so that
	    // users can find out the compressed and
	    // uncompressed sizes of data written in
	    // a compressed entry!

	    if (readFully(sigbuf, 0, sigbuf.length) != sigbuf.length) {
		throw new IOException("signature not found");
	    }

	    long sig = get32(sigbuf, 0);

	    if (sig == CEN_HEADSIG) {
		throw new IOException("CEN header signature found at invalid location");
	    } else if (sig == END_ENDSIG) {
		throw new IOException("DATA header signature found at invalid location");
	    } else if (sig == LOC_HEADSIG) {
		throw new IOException("LOC header signature found at invalid location");
	    } else if (sig != DATA_HEADSIG) {
		throw new IOException("DATA header signature bad: 0x" +
				      Long.toHexString(sig));
	    }

	    //System.out.println("found DATA header in closeEntry()");

	    if (readFully(dheader, sigbuf.length, dheader.length - sigbuf.length)
		!= (dheader.length - sigbuf.length)) {
		throw new IOException("DATA header could not be read");
	    }

	    long data_crc = get32(dheader, DATA_CRC);

	    if (data_crc == 0) {
		throw new IOException("CRC of 0 is not valid in a DATA header");
	    }

	    entry.setCrc( data_crc );

	    long data_csize = get32(dheader, DATA_COMPRESSEDSIZE);

	    if (data_csize == 0) {
		throw new IOException("COMPRESSEDSIZE of 0 is not valid in a DATA header");
	    }

	    entry.setCompressedSize( data_csize );

	    long data_size = get32(dheader, DATA_UNCOMPRESSEDSIZE);

	    if (data_size == 0) {
		throw new IOException("UNCOMPRESSEDSIZE os 0 is not valid in a DATA header");
	    }

	    entry.setSize( data_size );

	    //System.out.println("ZipEntry (Post DATA): " + entry + ", meth=" + entry.method + ", size=" + entry.size + ", csize=" + entry.csize + ", crc=" + entry.crc);

	    entry = null;
	}
      }
  }

  public int read(byte[] b, int off, int len) throws IOException {
    return super.read(b, off, len);
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

