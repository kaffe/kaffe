package java.util.zip;

import java.io.FilterInputStream;
import java.io.IOException;
import java.io.InputStream;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class InflaterInputStream
  extends FilterInputStream
{
	final private static int DEFAULT = 512;
	protected Inflater inf;
	protected byte[] buf;
	protected int len;

public InflaterInputStream(InputStream in) {
	this(in, new Inflater(), DEFAULT);
}

public InflaterInputStream(InputStream in, Inflater inf) {
	this(in, inf, DEFAULT);
}

public InflaterInputStream(InputStream in, Inflater inf, int size) {
	super(in);
	if (in == null)
	  throw new NullPointerException("in");
	if (inf == null)
	  throw new NullPointerException("inf");
	this.inf = inf;
	if (size < 1)
	  throw new IllegalArgumentException("size < 1");
	buf = new byte[size];
}

protected void fill() throws IOException {
	len = super.read(buf, 0, buf.length);
}

public int read() throws IOException {
	byte[] b = new byte[1];
	int r = read(b, 0, 1);
	if (r == -1) {
		return (-1);
	}
	else {
		return ((int)b[0]);
	}
}

public int read(byte b[], int off, int lenx) throws IOException {
	if (inf.finished()) {
		return (-1);
	}
	if (inf.needsInput()) {
		fill();
		if (len == -1) {
			return (-1);
		}
		inf.setInput(buf, 0, len);
	}
	try {
		/* we have to call inflate until
		 * it is finished or needs more input
		 */
		int inflated = 0;
		do {
			inflated += inf.inflate(b,
						off + inflated,
						lenx - inflated);
		}
		while (inflated < lenx && !inf.needsInput() && !inf.finished());
 
		/* recurse if only 0 bytes were inflated */
		if (inflated == 0 && lenx != 0) {
		    return read(b, off, lenx);
		}

		return inflated;
	}
	catch (DataFormatException _) {
		throw new IOException("bad data format");
	}
}

public long skip(long n) throws IOException {
	// This is a terribly inefficient way to skip ...
	long cnt;
	byte[] b = new byte[1];

	for (cnt = 0; cnt < n; cnt++) {
		int r = read(b, 0, 1);
		if (r == -1) {
			break;
		}
	}
	return (cnt);
}

}
