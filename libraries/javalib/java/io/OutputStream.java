package java.io;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
abstract public class OutputStream
{
public void close() throws IOException {
	/* Do nothing */
}

public void flush() throws IOException {
	/* Do nothing */
}

public void write(byte b[]) throws IOException {
	write(b, 0, b.length);
}

public void write(byte b[], int off, int len) throws IOException {
	for (int pos=off; pos<off+len; pos++) {
		write((int )b[pos]);
	}
}

abstract public void write(int b) throws IOException;
}
