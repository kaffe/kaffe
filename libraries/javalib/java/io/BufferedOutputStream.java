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
public class BufferedOutputStream
  extends FilterOutputStream
{
	protected byte[] buf;
	protected int count;
	final private static int DEFAULTBUFFER = 2048;

public BufferedOutputStream(OutputStream out) {
	this(out, DEFAULTBUFFER);
}

public BufferedOutputStream(OutputStream out, int size) {
	super(out);
	buf = new byte[size];
	count = 0;
}

private void writeBuffer() throws IOException {
	if (count > 0)
	    out.write(buf, 0, count);
	count = 0;
}

public synchronized void flush() throws IOException {
	writeBuffer();
	out.flush();
}

public synchronized void write(byte b[], int off, int len) throws IOException {
	if (count + len > buf.length) {
		writeBuffer();
		out.write(b, off, len);
		return;
	}
	System.arraycopy(b, off, buf, count, len);
	count += len;
}

public synchronized void write(int b) throws IOException {
	if (count==buf.length) {
		writeBuffer();
	}
	buf[count++] = (byte)b;
}
}
