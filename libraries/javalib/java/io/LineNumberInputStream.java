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
public class LineNumberInputStream
  extends FilterInputStream
{
	private int lineNo = 0;
	private int markLineNo;
	private boolean pushedBack = false;
	private int pushBack;

public LineNumberInputStream(InputStream in) {
	super(in);
}

public int available() throws IOException {
	int sup=super.available();

	if (pushedBack==true) sup++;

	/* Code according to Sun's SPEC */
	//    return sup/2;

	/* Code according to Sun's code.. thanks again Sun! */
	return sup;
}

public int getLineNumber() {
	return lineNo;
}

public void mark(int readlimit) {
	markLineNo=lineNo;
	super.mark(readlimit);
}

private void pushBack(int chr) {
	pushBack=chr;
	pushedBack=true;
}

private int pushBackRead() throws IOException {
	if (pushedBack) {
		pushedBack=false;
		return pushBack;
	}
	else {
		return super.read();
	}
}

public int read() throws IOException {
	int chr=pushBackRead();

	if ((chr=='\n') || (chr=='\r')) lineNo++;

	if (chr=='\r') {
		/* Read ahead */
		int next=pushBackRead();
		if (next=='\n') chr=next; else pushBack(next);
	}

	//System.out.println("LINENo: "+lineNo);
	return chr;
}

public int read(byte b[], int off, int len) throws IOException {
	for (int pos=off; pos<off+len; pos++) {
		int data=read();
		if (data==-1) {
			if (pos-off==0) return -1; else return pos-off;
		}
		b[pos]=(byte )data;
	}

	return len;
}

public void reset() throws IOException {
	lineNo=markLineNo;
	super.reset();
}

public void setLineNumber(int lineNumber) {
	lineNo=lineNumber;
}

public long skip(long n) throws IOException {
	byte junk[]=new byte[(int )n];

	return (long)read(junk, 0, junk.length);
}
}
