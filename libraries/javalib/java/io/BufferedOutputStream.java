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

public class BufferedOutputStream extends FilterOutputStream
{
	private static final int DEFAULTBUFFER = 2048;
	protected byte[] buf;
	protected int count;

public BufferedOutputStream(OutputStream out) {
	this(out, DEFAULTBUFFER);
}

public BufferedOutputStream(OutputStream out, int size) {
	super(out);
	buf = new byte[size];
	count = 0;
}

public synchronized void flush() throws IOException {
	if (count > 0) {
		out.write(buf, 0, count);
		count = 0;
	}
	out.flush();
}

public synchronized void write(byte b[], int off, int len) throws IOException {

	// If no write will be necessary, just copy the new data to the buffer
	if (count + len <= buf.length) {
	    System.arraycopy(b, off, buf, count, len);
	    count += len;
	    return;
	}

	// Otherwise, first write out any old buffered data
	if (count > 0) {
		out.write(buf, 0, count);
		count = 0;
	}

	// Then write out the new data directly, without copying
	out.write(b, off, len);
}

public synchronized void write(int b) throws IOException {
	if (count == buf.length) {
		out.write(buf, 0, buf.length);
		count = 0;
	}
	buf[count++] = (byte)b;
}

}

