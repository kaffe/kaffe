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
				/* The spec demands that we handle '\r\n'
				   as a single end-of-line character. The
				   core of the problem is: How do you unread
				   the character following '\r' if it turns out
				   not to be '\n'?

				   There are several ways to do this, none
				   of them have worked really great so far.

				   *  Wrapping the input stream in the
				   constructor with a PushbackInputStream:
				   fails because classes extending DataInputStream
				   can access the protected field in. Casting that 
				   field to classes that don't extend PushbackInputStream
				   will result in a ClassCastException at runtime.
				   That bug prevented the jakarta BCEL verifier 
				   from verifying anything when run on kaffe.

				   * Using a boolean flag to decide if the
				   next '\n' should be skipped:
				   There is no way to skip it when the user uses
				   read() to read a single character. The API spec
				   says that DataInputStream doesn't override
				   FilteredInputStream.read(). This can lead to
				   input data curruption. That bug prevented jython
				   from running on kaffe.

				   * Mark and reset:
				   Not all streams support marking and resetting.

				   This is the current solution:

				   We use an internal buffer in FilterInputStream.
				   That's a clumsy workaround, but there doesn't seem
				   to be a better method.

				   Making sure bytes are available before
				   reading should prevent hanging on a socket.
				*/
				if (available() > 0) {
					final int lf = read();

					if (lf != -1 && lf != '\n') {
						super.buffer = lf;
					}
				}

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
