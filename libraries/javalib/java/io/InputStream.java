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

abstract public class InputStream extends Object {

private static final byte skipBuffer[] = new byte[1024];

public int available() throws IOException {
	return 0;
}

public void close() throws IOException {
	/* Do nothing */
}

public synchronized void mark(int readlimit) {
	/* Do nothing */
}

public boolean markSupported() {
	return false;
}

abstract public int read() throws IOException;

public int read(byte b[]) throws IOException {
	return read(b, 0, b.length);
}

public int read(byte b[], int off, int len) throws IOException {
	if (len < 0 || off < 0 || off + len > b.length) {
		throw new IndexOutOfBoundsException();
	}

	for (int pos=off; pos<off+len; pos++) {
	    final int data=read();
	    if (data==-1) {
		if (pos == off) return -1; else return pos-off;
	    }
	    b[pos]=(byte )data;
	}

	return len;
}

public synchronized void reset() throws IOException {
	throw new IOException();
}

public long skip(long n) throws IOException {
	long skipped = 0;

	while (n > 0) {
		final int r = read(skipBuffer,
				   0,
				   Math.min(skipBuffer.length, (int)n));
		if (r < 0)
			break;
		n -= r;
		skipped += r;
	}
	return skipped;
}

}
