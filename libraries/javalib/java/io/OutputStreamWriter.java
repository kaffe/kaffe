package java.io;

import java.lang.String;
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
	private final static int BUFDEFAULT = 1024;
	private final static int MINMARGIN = 32;
	private OutputStream strm;
	private CharToByteConverter encoding;
	private byte[] outbuf = new byte[BUFDEFAULT];
	private int buflen;

public OutputStreamWriter(OutputStream out) {
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
	try {
		strm.close();
		strm = null;
	}
	catch (NullPointerException _) {
		throw new IOException("stream closed");
	}
}

public void flush() throws IOException
{
	try {
		synchronized (lock) {
			if (buflen > 0) {
				strm.write(outbuf, 0, buflen);
				buflen = 0;
			}
		}
		strm.flush();
	}
	catch (NullPointerException _) {
		throw new IOException("stream closed");
	}
}

public String getEncoding() {
	return (encoding.toString());
}

public void write(String str, int off, int len) throws IOException
{
	write(str.toCharArray(), off, len);
}

public void write(char cbuf[], int off, int len) throws IOException
{
	if (strm == null) {
		throw new IOException("stream closed");
	}

	synchronized (lock) {
		while (len > 0) {
			int outlen = encoding.convert(cbuf, off, len,
				outbuf, buflen, outbuf.length - buflen);
			buflen += outlen;
			if (outlen == 0 || outbuf.length - buflen < MINMARGIN) {
				flush();
			}
			off += outlen;
			len -= outlen;
		}
	}
}

public void write(int c) throws IOException
{
	write (new char[] { (char)c }, 0, 1);
}
}
