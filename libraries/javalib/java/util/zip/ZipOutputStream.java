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

import java.io.OutputStream;
import java.io.IOException;
import java.util.Vector;
import java.util.Enumeration;

public class ZipOutputStream extends DeflaterOutputStream implements ZipConstants {

public static final int DEFLATED = 8;
public static final int STORED = 0;

private static final int ZIPVER = 0x000a;

private int method = STORED;
private int level = Deflater.DEFAULT_COMPRESSION;
private byte[] lh = new byte[LOC_RECSZ];
private byte[] ch = new byte[CEN_RECSZ];
private byte[] ce = new byte[END_RECSZ];
private ZipEntry curr;
private Vector dir;
private OutputStream strm;
private int dout;
private int eout;
private CRC32 crc;

class Storer
  extends Deflater {

private int total;
private byte[] buf;
private int off;
private int len;

Storer() {
}

public int deflate(byte[] b, int p, int l) {
	if (l >= len) {
		l = len;
	}
	System.arraycopy(buf, off, b, p, l);
	total += l;
	off += l;
	len -= l;
	return (l);
}

public synchronized void setInput(byte b[], int o, int l)
{
	buf = b;
	off = o;
	len = l;
}

public boolean needsInput() {
	return (len > 0 ? false : true);
}

public boolean finished() {
	return (len > 0 ? false : true);
}

public int getTotalIn() {
	return (total);
}

public int getTotalOut() {
	return (total);
}

public void reset() {
}

}

public ZipOutputStream(OutputStream out)
{
	super(out);
	strm = out;
	curr = null;
	dir = new Vector();
	dout = 0;
	crc = new CRC32();
}

public void close() throws IOException
{
	finish();
	super.close();
}

public void closeEntry() throws IOException
{
	if (curr == null) {
		return;
	}

	super.finish();
	int in = def.getTotalIn();
	int out = def.getTotalOut();
	long crcval = crc.getValue();
	def.reset();
	crc.reset();

	if (curr.csize != -1 && curr.csize != out) {
		throw new ZipException("compress size set incorrectly");
	}
	if (curr.size != -1 && curr.size != in) {
		throw new ZipException("uncompress size set incorrectly");
	}
	if (curr.crc != 0 && curr.crc != crcval) {
		throw new ZipException("crc set incorrectly");
	}

	curr.csize = out;
	curr.size = in;
	curr.crc = crcval;
	dout += curr.csize;

	curr = null;
}

public void finish() throws IOException
{
	if (dir == null) {
		return;
	}

	closeEntry();

	// Write the directory.
	Enumeration e = dir.elements();
	int count = 0;
	int size = 0;
	while (e.hasMoreElements()) {

		ZipEntry ze = (ZipEntry)e.nextElement();

		put32(ch, CEN_SIGNATURE, (int)CEN_HEADSIG);
		put16(ch, CEN_VERSIONMADE, ZIPVER);
		put16(ch, CEN_VERSIONEXTRACT, ZIPVER);
		put16(ch, CEN_FLAGS, ze.flag);
		put16(ch, CEN_METHOD, ze.method);
		put16(ch, CEN_TIME, 0);
		put16(ch, CEN_DATE, 0);
		put32(ch, CEN_CRC, (int)ze.crc);
		put32(ch, CEN_COMPRESSEDSIZE, (int)ze.csize);
		put32(ch, CEN_UNCOMPRESSEDSIZE, (int)ze.size);
		put16(ch, CEN_FILENAMELEN, ze.name == null ? 0 : ze.name.length());
		put16(ch, CEN_EXTRAFIELDLEN, ze.extra == null ? 0 : ze.extra.length);
		put16(ch, CEN_FILECOMMENTLEN, ze.comment == null ? 0 : ze.comment.length());
		put16(ch, CEN_DISKNUMBER, 0);
		put16(ch, CEN_INTERNALATTR, 0);
		put32(ch, CEN_EXTERNALATTR, 0);
		put32(ch, CEN_LCLOFFSET, (int)ze.offset);

		strm.write(ch);
		strm.write(ze.name.getBytes());

		count++;
		size += CEN_RECSZ + ze.name.length();
	}

	put32(ce, END_SIGNATURE, (int)END_ENDSIG);
	put16(ce, END_DISKNUMBER, 0);
	put16(ce, END_CENDISKNUMBER, 0);
	put16(ce, END_TOTALCENONDISK, count);
	put16(ce, END_TOTALCEN, count);
	put32(ce, END_CENSIZE, size);
	put32(ce, END_CENOFFSET, dout);
	put16(ce, END_COMMENTLEN, 0);

	strm.write(ce);

	dir = null;
}

public void putNextEntry(ZipEntry ze) throws IOException
{
	closeEntry();	// Close previous entry

	put32(lh, LOC_SIGNATURE, (int)LOC_HEADSIG);
	put16(lh, LOC_VERSIONEXTRACT, ZIPVER);
	put16(lh, LOC_FLAGS, ze.flag);
	if (ze.method == -1) {
		ze.method = method;
	}
	if (ze.method == STORED) {
		if (ze.size == -1) {
			throw new ZipException("size not set in stored entry");
		}
		ze.csize = ze.size;
		if (ze.crc == 0) {
			throw new ZipException("crc not set in stored entry");
		}
	}

	if (curr == null || curr.method != ze.method) {
		if (ze.method == STORED) {
			def = new Storer();
		}
		else {
			def = new Deflater(level, true);
		}
	}

	put16(lh, LOC_METHOD, ze.method);
	put16(lh, LOC_TIME, 0);
	put16(lh, LOC_DATE, 0);
	put32(lh, LOC_CRC, (int)ze.crc);

	put32(lh, LOC_COMPRESSEDSIZE, ze.csize == -1 ? 0 : (int)ze.csize);
	put32(lh, LOC_UNCOMPRESSEDSIZE, ze.size == -1 ? 0 : (int)ze.size);

	put16(lh, LOC_FILENAMELEN, ze.name == null ? 0 : ze.name.length());
	put16(lh, LOC_EXTRAFIELDLEN, ze.extra == null ? 0 : ze.extra.length);

	strm.write(lh);
	if (ze.name != null) {
		strm.write(ze.name.getBytes());
	}
	if (ze.extra != null) {
		strm.write(ze.extra);
	}

	ze.offset = dout;
	dout += LOC_RECSZ + ze.name.length();

	// Add entry to list of entries we need to write at the end.
	dir.addElement(ze);

	curr = ze;
}

public void setComment(String comment)
{
	// Currently ignored.
}

public void setLevel(int lvl)
{
	if ((lvl < Deflater.NO_COMPRESSION || lvl > Deflater.BEST_COMPRESSION) && lvl != Deflater.DEFAULT_COMPRESSION) {
		throw new IllegalArgumentException("bad compression level");
	}
	level = lvl;
}

public void setMethod(int m)
{
	if (m != DEFLATED && m != STORED) {
		throw new IllegalArgumentException("bad compression method");
	}
	method = m;
}

public synchronized void write(byte[] buf, int off, int len) throws IOException
{
	super.write(buf, off, len);
	crc.update(buf, off, len);
}

private void put16(byte[] zheader, int pos, int val) {
	zheader[pos] = (byte)val;
	zheader[pos+1] = (byte)(val >> 8);
}

private void put32(byte[] zheader, int pos, int val) {
	zheader[pos] = (byte)val;
	zheader[pos+1] = (byte)(val >> 8);
	zheader[pos+2] = (byte)(val >> 16);
	zheader[pos+3] = (byte)(val >> 24);
}

}
