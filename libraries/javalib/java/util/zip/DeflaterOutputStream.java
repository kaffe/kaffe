package java.util.zip;

import java.io.FilterOutputStream;
import java.io.IOException;
import java.io.OutputStream;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class DeflaterOutputStream
  extends FilterOutputStream
{
	final private static int DEFAULT = 512;
	protected Deflater def;
	protected byte[] buf;

public DeflaterOutputStream(OutputStream out) {
	this(out, new Deflater(), DEFAULT);
}

public DeflaterOutputStream(OutputStream out, Deflater defx) {
	this(out, defx, DEFAULT);
}

public DeflaterOutputStream(OutputStream out, Deflater defx, int size) {
	super(out);
	if (out == null)
	  throw new NullPointerException("out");
	if (defx == null)
	  throw new NullPointerException("def");
	def = defx;
	if (size < 1)
	  throw new IllegalArgumentException("size < 1");
	buf = new byte[size];
}

public void close() throws IOException {
	finish();
	super.close();
}

protected void deflate() throws IOException {
	do {
		int r = def.deflate(buf, 0, buf.length);
		out.write(buf, 0, r);
	} while (!def.needsInput());
}

public void finish() throws IOException {
	def.finish();
	while (!def.finished()) {
		deflate();
	}
}

public void write(byte b[], int off, int len) throws IOException {
	if (off < 0 || len < 0 || off + len > b.length)
		throw new IndexOutOfBoundsException();
	
	if(len != 0) {
		def.setInput(b, off, len);
		deflate();
	}
}

public void write(int b) throws IOException {
	byte a[] = new byte[1];
	a[0] = (byte)b;
	write(a, 0, 1);
}

}
