
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

import java.io.IOException;
import java.io.InputStream;
import kaffe.util.UTF8;
import kaffe.util.zip.SwitchInflater;

public class ZipInputStream extends InflaterInputStream
	implements ZipConstants {

  private byte sigbuf[] = new byte[4];
  private byte zheader[] = new byte[LOC_RECSZ];
  private byte dheader[] = new byte[DATA_RECSZ];
  private boolean gotSig;
  private SwitchInflater sinf;
  private ZipEntry entry;

  public ZipInputStream(InputStream in) {
    super(in, new SwitchInflater(true, true));
    sinf = (SwitchInflater)inf;
    sinf.reset();
    sinf.setMode(true);
    sinf.setLength(Integer.MAX_VALUE);
  }

  public ZipEntry getNextEntry() throws IOException {

    closeEntry();

    synchronized (this) {

      // Read next signature
      int sig = readSig();
      switch (sig) {
      case (int)CEN_HEADSIG:		// central dir: no more entries
	return null;
      case (int)LOC_HEADSIG:		// another entry
	break;
      default:				// unexpected
	throw new IOException("Bogus signature: 0x" + Integer.toHexString(sig));
      }

      // Read remainder of local header
      readFully(zheader, sigbuf.length, zheader.length - sigbuf.length);

      // Read filename; assume UTF-8 encoding
      byte[] nameBuf = new byte[get16(zheader, LOC_FILENAMELEN)];
      readFully(nameBuf, 0, nameBuf.length);

      // Read extra field
      byte[] extra = new byte[get16(zheader, LOC_EXTRAFIELDLEN)];
      readFully(extra, 0, extra.length);

      // Setup new entry
      entry = new ZipEntry(UTF8.decode(nameBuf));
      entry.version = get16(zheader, LOC_VERSIONEXTRACT);
      entry.flag    = get16(zheader, LOC_FLAGS);

      entry.setMethod(get16(zheader, LOC_METHOD));
      entry.setDosTime(get32(zheader, LOC_TIME));
      entry.setCrc(get32(zheader, LOC_CRC) & 0xffffffffL);

      entry.setCompressedSize(get32(zheader, LOC_COMPRESSEDSIZE) & 0xffffffffL);
      entry.setSize(get32(zheader, LOC_UNCOMPRESSEDSIZE) & 0xffffffffL);

      entry.setExtra( extra );
      entry.setComment("");
      entry.offset = 0;

      // Select the loader, simple or inflater.
      if (entry.method == STORED) {
	sinf.setMode(true);
	sinf.setLength((int)entry.csize);
      }
      else {
	sinf.setMode(false);
      }

      //System.out.println("ZipEntry (Post LOC): " + entry + ", meth="
      // + entry.method + ", size=" + entry.size + ", csize=" + entry.csize
      // + ", crc=" + entry.crc);

      return entry;
    }
  }

  public void closeEntry() throws IOException {
    synchronized (this) {

      // Don't do this twice
      if (entry == null)
	return;

      // Cleanup
      try {

	// Skip remaining compressed data
	byte[] buf = new byte[512];
	while (read(buf, 0, buf.length) != -1);

	// Reset inflate state machine
	sinf.reset();
	sinf.setMode(true);
	sinf.setLength(Integer.MAX_VALUE);

	// In the case where closeEntry() is called after reading
	// a compressed entry, we need to read the DATA header that
	// follows, if any, and update the ZipEntry with the info
	// from the DATA header. This is so that users can find out
	// the compressed and uncompressed sizes of data written in
	// a compressed entry.
	if (entry.method == DEFLATED && peekSig() == (int)DATA_HEADSIG) {

	  // Consume signature
	  readSig();

	  // Read remainder of data header
	  readFully(dheader, sigbuf.length, dheader.length - sigbuf.length);

	  // Read CRC
	  int data_crc = get32(dheader, DATA_CRC);
	  entry.setCrc(data_crc & 0xffffffffL);

	  // Read compressed size
	  int data_csize = get32(dheader, DATA_COMPRESSEDSIZE);
	  entry.setCompressedSize(data_csize & 0xffffffffL);

	  // Read uncompressed size
	  int data_size = get32(dheader, DATA_UNCOMPRESSEDSIZE);
	  if (data_crc == 0 && data_size != 0) {
	      throw new IOException(
		"CRC error: data_crc=0, data_csize=" + data_csize + ",data_size=" + data_size);
	  }
	  entry.setSize(data_size & 0xffffffffL);

	  //System.out.println("ZipEntry (Post DATA): " + entry
	  // + ", meth=" + entry.method + ", size=" + entry.size
	  // + ", csize=" + entry.csize + ", crc=" + entry.crc);
	}
      } finally {
	entry = null;
      }
    }
  }

  public int read(byte[] b, int off, int len) throws IOException {
    return super.read(b, off, len);
  }

  public long skip(long n) throws IOException {
    return super.skip(n);
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

  private int get32(byte[] buf, int base) {
    return  buf[base] & 0xFF
	| ((buf[base+1] & 0xFF) << 8)
	| ((buf[base+2] & 0xFF) << 16)
	| ((buf[base+3] & 0xFF) << 24);
  }

  private void readFully(byte[] buf, int off, int len) throws IOException {
    int nread;

    for (int remain = len; remain > 0; remain -= nread) {
      nread = read(buf, off, remain);
      if (nread == -1) {
	if (remain != 0) {
	  throw new IOException("truncated file (only read "
	    + (len - remain) + "/" + len + ")");
	}
	return;
      }
      off += nread;
    }
  }

  private int readSig() throws IOException {
    int sig = peekSig();
    gotSig = false;
    return sig;
  }

  private int peekSig() throws IOException {
    if (!gotSig) {
      readFully(sigbuf, 0, sigbuf.length);
      gotSig = true;
    }
    return get32(sigbuf, 0);
  }

  protected ZipEntry createZipEntry(String name) {
	// XXX FIXME what is this supposed to do?
	return null;
  }
}

