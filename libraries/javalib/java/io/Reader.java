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
abstract public class Reader
{
	protected Object lock;
	protected char[] cbuf;

protected Reader()
	{
	lock = this;
}

protected Reader(Object lk)
	{
	lock = lk;
}

abstract public void close() throws IOException;

public void mark(int readAheadLimit) throws IOException
{
	// Stream does not support marking.
}

public boolean markSupported()
	{
	return (false);
}

public int read () throws IOException
{
	// this is just the VERY inefficient generic read(), it should be
	// overridden in almost every subclass
	if ( cbuf == null ) cbuf = new char[1];

	return (read( cbuf, 0, 1) < 0) ? -1 : cbuf[0];
}

public int read(char cbuf[]) throws IOException
{
	return (read(cbuf, 0, cbuf.length));
}

abstract public int read(char cbuf[], int off, int len) throws IOException;

public boolean ready() throws IOException
{
	return (true);
}

public void reset() throws IOException
{
	throw new IOException();
}

public long skip(long n) throws IOException
{
	char[] buf = new char[1024];
	long sk = 0;
	int r = 1;

	while (r > 0) {
		r = read(buf, 0, (int)((long)buf.length < n ? (long)buf.length : n));
		if (r > 0) {
			sk += r;
			n -= r;
		}
	}
	return (sk);
}
}
