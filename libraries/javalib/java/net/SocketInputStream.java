/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.net;

import java.io.InputStream;
import java.io.IOException;

class SocketInputStream
  extends InputStream {

private SocketImpl impl;
private byte[] buf;

public SocketInputStream(SocketImpl impl) {
	this.impl = impl;
}

public int read(byte b[]) throws IOException {
	return (read(b, 0, b.length));
}

public int read(byte b[], int off, int length) throws IOException {
	synchronized (this) {
		return (impl.read(b, off, length));
	}
}

public int read() throws IOException {
	synchronized (this) {
		if (buf == null) {
			buf = new byte[1];
		}
		if (impl.read(buf, 0, 1) == 1) {
			return (buf[0] & 0xFF);
		}
		return (-1);
	}
}

public int available() throws IOException {
	return (impl.available());
}

public void close() throws IOException {
	impl.close();
}

}
