package java.net;

import java.io.FileInputStream;
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
class SocketInputStream
  extends FileInputStream
{
	boolean eof;
/* Do I need this? */
	SocketImpl impl;
	byte[] temp;

public SocketInputStream(SocketImpl impl) {
	super(impl.fd);
	this.impl=impl;
}

public int read(byte b[]) throws IOException {
	return read(b, 0, b.length);
}

public int read(byte b[], int off, int length) throws IOException {
	return socketRead(b, off, length);
}

/* I have no idea what this is for */
native private int socketRead(byte buf[], int offset, int len);
}
