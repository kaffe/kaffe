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

public class DataInputStream extends FilterInputStream implements DataInput {

public DataInputStream(InputStream in) {
	super(in);
}

public final int read(byte b[]) throws IOException {
	return read(b, 0, b.length);
}

public final int read(byte b[], int off, int len) throws IOException {
	return super.read(b, off, len);
}

public final boolean readBoolean() throws IOException {
	return (readUnsignedByte() != 0);
}

public final byte readByte() throws IOException {
	int value = read();
	if (value == -1) {
		throw new EOFException();
	}
	return ((byte)value);
}

public final char readChar() throws IOException {
	int val = read() << 8;
	val |= read();
	if (val == -1) {
		throw new EOFException();
	}
	return ((char)val);
}

public final double readDouble() throws IOException {
	return Double.longBitsToDouble(readLong());
}

public final float readFloat() throws IOException {
	return Float.intBitsToFloat(readInt());
}

public final void readFully(byte b[]) throws IOException {
	readFully(b, 0, b.length);    
}

public final void readFully(byte b[], int off, int len) throws IOException {
	if (b == null) {
		throw new NullPointerException();
	}
	int total = 0;
	while (total < len) {
		int got = read(b, off + total, len - total);
		if (got == -1) {
			throw new EOFException();
		}
		total += got;
	}
}

public final int readInt() throws IOException {
	int v1 = read() << 24;
	v1 |= read() << 16;
	v1 |= read() << 8;
	int v2 = read();
	if (v2 == -1) {
		throw new EOFException();
	}
	return (v1 | v2);
}

/**
 * @deprecated
 */
public final String readLine() throws IOException {
	final StringBuffer buffer = new StringBuffer();
	final byte[] data = new byte[1];
	boolean eof = false;

	while (true) {
		if (read(data, 0, 1) != 1) {
			eof = true;
			break;
		}
		final char ch = (char) (data[0] & 0xff);
		if (ch == '\n') {
			break;
		}

		if (ch == '\r') {       // Check for '\r\n'
			// Note that we don't know whether the InputStream
			// implements mark() and reset(), but we're using
			// them anyway. If they don't, then characters
			// after a lone '\r' will be elided from the input
			// (ie, this is a bug). We could override mark()
			// and reset() to always provide at least a
			// one-character buffer, but then we'd violate
			// the spec, which says to inherit these from
			// FilterInputStream...

			super.mark(1);
			if (read(data, 0, 1) == 1 && data[0] != '\n') {
				try {
					super.reset();
				}
				catch (IOException e) {
				}
			}
			break;
		}
		buffer.append(ch);
	}
	if (eof == true && buffer.length() == 0) {
		return (null);
	}
	return (buffer.toString());
}

public final long readLong() throws IOException {
	return ((long)readInt() << 32) | ((long)readInt() & 0xffffffffL);
}

public final short readShort() throws IOException {
	int val = read() << 8;
	val |= read();
	if (val == -1) {
		throw new EOFException();
	}
	return ((short)val);
}

public final String readUTF() throws IOException {
	return UTF8.decode(this, readUnsignedShort());
}

public final int readUnsignedByte() throws IOException {
	return readByte() & 0xff;
}

public final int readUnsignedShort() throws IOException {
	return readShort() & 0xffff;
}

public final int skipBytes(int n) throws IOException {
	int remain = n;
	while (remain > 0) {
		remain -= (int)in.skip((long)remain);
	}
	return n;
}
}
