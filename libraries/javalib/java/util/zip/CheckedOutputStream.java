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

import java.io.FilterOutputStream;
import java.io.OutputStream;
import java.io.IOException;

public class CheckedOutputStream extends FilterOutputStream {

private Checksum cksum;

public CheckedOutputStream(OutputStream out, Checksum sum)
{
	super(out);
	cksum = sum;
}

public Checksum getChecksum()
{
	return (cksum);
}

public void write(int bval) throws IOException
{
	out.write(bval);
	cksum.update(bval);
}

public void write(byte[] buf, int off, int len) throws IOException
{
	out.write(buf, off, len);
	cksum.update(buf, off, len);
}

}
