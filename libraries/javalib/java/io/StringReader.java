package java.io;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class StringReader
  extends Reader
{
	private char[] buf;
	private int pos;
	private int markpos;

public StringReader(String s) {
	buf = s.toCharArray();
}

/* internal function used to signal EOF */
private boolean atEOF() {
	return pos == buf.length;
}

/* Internal function used to check whether the
   StringReader has been closed already, throws
   an IOException in that case.
*/
private void checkIfStillOpen() throws IOException {
	if (buf == null) {
		throw new IOException("Stream closed");
	}
}

public void close() {
	synchronized(lock) {
		if (buf != null) {
			buf = null;
		}
	}
}

public void mark(int readAheadLimit) throws IOException {
	if (readAheadLimit < 0) {
		throw (new IllegalArgumentException("Read-ahead limit < 0"));
	}

	synchronized(lock) {
		checkIfStillOpen();
		markpos = pos;
	}
}

public boolean markSupported() {
	return (true);
}

public int read() throws IOException {
	synchronized(lock) {
		checkIfStillOpen();

		if (!atEOF()) {
			return ((int)buf[pos++]);
		}
	}
	return (-1);
}

public int read ( char cbuf[], int off, int len ) throws IOException {
	if (len < 0 || off < 0 || off + len > cbuf.length) {
		throw new IndexOutOfBoundsException();
	}

	synchronized(lock) {
		checkIfStillOpen();

		if (!atEOF()) {
			int num_read = Math.min(len, buf.length - pos);
			System.arraycopy(buf, pos, cbuf, off, num_read);
			pos += num_read;
			return num_read;
		}
	}
	return (-1);
}

public boolean ready() throws IOException {
	return (true);
}

public void reset() throws IOException {
	synchronized(lock) {
		checkIfStillOpen();
		pos = markpos;
	}
}

public long skip(long ns) throws IOException {
	synchronized(lock) {
		checkIfStillOpen();

		if (ns <= 0) {
			return 0;
		}
		
		long left = Math.min(ns, (long)(buf.length - pos));
		pos += (int)left;
		return (left);
	}
}
}
