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

public class GZIPOutputStream extends DeflaterOutputStream {

protected CRC32 crc;

public GZIPOutputStream(OutputStream out)
{
        this(out, 512);
}

public GZIPOutputStream(OutputStream out, int readsize)
{
        super(out, new Deflater(), readsize);
	crc = new CRC32();
}

public void close() throws IOException
{
	finish();
	super.close();
}

public void finish() throws IOException
{
	throw new NotImplemented();
}

public void write(byte[] buf, int off, int len) throws IOException
{
	super.write(buf, off, len);
	crc.update(buf, off, len);
}

}
