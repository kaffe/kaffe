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

import kaffe.io.ByteToCharConverter;

public class InputStreamReader
  extends Reader
{
	private static final int BUFDEFAULT = 128;
	private ByteToCharConverter encoding;
	private InputStream strm;
	private byte[] inbuf = new byte[BUFDEFAULT];

public InputStreamReader(InputStream in) {
	super(in);
	strm = in;
	encoding = ByteToCharConverter.getDefault();
}

public InputStreamReader(InputStream in, String enc) throws UnsupportedEncodingException {
	super(in);
	strm = in;
	encoding = ByteToCharConverter.getConverter(enc);
}

/* Internal function used to check whether the
   InputStreamReader has been closed already, throws
   an IOException in that case.
*/
private void checkIfStillOpen() throws IOException {
	if (strm == null) {
		throw new IOException("Stream closed");
	}
}

public void close() throws IOException {
	synchronized(lock) {
		if (strm != null) {
			strm.close();
			strm = null;
			inbuf = null;
			encoding = null;
		}
	}
}

public String getEncoding() {
	synchronized(lock) {
		return encoding == null ? null : encoding.toString();
	}
}

public int read() throws IOException {
        return super.read();
}

public int read ( char cbuf[], int off, int len ) throws IOException {
	if (len < 0 || off < 0 || off + len > cbuf.length) {
		throw new IndexOutOfBoundsException();
	}

	int outlen = 0;
	boolean seenEOF = false;

	synchronized ( lock ) {
		checkIfStillOpen();

		while (len > outlen) {
			// First we retreive anything left in the converter
			final int inpos = encoding.withdraw(inbuf, 0, inbuf.length);
			int n = len - outlen;
			final int m = inbuf.length - inpos;
			if (n > m) {
				n = m;
			}
			int inlen = strm.read(inbuf, inpos, n);
			if (inlen < 0) {
				inlen = 0;
				seenEOF = true;
			}
			outlen += encoding.convert(inbuf, 0, inpos+inlen, cbuf, off+outlen, len-outlen);
			if (inlen < n || !ready()) {
				break;
			}
		}
		if (seenEOF && !encoding.havePending()) {
			return (-1);
		}
		return (outlen);
	}
}

public boolean ready() throws IOException {
	synchronized (lock) {
		checkIfStillOpen();

		return encoding.havePending() || (strm.available() > 0);
	}
}
}
