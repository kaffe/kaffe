/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util.zip;

import java.io.OutputStream;
import java.io.IOException;
import kaffe.util.NotImplemented;

public class ZipOutputStream extends DeflaterOutputStream implements ZipConstants {

public static final int DEFLATED = 8;
public static final int STORED = 0;

private int method = STORED;
private int level = Deflater.DEFAULT_COMPRESSION;

public ZipOutputStream(OutputStream out)
{
	super(out);
}

public void close() throws IOException
{
	finish();
	super.close();
}

public void closeEntry() throws IOException
{
	throw new NotImplemented();
}

public void finish() throws IOException
{
	throw new NotImplemented();
}

public void putNextEntry(ZipEntry ze) throws IOException
{
	throw new NotImplemented();
}

public void setComment(String comment)
{
	// Currently ignored.
}

public void setLevel(int lvl)
{
	if ((lvl < Deflater.NO_COMPRESSION || lvl > Deflater.BEST_COMPRESSION) && lvl != Deflater.DEFAULT_COMPRESSION) {
		throw new IllegalArgumentException("bad compression level");
	}
	level = lvl;
}

public void setMethod(int m)
{
	if (m != DEFLATED && m != STORED) {
		throw new IllegalArgumentException("bad compression method");
	}
	method = m;
}

public synchronized void write(byte[] buf, int off, int len) throws IOException
{
	throw new NotImplemented();
}

}
