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

	/* used by DataInputStream handle unreading of characters.
	 * See DataInputStream.readLine() for a description.
	 */
	private static final int BUFFER_EMPTY = -1;
	int buffer = BUFFER_EMPTY;

protected FilterInputStream(InputStream in) {
	this.in = in;
}

public int available() throws IOException {
	return in.available() + ((buffer == BUFFER_EMPTY) ? 0 : 1);
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
	if (buffer == BUFFER_EMPTY) {
		return in.read();
	}
	else {
		final int value = buffer;
		buffer = BUFFER_EMPTY;
		return value;
	}
}

public int read(byte b[]) throws IOException {
    /* BufferedInputStream depends on this method
     * *not* reading directly from the stream.
     */
	if (buffer == BUFFER_EMPTY) {
		return (read(b, 0, b.length));
	}
	else {
		b[0] = (byte) read();
		return read(b, 1, b.length - 1);
	}
}

public int read(byte b[], int off, int len) throws IOException {
	if (buffer == BUFFER_EMPTY) {
		return (in.read(b, off, len));
	}
	else {
		b[off] = (byte) read();
		return in.read(b, off + 1, len - 1);
	}		
}

public synchronized void reset() throws IOException {
	in.reset();
}

public long skip(long n) throws IOException {
	if (buffer == BUFFER_EMPTY) {
		return (in.skip(n));
	}
	else {
		buffer = BUFFER_EMPTY;
		return (in.skip(n - 1));
	}
}
}
