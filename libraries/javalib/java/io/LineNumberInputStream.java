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
/*
 * @deprecated
 */
public class LineNumberInputStream
  extends FilterInputStream
{
	private int lineNo;
	private boolean skipNextLF;
	private int markLineNo;
	private boolean markSkipNextLF;

public LineNumberInputStream(InputStream in) {
	super(in);
}

public int available() throws IOException {
	/* get the number of available bytes in input */
	int avail = super.available();

	/* if next '\n' needs to be skipped,
	 * substract one from the number of available bytes
	 */
	return skipNextLF ? Math.max(avail - 1, 0) : avail;
}

public int getLineNumber() {
	return lineNo;
}

public void mark(int readlimit) {
	super.mark(readlimit);

	/* store the state information for reset */
	markLineNo=lineNo;
	markSkipNextLF = skipNextLF;
}

public int read() throws IOException {
	int chr=super.read();

	/* skip next '\n' if necessary */
	if (skipNextLF) {
		skipNextLF = false;
		if (chr == '\n') {
			chr = super.read();
		}
	}

	/* '\r' might be followed by '\n'
	 * so set skipNextLF to true.
	 * convert '\r' to '\n'
	 */
	if (chr=='\r') {
		skipNextLF = true;
		chr = '\n';
	}

	/* increase line number if necessary */
	if (chr=='\n') {
		lineNo++;
	}

	return chr;
}

public int read(byte b[], int off, int len) throws IOException {
	if (off < 0 || len < 0 || off + len > b.length) {
	   throw new IndexOutOfBoundsException();
	}

	final int limit = off + len;

	for (int pos=off; pos < limit; ++pos) {
		final int data=read();
		if (data==-1) {
			if (pos-off==0) return -1; else return pos-off;
		}
		b[pos]=(byte )data;
	}

	return len;
}

public void reset() throws IOException {
	super.reset();

	/* reset state information */
	lineNo=markLineNo;
	skipNextLF = markSkipNextLF;
}

public void setLineNumber(int lineNumber) {
	lineNo=lineNumber;
}

public long skip(long n) throws IOException {
	for (long i = 0; i < n; ++i) {
		final int ch = read();
		if (ch == -1) {
			if (i == 0) {
				return -1;
			}
			else {
				return i;
			}
		}
	}

	return n;
}
}
