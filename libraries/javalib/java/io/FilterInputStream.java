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
	return (in.available());
}

public void close() throws IOException {
	if (in != null) {
		in.close();
		in = null;
	}
}

public synchronized void mark(int readlimit) {
	in.mark(readlimit);
}

public boolean markSupported() {
	return (in.markSupported());
}

public int read() throws IOException {
	return (in.read());
}

public int read(byte b[]) throws IOException {
    /* BufferedInputStream depends on this method
     * *not* reading directly from the stream.
     */
	return (read(b, 0, b.length));
}

public int read(byte b[], int off, int len) throws IOException {
	return (in.read(b, off, len));
}

public synchronized void reset() throws IOException {
	in.reset();
}

public long skip(long n) throws IOException {
	return (in.skip(n));
}
}
