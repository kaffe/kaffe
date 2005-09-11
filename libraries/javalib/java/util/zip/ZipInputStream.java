
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
import java.io.UnsupportedEncodingException;

import kaffe.util.UTF8;
import kaffe.util.zip.SwitchInflater;

public class ZipInputStream extends InflaterInputStream
	implements ZipConstants {

  private byte sigbuf[] = new byte[4];
  private byte zheader[] = new byte[LOCHDR];
  private byte dheader[] = new byte[EXTHDR];
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

      // First, open the stream.
      sinf.setLength(Integer.MAX_VALUE);

      // Read next signature
      int sig = readSig();
      switch (sig) {
      case (int)CENSIG:		// central dir: no more entries
	return null;
      case (int)LOCSIG:		// another entry
	break;
      default:				// unexpected
	return null;
      }

      // Read remainder of local header
      readFully(zheader, sigbuf.length, zheader.length - sigbuf.length);

      // Read filename; assume UTF-8 encoding
      byte[] nameBuf = new byte[get16(zheader, LOCNAM)];
      readFully(nameBuf, 0, nameBuf.length);

      // Read extra field
      byte[] extra = new byte[get16(zheader, LOCEXT)];
      readFully(extra, 0, extra.length);

      // Setup new entry
      entry = createZipEntry(UTF8.decode(nameBuf));
      entry.version = get16(zheader, LOCVER);
      entry.flag    = get16(zheader, LOCFLG);

      entry.setMethod(get16(zheader, LOCHOW));
      entry.setDosTime(get32(zheader, LOCTIM));
      entry.setCrc(get32(zheader, LOCCRC) & 0xffffffffL);

      entry.setCompressedSize(get32(zheader, LOCSIZ) & 0xffffffffL);
      entry.setSize(get32(zheader, LOCLEN) & 0xffffffffL);

      entry.setExtra( extra );
      entry.setComment("");
      entry.offset = 0;

      // Select the loader, simple or inflater.
      if (entry.getMethod() == ZipEntry.STORED) {
	sinf.setMode(true);
	sinf.setLength((int)entry.getCompressedSize());
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
	if (entry.getMethod() == ZipEntry.DEFLATED && peekSig() == (int)EXTSIG) {

	  // Consume signature
	  readSig();

	  // Read remainder of data header
	  readFully(dheader, sigbuf.length, dheader.length - sigbuf.length);

	  // Read CRC
	  int data_crc = get32(dheader, EXTCRC);
	  entry.setCrc(data_crc & 0xffffffffL);

	  // Read compressed size
	  int data_csize = get32(dheader, EXTSIZ);
	  entry.setCompressedSize(data_csize & 0xffffffffL);

	  // Read uncompressed size
	  int data_size = get32(dheader, EXTLEN);
	  if (data_crc == 0 && data_size != 0) {
	      throw new IOException(
		"CRC error: data_crc=0, data_csize=" + data_csize + ",data_size=" + data_size);
	  }
	  entry.setSize(data_size & 0xffffffffL);

	  //System.out.println("ZipEntry (Post DATA): " + entry
	  // + ", meth=" + entry.method + ", size=" + entry.size
	  // + ", csize=" + entry.csize + ", crc=" + entry.crc);
	}
	
	// Close the input stream for the rest of the world.
	sinf.setLength(0);
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
    // Now completely close the byte stream. There may be some
    // bytes left in the buffer.
    sinf.setInput(null, 0, 0);
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
	return new ZipEntry(name);
  }
}

