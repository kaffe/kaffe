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

import java.lang.String;
import kaffe.io.ByteToCharConverter;

public class InputStreamReader
  extends Reader
{
	final private static int BUFDEFAULT = 128;
	private ByteToCharConverter encoding;
	private InputStream strm;
	private byte[] inbuf = new byte[BUFDEFAULT];

public InputStreamReader(InputStream in) {
	strm = in;
	encoding = ByteToCharConverter.getDefault();
}

public InputStreamReader(InputStream in, String enc) throws UnsupportedEncodingException {
	strm = in;
	encoding = ByteToCharConverter.getConverter(enc);
}

public void close() throws IOException {
	if (strm != null) {
		strm.close();
	}
}

public String getEncoding() {
	return (encoding.toString());
}

public int read ( char cbuf[], int off, int len ) throws IOException {
	int outlen = 0;
	boolean seenEOF = false;

	synchronized ( lock ) {
		while (len > outlen) {
			// First we retreive anything left in the converter
			int inpos = encoding.withdraw(inbuf, 0, inbuf.length);
			int n = len - outlen;
			int m = inbuf.length - inpos;
			if (n > m) {
				n = m;
			}
			int inlen = strm.read(inbuf, inpos, n);
			if (inlen < 0) {
				inlen = 0;
				seenEOF = true;
			}
			outlen += encoding.convert(inbuf, 0, inpos+inlen, cbuf, off+outlen, len-outlen);
			if (inlen < n || !encoding.havePending()) {
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
	return (strm.available() > 0 ? true : false);
}
}
