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
	/* used to determine whether to skip next '\n' */
	private boolean skipNextLF;

public DataInputStream(InputStream in) {
	super(in);
}

public final int read(byte b[]) throws IOException {
	return read(b, 0, b.length);
}

public final int read(byte b[], int off, int len) throws IOException {
	if (off < 0 || len < 0 || off + len > b.length) {
	   throw new IndexOutOfBoundsException();
	}

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
	if (skipNextLF) {
		skipNextLF = false;
		if (value == '\n') {
			value = readByte();
		}
	}
	return ((byte)value);
}

public final char readChar() throws IOException {
	int val = readUnsignedByte() << 8;
	val |= readUnsignedByte();
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
	int total = 0;
	while (total < len) {
		final int got = read(b, off + total, len - total);
		if (got == -1) {
			throw new EOFException();
		}
		total += got;
	}
}

public final int readInt() throws IOException {
	int v1 = readUnsignedByte() << 24;
	v1 |= readUnsignedByte() << 16;
	v1 |= readUnsignedByte() << 8;
	v1 |= readUnsignedByte();
	return v1;
}

/**
 * @deprecated
 */
public final String readLine() throws IOException {
	final StringBuffer buffer = new StringBuffer();
	boolean eof = false;

	try {
		while (true) {
			final char ch = (char) readUnsignedByte();
			if (ch == '\n') {
				break;
			}
			if (ch == '\r') {
				skipNextLF = true;
				break;
			}

			buffer.append(ch);
		}
	}
	catch (EOFException e) {
		eof = true;
	}

	if (eof && buffer.length() == 0) {
		return (null);
	}
	return (buffer.toString());
}

public final long readLong() throws IOException {
	return ((long)readInt() << 32) | ((long)readInt() & 0xffffffffL);
}

public final short readShort() throws IOException {
	int val = readUnsignedByte() << 8;
	val |= readUnsignedByte();
	return ((short)val);
}

public final String readUTF() throws IOException {
	return readUTF(this);
}

public static final String readUTF(DataInput in) throws IOException {
	return UTF8.decode(in, in.readUnsignedShort());
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
