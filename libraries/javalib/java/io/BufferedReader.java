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


public class BufferedReader
  extends Reader
{
	final private static int DEFAULTBUFFERSIZE = 1024;
	private Reader rd;
	private char[] inbuf;
	private int pos;
	private int size;
	private boolean markset;

public BufferedReader(Reader in) {
	this(in, DEFAULTBUFFERSIZE);
}

public BufferedReader(Reader in, int sz) {
	rd = in;
	inbuf = new char[sz];
	pos = 0;
	size = 0;
	markset = false;
}

public void close() throws IOException {
	rd.close();
}

public void mark(int readAheadLimit) throws IOException {
	synchronized(lock) {

		char[] oldbuf = inbuf;

		if (readAheadLimit > inbuf.length) {
			inbuf = new char[readAheadLimit];
		}
		/* Copy data to the beginning of the buffer */
		System.arraycopy(oldbuf, pos, inbuf, 0, size);
		pos = 0;
		markset = true;
	}
}

public boolean markSupported() {
	return (true);
}

protected void pushback() {
	size++;
	pos--;
}

public int read () throws IOException {
	synchronized ( lock ) {
		if (pos < size) {
			return (inbuf[pos++]);
		}
		else if (refillBuffer() > 0) {
			return (inbuf[pos++]);
		}
		else {
			return (-1);
		}
	}
}

public int read ( char cbuf[], int off, int len ) throws IOException {
	int cnt;

	synchronized(lock) {
		for (;;) {
			for (cnt = 0; cnt < size && cnt < len; cnt++) {
				cbuf[off++] = inbuf[pos++];
			}
			size -= cnt;
			len -= cnt;

			/* If we're out of buffer space then return now */
			if (len == 0) {
				return (cnt);
			}

			// If we've put something in the buffer already, and
			// the last refill was short, we terminate now.
			if (cnt > 0 && size != inbuf.length) {
				return (cnt);
			}

			if (refillBuffer() <= 0) {
				if (cnt <= 0) {
					return (-1);
				}
				return (cnt);
			}
		}
	}
}

public String readLine () throws IOException {
	int     start = pos;
	char    c = ' ';    // silly javac, complains about initialization
	String  s = null;

	synchronized ( lock ) {
		while ( true ) {
			for (; pos < size && (c = inbuf[pos]) != '\n' && c != '\r'; pos++);
			if (pos == size) {	// nothing found yet
				if (pos > start) {
					if (s == null) {
						s = new String(inbuf, start, pos-start);
					}
					else {
						s += new String(inbuf, start, pos-start);
					}
				}
				if (refillBuffer() <= 0) {
					return (s);
				}
				start = 0;
			}
			else {			// we got a line terminator
				if ( s == null ) {
					s = new String(inbuf, start, pos-start);
				}
				else {
					s += new String(inbuf, start, pos-start);
				}
				pos++;
				if (c == '\r') {
					if (read() != '\n' && pos > 0) {
						pos--;  // skip over "\r\n"
					}
				}
				return (s);
			}
		}
	}
}

public boolean ready() throws IOException {
	synchronized(lock) {

		if (size > 0 || rd.ready()) {
			return (true);
		}
		else {
			return (false);
		}

	}
}

private int refillBuffer () throws IOException {
	int n;

	synchronized ( lock ) {
		n = rd.read( inbuf, 0, inbuf.length);
		pos = 0;
		markset = false;
			
		if (n > 0) {
			size = n;
			return (n);
		}
		else {
			size = 0;
			return (-1);
		}
	}
}

public void reset() throws IOException {
	synchronized(lock) {

		if (!markset) {
			throw new IOException("invalid mark");
		}
		/* Reset to the beginning of the buffer */
		size += pos;
		pos = 0;

	}
}

public long skip(long n) throws IOException {

	synchronized(lock) {

		/* If we can skip without leaving the buffer do that now */
		if (n < (long)size) {
			size -= (int)n;
			pos += (int)n;
			return (n);
		}

		/* Otherwise empty the buffer and skip though the data stream */
		long cnt = (long)size;
		n -= cnt;
		size = 0;
		pos = 0;

		for (;;) {
			markset = false;
			long r = (long)rd.read(inbuf, 0, inbuf.length);
			if (r == -1) {
				return (cnt);
			}
			if (r > n) {
				pos = (int)n;
				size = (int)(r - n);
				cnt += n;
				return (cnt);
			}
			cnt += r;
			n -= r;
		}
	}
}
}
