
/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.io;

import kaffe.util.UTF8;

public class DataOutputStream extends FilterOutputStream implements DataOutput {
	protected int written;

public DataOutputStream(OutputStream out) {
	super(out);
}

public void flush() throws IOException {
	super.flush();
}

final public int size() {
	return written;
}

public synchronized void write(byte b[], int off, int len) throws IOException {
	if (off < 0 || len < 0 || off + len > b.length) {
	   throw new IndexOutOfBoundsException();
	}

	super.write(b, off, len);
	written += len;
}

public synchronized void write(int b) throws IOException {
	super.write(b);
	written++;		
}

final public void writeBoolean(boolean v) throws IOException {
	if (v) writeByte(1); else writeByte(0);
}

final public void writeByte(int v) throws IOException {
	write(v);
}

final public void writeBytes(String s) throws IOException {
	final char[] c = s.toCharArray();
	byte[] b = new byte[c.length];
	for (int pos = 0; pos < c.length; pos++)
		b[pos] = (byte)(c[pos] & 0xFF);
	write(b, 0, b.length);
}

final public void writeChar(int v) throws IOException {
	writeByte((v & 0xFF00) >> 8);
	writeByte((v & 0x00FF));		
}

final public void writeChars(String s) throws IOException {
	for (int pos=0; pos<s.length(); pos++) {
		writeChar(s.charAt(pos));
	}
}

final public void writeDouble(double v) throws IOException {
	writeLong(Double.doubleToLongBits(v));
}

final public void writeFloat(float v) throws IOException {
	writeInt(Float.floatToIntBits(v));
}

final public void writeInt(int v) throws IOException {
	byte b[] = new byte[4];
	int i, shift;

	for (i = 0, shift = 24; i < 4; i++, shift -= 8)
		b[i] = (byte)(0xFF & (v >> shift));
	write(b, 0, 4);
}

final public void writeLong(long v) throws IOException {
	final int hiInt=(int )(v >> 32);
	final int loInt=(int )(v & 0xFFFFFFFF);

	writeInt(hiInt);
	writeInt(loInt);
}

final public void writeShort(int v) throws IOException {
	writeChar(v);
}

final public void writeUTF(String str) throws IOException {
	final byte[] data = UTF8.encode(str);
	if (data.length > 0xffff) {
		throw new UTFDataFormatException("String too long");
	}
	synchronized(this) {
		writeShort(data.length);
		write(data, 0, data.length);
	}
}

}

