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
import java.io.FilterInputStream;
import java.io.IOException;

public class CheckedInputStream extends FilterInputStream {

private Checksum cksum;

public CheckedInputStream(InputStream in, Checksum sum)
{
	super(in);
	cksum = sum;
}

public Checksum getChecksum()
{
	return (cksum);
}

public int read() throws IOException
{
	int bval = in.read();
	if (bval == -1) {
		return (-1);
	}
	cksum.update(bval);
	return (bval);
}

public int read(byte[] buf, int off, int len) throws IOException
{
	int total = in.read(buf, off, len);
	if (total > 0) {
		cksum.update(buf, off, total);
	}
	return (total);
}

}
