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
public class ByteArrayInputStream
  extends InputStream
{
	protected byte[] buf;
	protected int pos;
	protected int count;
	protected int mark;

public ByteArrayInputStream(byte buffer[])
	{
	this(buffer, 0, buffer.length);
}

public ByteArrayInputStream(byte buffer[], int offset, int length)
	{
	count = Math.min(offset + length, buffer.length);
	pos = offset;
	mark = offset;
	buf = buffer;
}

public synchronized int available()
	{
	return (count-pos);
}

public synchronized void mark(int readaheadlimit)
	{
	// ignore readaheadlimit because no matter how many bytes are
	// read, we can always resupply them since we read from a finite
	// buffer
	mark = pos;
}

public boolean markSupported()
	{
	return (true);
}

public synchronized int read()
	{
	if (available() == 0) {
		return (-1);
	}
	return (((int)buf[pos++]) & 0xFF);
}

public synchronized int read(byte b[], int offset, int len)
	{
	if (offset < 0 || len < 0 || offset + len > b.length) {
	   throw new IndexOutOfBoundsException();
	}

	if (available() == 0) {
		return (-1);
	}

	final int toRead = Math.min(available(), len);
	System.arraycopy(buf, pos, b, offset, toRead);
	pos += toRead;

	return (toRead);
}

public synchronized void reset()
	{
	pos = mark;
}

public synchronized long skip(long n)
	{
	final long toSkip = Math.min(n, available());
	pos += (int)toSkip;

	return (toSkip);
}
}
