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

public class GZIPOutputStream extends DeflaterOutputStream {

protected CRC32 crc;
private int count;
private OutputStream strm;

public GZIPOutputStream(OutputStream out) throws IOException {
        this(out, 512);
}

public GZIPOutputStream(OutputStream out, int readsize) throws IOException {
        super(out, new Deflater(Deflater.DEFAULT_COMPRESSION, true), readsize);
	strm = out;
	crc = new CRC32();
	count = 0;

	/* Write GZIP header */
	writeByte(31);	// GZIP identifier
	writeByte(139);	//  "
	writeByte(8);	// Deflating
	writeByte(0);	// No extra bits
	writeInt(0);	// No time stamp
	writeByte(2);	// Maximum compression, slowest algorithm
	writeByte(255);	// Unknown OS
}

public void close() throws IOException {
	finish();
	super.close();
}

public void finish() throws IOException {
	super.finish();
	// Make sure we only do this once.
	if (count > -1) {
		writeInt((int)crc.getValue());
		writeInt(count);
		count = -1;
	}
}

private void writeInt(int v) throws IOException {
	byte[] buf = new byte[4];
	buf[0] = (byte)v;
	buf[1] = (byte)(v >> 8);
	buf[2] = (byte)(v >> 16);
	buf[3] = (byte)(v >> 24);
	strm.write(buf);
}

private void writeByte(int v) throws IOException {
	byte[] buf = new byte[1];
	buf[0] = (byte)v;
	strm.write(buf);
}

public void write(byte[] buf, int off, int len) throws IOException {
	super.write(buf, off, len);
	crc.update(buf, off, len);
	count += len;
}

}
