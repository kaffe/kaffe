package java.io;

import kaffe.io.CharToByteConverter;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class OutputStreamWriter
  extends Writer
{
	final private static int BUFDEFAULT = 1024;
	private OutputStream strm;
	private CharToByteConverter encoding;
	private byte[] outbuf = new byte[BUFDEFAULT];

public OutputStreamWriter(OutputStream out)
	{
	strm = out;
	encoding = CharToByteConverter.getDefault();
}

public OutputStreamWriter(OutputStream out, String enc) throws UnsupportedEncodingException
{
	strm = out;
	encoding = CharToByteConverter.getConverter(enc);
}

public void close() throws IOException
{
	flush();
	strm.close();
}

public void flush() throws IOException
{
	strm.flush();
}

public String getEncoding()
	{
	return (encoding.toString());
}

public void write(String str, int off, int len) throws IOException
{
	write(str.toCharArray(), off, len);
}

public void write ( char cbuf[], int off, int len ) throws IOException
{
	int outlen;

	synchronized(lock) {
		while ( len > 0 ) {
			outlen = encoding.convert( cbuf, off, len, outbuf, 0, outbuf.length);
			strm.write( outbuf, 0, outlen);
			off += outlen;
			len -= outlen;
		}
	}
}

public void write(int c) throws IOException
{
	char ch[] = new char[1];
	ch[0] = (char)c;
	write (ch, 0, 1);
}
}
