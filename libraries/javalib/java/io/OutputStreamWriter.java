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
	private final static int BUFDEFAULT = 8192;
	private static final int MINMARGIN = 32;
	private OutputStream strm;
	private CharToByteConverter encoding;
	private byte[] outbuf = new byte[BUFDEFAULT];
	private int buflen;

public OutputStreamWriter(OutputStream out) {
	super(out);
	strm = out;
	encoding = CharToByteConverter.getDefault();
}

public OutputStreamWriter(OutputStream out, String enc) throws UnsupportedEncodingException
{
	super(out);
	strm = out;
	encoding = CharToByteConverter.getConverter(enc);
}

/* Internal function used to check whether the
   OutputStreamWriter has been closed already, throws
   an IOException in that case.
*/
  private void checkIfStillOpen() throws IOException {
    if (strm == null) {
      throw new IOException("Stream closed");
    }
  }

public void close() throws IOException
{
	synchronized(lock) {
		if (strm != null) {
			flush();
			strm.close();
			strm = null;
			encoding = null;
			outbuf = null;
		}
	}
}

public void flush() throws IOException
{
	synchronized (lock) {
		checkIfStillOpen();
		if (buflen > 0) {
			strm.write(outbuf, 0, buflen);
			buflen = 0;
		}
		strm.flush();
	}
}

public String getEncoding() {
	synchronized(lock) {
		return encoding == null ? null : encoding.toString();
	}
}

public void write(String str, int off, int len) throws IOException
{
	if (len < 0 || off < 0 || off + len > str.length()) {
		throw new IndexOutOfBoundsException();
	}

	synchronized(lock) {
		checkIfStillOpen();
		super.write(str, off, len);
	}
}

public void write(char cbuf[], int off, int len) throws IOException
{
	if (len < 0 || off < 0 || off + len > cbuf.length) {
		throw new IndexOutOfBoundsException();
	}

	synchronized (lock) {
		checkIfStillOpen();
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
        super.write (c);
}
}
