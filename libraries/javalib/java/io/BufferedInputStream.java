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

public class BufferedInputStream
  extends FilterInputStream
{
	protected byte[] buf;
	protected int count;
	protected int pos;
	protected int markpos = -1;
	protected int marklimit;
	private boolean EOF = false;
	final private static int DEFAULTBUFFER = 2048;

public BufferedInputStream(InputStream in) {
	this(in, DEFAULTBUFFER);
}

public BufferedInputStream(InputStream in, int size) {
	super(in);
	buf=new byte[size];
	count=size;
	pos=size;
	marklimit=0;
}

public synchronized int available() throws IOException {
	return (count-pos)+in.available();
}

private void fill() {
	marklimit = 0;
	pos = 0;
	try {
		count = super.read(buf);
	} catch (IOException e) {
		count = -1;
	}
}

public synchronized void mark(int readlimit) {
	marklimit = readlimit;
	markpos = pos;
}

public boolean markSupported() {
	return true;
}

public synchronized int read() throws IOException {
	if (pos >= count) {
		fill();
		if (count <= 0) {
			return -1;
		}
	}
	return (buf[pos++] & 0xFF);
}

public synchronized int read(byte b[], int off, int len) throws IOException {
	if (count <= 0) {
		return (-1);
	}
	int i;
	for (i = 0; i < len; i++) {
		if (pos >= count) {
			// If we've put something in the buffer and the last
			// buffered read didn't fill it, we quit now and
			// don't refill.
			if (i > 0 && count != buf.length) {
				break;
			}
			fill();
			if (count <= 0) {
				break;
			}
		}
		b[off+i] = buf[pos++];
	}
	return (i);
}

public synchronized void reset() throws IOException {
	if (pos > markpos + marklimit) {
		throw new IOException();
	}
	pos = markpos;
}

public synchronized long skip(long n) throws IOException {
	long a = n - (long)(count - pos);
	if (a <= 0) {
		pos += (int)n;
		return n;
	}
	else {
		a = super.skip(a) + (count-pos);
		pos = count;
		return a;
	}
}
}
