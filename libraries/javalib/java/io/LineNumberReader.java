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


public class LineNumberReader extends BufferedReader {
	private int     lineno;
	private int     marklineno;
	private boolean skipnextlf;
	private boolean markskipnextlf;

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

		// Store this LineNumberReader's state.
		marklineno = lineno;
		markskipnextlf = skipnextlf;
	}
}

public int read() throws IOException {
	int ch;

	synchronized(lock) {
		ch = super.read();
		switch (ch) {

		case -1 :
		        // Do nothing on EOF.
		        break;

		case '\n': 
		        // Skip over this line feed if
		        // the previous character was a
		        // carriage return.
		        if (skipnextlf) {
			        skipnextlf = false;

				// Return next character. read()
				// automatically takes care
				// of line numbers and flags.
				ch = read();
			}
			else {
				lineno++;
			}
			break;

		case '\r':
		        // read() compresses \r and \r\n to \n.

		        // If next character to be read is a \n,
		        // skip over it. It belongs to this \r.
		        skipnextlf = true;

			// Handle compression.
			ch = '\n';

		        lineno++;			
			break;

		default:
		        // Normal character. Next \n doesn't need to
		        // be skipped over.
		        skipnextlf = false;
		}
	}
	return (ch);
}

public int read ( char [] cbuf, int off, int len ) throws IOException {
	int i, n, m;
	
	synchronized ( lock ) {

		n = super.read( cbuf, off, len);

		// Count lines and update flags.
		m = off+n;
		for ( i=off; i<m; i++ ){
			switch (cbuf[i]) {
			case '\n': 
				// Skip over this line feed if
				// the previous character was a
				// carriage return.
				if (skipnextlf) {
					skipnextlf = false;
				}
				else {
					lineno++;
				}
				break;

		        case '\r':
				// read(char[], int, int) doesn't compress line
				// terminators.

				// If next character to be read is a \n,
				// skip over it. It belongs to this \r.
				skipnextlf = true;

				lineno++;			
				break;

			default:
				// Normal character. Next \n doesn't need to
				// be skipped over.
				skipnextlf = false;
			}
		}
	}
	
	return n;
}

public String readLine() throws IOException {
	StringBuffer line = new StringBuffer();

	synchronized(lock) {
		if (skipnextlf) {
			/* If we have to skip next \n, then first
			   read a single char to see if it is a \n.
			   We can't use read() for that, since it 
			   would falsely report a \r for a \n.
			*/
			char [] buf = new char [1];
			if (read(buf) > 0) {
				switch (buf[0]) {
				case '\n':
					// Skip over \n.
					break;
				case '\r':
					// An empty line.
					return "";
				default:
					/* Normal character. Append it to 
					   the line.
					*/
					line.append(buf);
				}
			}
		}

		int    oldlineno = lineno;
		String str = super.readLine();

		if (str == null) {
			if (line.length() == 0) {
				return null;
			}
		}
		else {
			line.append(str);
		}

		lineno = oldlineno + 1;
	}

	return line.toString();
}

public void reset() throws IOException {
	synchronized(lock) {
		super.reset();
		lineno = marklineno;
		skipnextlf = markskipnextlf;
	}
}

public void setLineNumber(int lineNumber) {
	synchronized(lock) {
		lineno = lineNumber;
	}
}

public long skip(long count) throws IOException {
	if (count < 0) {
		throw new IllegalArgumentException("skip() value is negative");
	}

	long skipped = 0;
	int  buffersize;
	final long DEFAULTSIZE = 1024L;

	// We might have to skip over many characters.
	// If that's true, don't waste memory allocating
	// a huge junk buffer.
	buffersize = (int) Math.min(count, DEFAULTSIZE);

	char [] junk = new char[buffersize];

	synchronized(lock) {
		do {
			// Skip characters.
			int n = read(junk, 0,
			    Math.min(buffersize, (int)(count - skipped)));

			// If we've skipped any characters, add them to
			// the total. Otherwise stop skipping.
			if (n > 0) {
				skipped += n;
			}
			else {
				break;
			}
		} while (count > skipped);
	}

	return (skipped);
}
}


