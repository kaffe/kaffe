
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

abstract public class Reader {
	protected Object lock;
	private final char[] single = new char[1];

protected Reader() {
	lock = this;
}

protected Reader(Object lock) {
	if (lock == null) {
		throw new NullPointerException();
	}
	this.lock = lock;
}

abstract public void close() throws IOException;

public void mark(int readAheadLimit) throws IOException {
	throw new IOException("mark() not supported");
}

public boolean markSupported() {
	return false;
}

// This is just the VERY inefficient generic read(), it should be
// overridden in almost every subclass
public int read () throws IOException {
	synchronized (lock) {
	        return read(single, 0, 1) < 0 ? -1 : single[0];
	}
}

public int read(char cbuf[]) throws IOException {
	return read(cbuf, 0, cbuf.length);
}

abstract public int read(char cbuf[], int off, int len) throws IOException;

public boolean ready() throws IOException {
	return false;
}

public void reset() throws IOException {
	throw new IOException("reset() not supported");
}

public long skip(long n) throws IOException {
	if (n < 0) {
		throw new IllegalArgumentException("skip value is negative");
	}

	final char[] buf = new char[1024];
	int skipped = 0;

	while (n > 0) {
		final int r = read(buf, 0, Math.min(buf.length, (int)n));
		if (r < 0)
			break;
		n -= r;
		skipped += r;
	}
	return skipped;
}

}

