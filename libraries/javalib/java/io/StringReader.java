package java.io;

import java.lang.String;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class StringReader
  extends Reader
{
	private char[] buf;
	private int pos;
	private int markpos;

public StringReader(String s)
	{
	buf = s.toCharArray();
	pos = 0;
	markpos = 0;
}

public void close()
	{
	// Does nothing.
}

public void mark(int readAheadLimit) throws IOException
{
	markpos = pos;
}

public boolean markSupported()
	{
	return (true);
}

public int read() throws IOException
{
	synchronized(lock) {

		if (pos == buf.length) {
			return (-1);
		}
		else {
			return ((int)buf[pos++]);
		}

	}
}

public int read ( char cbuf[], int off, int len ) throws IOException {
	int i, m = off+len;

	synchronized(lock) {
		for ( i=off; (i < m) && (pos < buf.length); i++, pos++ )
			cbuf[i] = buf[pos];
	
		return (i == off) ? -1 : (i - off);
	}
}

public boolean ready() throws IOException
	{
	return (true);
}

public void reset() throws IOException
{
	pos = markpos;
}

public long skip(long ns) throws IOException
{
	synchronized(lock) {

		long left = (long)(buf.length - pos);
		if (ns < left) {
			left = ns;
		}
		pos += (int)left;
		return (left);

	}
}
}
