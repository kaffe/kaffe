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

public class LineNumberReader
  extends BufferedReader
{
	private int lineno = 0;
	private int marklineno = 0;

public LineNumberReader(Reader in) {
	super(in);
}

public LineNumberReader(Reader in, int sz) {
	super(in, sz);
}

public int getLineNumber() {
	return (lineno);
}

public void mark(int readAheadLimit) throws IOException {
	synchronized(lock) {
		super.mark(readAheadLimit);
		marklineno = lineno;
	}
}

public int read() throws IOException {
	int ch;

	synchronized(lock) {
		ch = super.read();
		if (ch == -1) {
			return (-1);
		}
		if (ch == '\n') {
			lineno++;
		}
		else if (ch == '\r') {
			lineno++;
			// Handle \r\n -> \n compression
			ch = super.read();
			if (ch != '\n') {
				super.pushback();
				ch = '\n';
			}
		}
	}
	return (ch);
}

public int read ( char cbuf[], int off, int len ) throws IOException {
	int i, n, m;
	
	synchronized ( lock ) {
		n = super.read( cbuf, off, len);
		m = off+n;
		for ( i=off; i<m; i++ ){
			if ( cbuf[i] == '\n' ) lineno++;
		}
	}
	
	return n;
}

public String readLine() throws IOException {
	synchronized(lock) {
		String str = super.readLine();
		lineno++;
		return (str);
	}
}

public void reset() throws IOException {
	synchronized(lock) {
		super.reset();
		lineno = marklineno;
	}
}

public void setLineNumber(int lineNumber) {
	synchronized(lock) {
		lineno = lineNumber;
	}
}

public long skip(long n) throws IOException {
	long i;

	synchronized(lock) {

		// Hidiously slow ....
		for (i = 0; i < n; i++) {
			int ch = read();
			if (ch == -1) {
				if (i == 0) {
					i = -1;
				}
				break;
			}
		}

	}
	return (i);
}
}
