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

import java.io.InputStream;
import java.io.IOException;

public class GZIPInputStream extends InflaterInputStream {

public final static int GZIP_MAGIC = 0x8b1f;

protected CRC32 crc;
protected boolean eos;

public GZIPInputStream(InputStream in) throws IOException
{
	this(in, 512);
}

public GZIPInputStream(InputStream in, int readsize) throws IOException
{
	super(in, new Inflater(), readsize);
	crc = new CRC32();
	eos = false;
}

public void close() throws IOException
{
	super.close();
}

public int read(byte[] buf, int off, int len) throws IOException
{
	int total = super.read(buf, off, len);
	if (total > 0) {
		crc.update(buf, off, total);
	}
	else if (total == -1) {
		eos = true;
	}
	return (total);
}

}
