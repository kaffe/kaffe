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

import java.lang.String;

public class BufferedReader
  extends Reader
{
	final private static int DEFAULTBUFFERSIZE = 1024;
	private Reader rd;
	private char[] inbuf;
	private int pos;	// position of next char in buffer
	private int size;	// total length of valid chars in buffer
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

		// Allocate bigger buffer if necessary
		if (readAheadLimit > inbuf.length) {
			inbuf = new char[readAheadLimit];
		}

		// Shift data to the beginning of the buffer
		System.arraycopy(oldbuf, pos, inbuf, 0, size - pos);
		size -= pos;
		pos = 0;
		markset = true;
	}
}

public boolean markSupported() {
	return (true);
}

protected void pushback() {
	pos--;
}

public int read () throws IOException {
	if (pos < size || refillBuffer() > 0) {
		return (inbuf[pos++]);
	}
	return (-1);
}

public int read ( char cbuf[], int off, int len ) throws IOException {
	synchronized(lock) {
		// Do read directly in this case, according to JDK 1.2 docs
		if (pos == size && !markset && len >= inbuf.length) {
			return rd.read(cbuf, off, len);
		}
		int nread;	
		int chunk;
		for (nread = 0; nread < len; nread += chunk) {
			// Make sure there's something in the buffer
			if (pos == size) {
				// Avoid unneccesary blocking
				if (nread > 0 && !rd.ready()) {
					return nread;
				}
				// Try to fill buffer
				if (refillBuffer() <= 0) {
					return (nread > 0) ? nread : -1;
				}
			}

			// Copy next chunk of chars from buffer to output array
			chunk = len - nread;
			if (chunk > size) {
				chunk = size;
			}
			System.arraycopy(inbuf, pos, cbuf, off, chunk);
			pos += chunk;
			off += chunk;
		}
		return (nread);
	}
}

public String readLine () throws IOException {
	int     start = pos;
	char    c = ' ';    // silly javac, complains about initialization
	String  s = null;

	synchronized ( lock ) {
		while ( true ) {

			// Find next newline or carriage return
			while (pos < size && (c = inbuf[pos]) != '\n' && c != '\r') {
				pos++;
			}

			// Did we see one?
			if (pos == size) {      // nothing found yet
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
		return (pos < size || rd.ready());
	}
}

private int refillBuffer () throws IOException {
	synchronized ( lock ) {
		int n;

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
		pos = 0;

	}
}

public long skip(long n) throws IOException {
        synchronized(lock) {
		long bufskip;

		// Skip from within the buffer first
		bufskip = size - pos;
		if (bufskip > n) {
			bufskip = n;
		}
		pos += (int)bufskip;            // cast is OK

		// Anything more to skip?
		if (bufskip == n) {
			return n;
		}

		// Reset buffer and skip what remains directly
		pos = 0;
		size = 0;
		markset = false;
		return bufskip + rd.skip(n - bufskip);
	}
}
}
