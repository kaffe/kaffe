package java.net;

import java.io.FileOutputStream;
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
  extends FileOutputStream
{
	private SocketImpl impl;
	private byte[] temp;

public SocketOutputStream(SocketImpl impl) {
	super(impl.fd);
	this.impl=impl;
}

native private void socketWrite(byte buf[], int off, int len);

public void write(byte b[]) throws IOException {
	write(b, 0, b.length);
}

public void write(byte b[], int off, int len) throws IOException {
	socketWrite(b, off, len);
}
}
