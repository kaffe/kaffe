/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */


package java.io;


/* A shared pipe like UNIX.. Implements a circular buffer in this class, used by PipedOutputStream to store */
public class PipedInputStream
  extends InputStream
{
	private PipedOutputStream src = null;
	final protected static int PIPE_SIZE = 512;
	protected byte[] pipe = new byte[PIPE_SIZE];
	protected int out = 0;
	protected int in = 0;
	private boolean closed = true;

public PipedInputStream () {
}

/* Public interface */
public PipedInputStream (PipedOutputStream src) throws IOException {
	connect(src);
}

public void close() throws IOException {
	out = 0;
	in = 0;
	closed = true;
}

public void connect(PipedOutputStream src) throws IOException {
	if (this.src == null) {
		this.src = src;
		closed = false;
		src.connect(this);
	}
}

public synchronized int read() throws IOException {
	while (out == in) {
		if (closed) {
			return (-1);
		}
		try {
			this.wait();
		}
		catch (InterruptedException e) {}
	}

	/* Should be Ok now */
	byte result = pipe[out];
	out = (out + 1) % PIPE_SIZE;

	this.notifyAll();

	return ((int)result) & 0xFF;
}

public synchronized int read(byte b[], int off, int len) throws IOException {
	return super.read(b, off, len);
}

protected synchronized void receive(int b) {
	while (out == in+1) {
		try {
			this.wait();
		}
		catch (InterruptedException e) {}
	}
	pipe[in] = (byte)b;
	in = (in + 1) % PIPE_SIZE;

	this.notifyAll();
}

protected void receivedLast() {
	closed = true;
}
}
