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
public class PushbackInputStream
  extends FilterInputStream
{
	protected byte[] buf;
	protected int pos;

public PushbackInputStream(InputStream in) {
	this(in, 1);
}

public PushbackInputStream(InputStream in, int size) {
	super(in);
	buf = new byte[size];
	pos = buf.length;
}

public int available() throws IOException
{
	return (super.available() + (buf.length - pos));
}

public boolean markSupported() {
	return (false);
}

public int read() throws IOException {
	return (pos < buf.length) ? (buf[pos++] & 0xff) : super.read();
}

public int read(byte cbuf[], int off, int len) throws IOException {
	int cnt = 0;
	while (pos < buf.length && len > 0) {
		cbuf[off++] = buf[pos++];
		len--;
		cnt++;
	}
	if (len > 0) {
		int r = super.read(cbuf, off, len);
		if (r == -1) {
			if (cnt == 0) {
				cnt = -1;
			}
		}
		else {
			cnt += r;
		}
	}
	return (cnt);
}

public void unread(byte cbuf[]) throws IOException {
	unread(cbuf, 0, cbuf.length);
}

public void unread(byte cbuf[], int off, int len) throws IOException {
	if (pos < len) {
		throw new IOException("pushback buffer is full");
	}
	pos -= len;
	System.arraycopy(cbuf, off, buf, pos, len);
}

public void unread(int c) throws IOException {
	if (pos == 0) {
		throw new IOException("pushback buffer is full");
	}
	pos--;
	buf[pos] = (byte)c;
}
}
