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
public class FilterInputStream
  extends InputStream {

	protected InputStream in;

protected FilterInputStream(InputStream in) {
	this.in = in;
}

public int available() throws IOException {
	try {
		return (in.available());
	}
	catch (NullPointerException _) {
		return (0);
	}
}

public void close() throws IOException {
	try {
		in.close();
	}
	catch (NullPointerException _) {
	}
}

public synchronized void mark(int readlimit) {
	try {
		in.mark(readlimit);
	}
	catch (NullPointerException _) {
	}
}

public boolean markSupported() {
	try {
		return (in.markSupported());
	}
	catch (NullPointerException _) {
		return (false);
	}
}

public int read() throws IOException {
	try {
		return (in.read());
	}
	catch (NullPointerException _) {
		throw new EOFException("null stream");
	}
}

public int read(byte b[]) throws IOException {
	return (read(b, 0, b.length));
}

public int read(byte b[], int off, int len) throws IOException {
	try {
		return (in.read(b, off, len));
	}
	catch (NullPointerException _) {
		throw new EOFException("null stream");
	}
}

public synchronized void reset() throws IOException {
	try {
		in.reset();
	}
	catch (NullPointerException _) {
		throw new IOException("null stream");
	}
}

public long skip(long n) throws IOException {
	try {
		return (in.skip(n));
	}
	catch (NullPointerException _) {
		throw new EOFException("null stream");
	}
}
}
