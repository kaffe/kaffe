package java.io;

import java.lang.String;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class StringBufferInputStream
  extends InputStream
{
	protected String buffer;
	protected int pos;
	protected int count;

public StringBufferInputStream(String s) {
	buffer=s;
	count=s.length();
	reset();
}

public synchronized int available() {
	return count-pos;
}

public synchronized int read() {
	if (pos<count) {
		return (int )(buffer.charAt(pos++) & 0xFF);
	}
	else {
		return -1;
	}
}

public synchronized int read(byte b[], int off, int len) {
	for (int pos=off; pos<off+len; pos++) {
		int data=read();
		if (data==-1) {
			if (pos-off==0) return -1; else return pos-off;
		}
		b[pos]=(byte )data;
	}
	return len;
}

public synchronized void reset() {
	pos=0;
}

public synchronized long skip(long n) {
	long skipped=Math.min(available(), n);

	pos=pos+(int )skipped;

	return skipped;
}
}
