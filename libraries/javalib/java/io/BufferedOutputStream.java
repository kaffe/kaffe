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

public void finalize() throws IOException {
	flush();
}

public synchronized void flush() throws IOException {
	super.write(buf, 0, count);
	super.flush();
	count = 0;
}

public synchronized void write(byte b[], int off, int len) throws IOException {
	int pos = off;
	while (pos < off + len) {
		if (count == buf.length) {
			flush();
		}
		buf[count++] = b[pos++];
	}
}

public synchronized void write(int b) throws IOException {
	if (count==buf.length) {
		flush();
	}
	buf[count++] = (byte)b;
}
}
