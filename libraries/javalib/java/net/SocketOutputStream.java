package java.net;

import java.io.OutputStream;
import java.io.IOException;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
class SocketOutputStream
  extends OutputStream {

private SocketImpl impl;
private byte[] buf = new byte[1];

public SocketOutputStream(SocketImpl impl) {
	this.impl = impl;
}

public void write(byte b[]) throws IOException {
	write(b, 0, b.length);
}

public void write(byte b[], int off, int len) throws IOException {
	impl.write(b, off, len);
}

public void write(int b) throws IOException {
	buf[0] = (byte)b;
	write(buf);
}

public void close() throws IOException {
	impl.close();
}

}
