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

public class BufferedInputStream extends FilterInputStream
{
	protected byte[] buf;
	protected int count;
	protected int pos;
	protected int markpos;
	protected int marklimit;
	final private static int DEFAULTBUFFER = 256;

/*
 * Code invariants:
 *
 * 1 markpos <= pos <= count <= buf.length >= marklimit
 *
 * 2 If (count - pos > 0) then markpos != -1. That is, we only keep
 *   data in the buffer if a mark is set. A mark remains set until
 *   more than marklimit bytes have been read and/or skipped.
 *
 * 3 If (markpos == -1) then (count == 0 && pos == 0).
 */

public BufferedInputStream(InputStream in) {
	this(in, DEFAULTBUFFER);
}

public BufferedInputStream(InputStream in, int size) {
	super(in);
	buf = new byte[size];
	pos = count = 0;
	marklimit = size;
	markpos = -1;
}

public synchronized int available() throws IOException {
	return (count - pos) + in.available();
}

public synchronized void mark(int marklimit) {
	if (marklimit > buf.length - pos) {		// not enough room
		byte[] newbuf;

		if (marklimit <= buf.length) {
			newbuf = buf;			// just shift buffer
		} else {
			newbuf = new byte[marklimit];	// need a new buffer
		}
		System.arraycopy(buf, pos, newbuf, 0, count - pos);
		buf = newbuf;
		count -= pos;
		pos = markpos = 0;
	} else {
		markpos = pos;
	}
	this.marklimit = marklimit;
}

public boolean markSupported() {
	return true;
}

public synchronized int read() throws IOException {
	if (markpos == -1) {		// if no mark set, just read directly
		return super.read();
	} else if (pos == buf.length) {	// buffer consumed, invalidate it
		pos = count = 0;
		markpos = -1;
		return super.read();
	} else if (pos == count) {	// read more data into buffer first
		if (!fillBuffer()) {
			return -1;
		}
	}
	return (buf[pos++] & 0xFF);	// return next buffered byte
}

public synchronized int read(byte b[], int off, int len) throws IOException {
	int nread, total = 0;

	while (len > 0) {

		// If buffer fully consumed, invalidate mark & reset buffer
		if (pos == buf.length) {
			pos = count = 0;
			markpos = -1;
		}

		// If no mark is set, optimize with a direct read
		if (markpos == -1) {
			if ((nread = super.read(b, off, len)) == -1) {
				return (total > 0) ? total : -1;
			}
			return total + nread;
		}

		// If no data in buffer, go get some more
		if (pos == count) {
			if (!fillBuffer()) {
				return (total > 0) ? total : -1;
			}
		}

		// Copy a chunk of bytes from our buffer
		nread = count - pos;
		if (nread > len) {
			nread = len;
		}
		System.arraycopy(buf, pos, b, off, nread);
		total += nread;
		pos += nread;
		off += nread;
		len -= nread;
	}
	return total;
}

public synchronized void reset() throws IOException {
	if (markpos == -1) {
		throw new IOException(
		    "Attempt to reset when no mark is valid"
			+ " (marklimit=" + marklimit + ")");
	}
	pos = markpos;
}

/*
 * This version of skip() does not invalidate a mark if less
 * than marklimit total bytes are read and/or skipped.
 * Not sure if this is actually a requirement or not.
 */
public synchronized long skip(long n) throws IOException {

	// Sanity check
	if (n <= 0) {
		return 0;
	}

	// Optimize for case of no mark set
	if (markpos == -1) {
		return super.skip(n);
	}

	// Skip buffered data, if available
	if (pos < count) {
		if (count - pos > n) {
			pos += (int)n;		// narrowing cast OK
		} else {
			n = count - pos;
			pos = count;
		}
		return n;
	}

	// If buffer fully consumed, invalidate mark & reset buffer
	if (pos == buf.length) {
		pos = count = 0;
		markpos = -1;
		return super.skip(n);
	}

	// Read data into buffer and try again
	return fillBuffer() ? skip(n) : 0;
}

/*
 * Get more buffered data. This should only be called when are all true:
 *
 *	1 markpos != -1
 *	2 pos == count
 *	3 count < buf.length
 *
 * Returns true if at least one byte was read.
 */
private boolean fillBuffer() throws IOException {
	int	nread;

	if ((nread = super.read(buf, pos, buf.length - pos)) <= 0) {
		return false;
	}
	count += nread;
	return true;
}
}
