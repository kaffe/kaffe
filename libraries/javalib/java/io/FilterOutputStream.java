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
public class FilterOutputStream
  extends OutputStream
{
	protected OutputStream out;

public FilterOutputStream(OutputStream out) {
	this.out = out;
}

public void close() throws IOException {
	flush();
	out.close();
}

public void flush() throws IOException {
	out.flush();
}

public void write(byte b[]) throws IOException {
	write(b, 0, b.length);
}

public void write(byte b[], int off, int len) throws IOException {
	out.write(b, off, len);
}

public void write(int b) throws IOException {
	out.write(b);
}
}
