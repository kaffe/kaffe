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
	private boolean closed = false;

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
	if (closed == false) {
		flush();
		strm.close();
		strm = null;
		closed = true;
	}
}

public void flush() throws IOException
{
	synchronized (lock) {
		if (closed == true) {
			throw new IOException("stream closed");
		}
		if (buflen > 0) {
			strm.write(outbuf, 0, buflen);
			buflen = 0;
		}
	}
	strm.flush();
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
		if (len > 0) {
			int outlen = encoding.convert(cbuf, off, len,
				outbuf, buflen, outbuf.length - buflen);
			if (outlen == 0) {
				flush();
				outlen = encoding.flush(outbuf, buflen, outbuf.length - buflen);
			}
			while (outlen > 0) {
				buflen += outlen;
				if (outbuf.length - buflen < MINMARGIN) {
					flush();
				}
				outlen = encoding.flush(outbuf, buflen, outbuf.length - buflen);
			}
		}
	}
}

public void write(int c) throws IOException
{
	write (new char[] { (char)c }, 0, 1);
}
}
