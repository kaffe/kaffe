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
import java.lang.String;

public class InputStreamReader
  extends Reader
{
	final private static int BUFDEFAULT = 1024;
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
	strm.close();
}

public String getEncoding() {
	return (encoding.toString());
}

public int read ( char cbuf[], int off, int len ) throws IOException {
	int outlen = 0;
	int olen;

	synchronized ( lock ) {
		// First we return anything left in the converter
		outlen = encoding.flush(cbuf, off, len);
		while (len > outlen) {
			int n = len - outlen;
			if (n > inbuf.length) {
				n = inbuf.length;
			}
			int inlen = strm.read(inbuf, 0, n);
			if (inlen < 0) {
				break;
			}
			outlen += encoding.convert(inbuf, 0, inlen, cbuf, off+outlen, len-outlen);
			if (inlen < n) {
				break;
			}
		}
		if (outlen <= 0) {
			return (-1);
		}
		return (outlen);
	}
}

public boolean ready() throws IOException {
	return (strm.available() > 0 ? true : false);
}
}
