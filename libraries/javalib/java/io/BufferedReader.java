
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

public class BufferedReader extends Reader {
	private static final int DEFAULTBUFFERSIZE = 8192;
	private Reader rd;
	private char[] inbuf;
	private int pos;	// position of next char in buffer
	private int size;	// total length of valid chars in buffer
				//  invariant: 0 <= pos <= size <= inbuf.length
	private boolean markset;
	private boolean markvalid;

public BufferedReader(Reader in) {
	this(in, DEFAULTBUFFERSIZE);
}

public BufferedReader(Reader in, int sz) {
	super(in);
	if (sz <= 0) {
		throw new IllegalArgumentException("Buffer size <= 0");
	}
	rd = in;
	inbuf = new char[sz];
}

/* Internal function used to check whether the
   BufferedReader has been closed already, throws
   an IOException in that case.
*/
private void checkIfStillOpen() throws IOException {
	if (rd == null) {
		throw new IOException("Stream closed");
	}
}

public void close() throws IOException {
	synchronized(lock) {
		if (rd == null) {
			return;
		}

		// Close the input reader
		rd.close();

		// Release the buffer
		inbuf = null;

		// Release the input reader (lock is a reference too)
		rd = null;
		lock = this;
	}
}

public void mark(int readAheadLimit) throws IOException {
	if (readAheadLimit < 0) {
		throw (new IllegalArgumentException("Read-ahead limit < 0"));
	}

	synchronized(lock) {
		checkIfStillOpen();

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
		markvalid = true;
	}
}

public boolean markSupported() {
	return true;
}

//Used in java.io.BufferedLineReader
void pushback() {
	synchronized (lock) {
		pos--;
	}
}

public int read () throws IOException {
	synchronized (lock) {
		checkIfStillOpen();

		if (pos < size || fillOutBuffer() > 0) {
			return (inbuf[pos++]);
		}
	}
	return (-1);
}

public int read ( char [] cbuf, int off, int len ) throws IOException {
	if (off < 0 || off + len > cbuf.length || len < 0) {
		throw new IndexOutOfBoundsException();
	}
	synchronized(lock) {
		checkIfStillOpen();

		// Do read directly in this case, according to JDK 1.2 docs
		if (pos == size && !markset && len >= inbuf.length) {
			return rd.read(cbuf, off, len);
		}

		int nread;	
		int chunk;
		for (nread = 0; nread < len; nread += chunk) {
			// Make sure there's something in the buffer
			if (pos == size) {
				// Avoid unneccessary blocking
				if (nread > 0 && !rd.ready()) {
					return nread;
				}
				// Try to fill buffer
				if (fillOutBuffer() <= 0) {
					return (nread > 0) ? nread : -1;
				}
			}

			// Copy next chunk of chars from buffer to output array
			chunk = len - nread;
			if (chunk > size - pos) {
				chunk = size - pos;
			}
			System.arraycopy(inbuf, pos, cbuf, off, chunk);
			pos += chunk;
			off += chunk;
		}
		return (nread);
	}
}

public String readLine () throws IOException {
	char    c = ' ';    // silly javac, complains about initialization
	String  s = null;

	synchronized ( lock ) {
		checkIfStillOpen();

		int start = pos;

		while ( true ) {

			// Find next newline or carriage return
			while (pos < size
			    && (c = inbuf[pos]) != '\n' && c != '\r') {
				pos++;
			}

			// Did we see one?
			if (pos == size) {      // nothing found yet
				if (pos > start) {
					if (s == null) {
						s = new String(inbuf,
						    start, pos-start);
					}
					else {
						s += new String(inbuf,
						    start, pos-start);
					}
				}
				if (fillOutBuffer() < 0) {
					return (s);
				}
				start = 0;
			}
			else {			// we got a line terminator
				if ( s == null ) {
					s = new String(inbuf, start, pos-start);
				}
				else {
					s += new String(inbuf,
					    start, pos-start);
				}
				pos++;
				if (c == '\r') {
				        char [] buf = new char [1];
					int     n;
					while ((n = read(buf, 0, 1)) == 0)
					        ;

					if (n == 1
					    && buf[0] != '\n' && pos > 0) {
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
		checkIfStillOpen();

		return (pos < size || rd.ready());
	}
}

// This only gets called when pos == size. It fills as much of the buffer
// beyond position "size" as possible. If no mark is set, we shift "pos"
// back to zero; otherwise, don't shift the buffer unless the mark overflows.

private int fillOutBuffer () throws IOException {
	synchronized ( lock ) {
		int n;

		if (markset) {
			if (pos == inbuf.length) {	// mark overflow
				markvalid = false;
				pos = 0;
			}
		} else {
			pos = 0;
		}

		n = rd.read(inbuf, pos, inbuf.length - pos);

		if (n >= 0) {
			size = pos + n;
			return (n);
		}
		else {
			size = pos;
			return (-1);
		}
	}
}

public void reset() throws IOException {
	synchronized(lock) {
		checkIfStillOpen();

		if (!markset) {
			throw new IOException("Stream not marked");
		}
		else if (!markvalid) {
			throw new IOException("Mark invalid");
		}
		pos = 0;
	}
}

public long skip(long n) throws IOException {
	if (n < 0) {
		throw new IllegalArgumentException("skip value is negative");
	}
	synchronized(lock) {
		checkIfStillOpen();

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
		markvalid = false;
		return bufskip + rd.skip(n - bufskip);
	}
}
}
