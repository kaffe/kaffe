/*
 * kaffe.io.AccessibleBAOStream - accessible ByteArrayOutputStream
 *
 * The sole reason ot this class is to provide access (to set or get to
 * the stream buffer. There are a number of cases where data has to be
 * stored "temporarily" in memory, to be later processed as a byte array.
 * Some of these cases can produce large data chunks (e.g. image processing),
 * and copying these chunks by means of toByteArray() (just to pass on the
 * data) can simply consume too much memory.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */

package kaffe.io;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class AccessibleBAOStream
  extends ByteArrayOutputStream
{
public AccessibleBAOStream ( byte[] buf ) {
	this.buf = buf;
}

public AccessibleBAOStream ( byte[] buf, int offset ) {
	this.buf = buf;
	this.count = offset;
	if ( offset > buf.length ) {
		throw new IllegalArgumentException( "offset too large");
	}
}

public AccessibleBAOStream ( int size ) {
	super( size);
}

public byte[] getBuffer() {
	return buf;
}

public void readFrom(InputStream in) {
	int remain;
	int r;

	for (;;) {
		remain = buf.length - count;
		if (remain <= 0) {
			byte oldBuf[] = buf;
			buf = new byte[oldBuf.length + 1024];
			System.arraycopy(oldBuf, 0, buf, 0, oldBuf.length);
			remain = buf.length - count;
		}
		if (remain > 1024) {
			remain = 1024;
		}
		try {
			r = in.read(buf, count, remain);
		}
		catch (IOException _) {
			break;
		}
		if (r <= 0) {
			break;
		}
		count += r;
	}
}

}
