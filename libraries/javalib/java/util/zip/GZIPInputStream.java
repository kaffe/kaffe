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

protected CRC32 crc;
protected boolean eos;
private InputStream strm;

public static final int GZIP_MAGIC		= 0x1f8b;
static final int OLD_GZIP_MAGIC		= 0x1f9e;

static final int GZIP_FLAG_ASCII_FLAG	= 0x01;	// probably ascii text
static final int GZIP_FLAG_CONTINUATION	= 0x02;	// continuation of
							//  multi-part gzip file
static final int GZIP_FLAG_EXTRA_FIELD	= 0x04;	// extra field present
static final int GZIP_FLAG_ORIG_NAME	= 0x08;	// file name present
static final int GZIP_FLAG_COMMENT	= 0x10;	// file comment present
static final int GZIP_FLAG_ENCRYPTED	= 0x20;	// file is encrypted
static final int GZIP_FLAG_RESERVED	= 0xc0;	// must be zero

public GZIPInputStream(InputStream in) throws IOException {
	this(in, 512);
}

public GZIPInputStream(InputStream in, int readsize) throws IOException {
	super(in, new Inflater(true), readsize);
	strm = in;
	crc = new CRC32();
	eos = false;

        /* Check GZIP header */
        checkBytes(new int[] { GZIP_MAGIC >> 8, OLD_GZIP_MAGIC >> 8 });
        checkBytes(new int[] { GZIP_MAGIC & 0xff, OLD_GZIP_MAGIC & 0xff });
        checkBytes(new int[] { Deflater.DEFLATED });

	/* Get flags byte */
	int flags = checkBytes(null);

	/* Skip time stamp, extra flags, and O/S fields */
        ignoreBytes(6);

	/* Check flags */
	if ((flags & GZIP_FLAG_CONTINUATION) != 0) {	// skip multi-part #
		ignoreBytes(2);
	}
	if ((flags & GZIP_FLAG_EXTRA_FIELD) != 0) {	// skip extra fields
		int len = strm.read();
		len |= strm.read() << 8;
		ignoreBytes(len);
	}
	if ((flags & GZIP_FLAG_ORIG_NAME) != 0) {	// skip name
		ignoreString();
	}
	if ((flags & GZIP_FLAG_COMMENT) != 0) {		// skip comment
		ignoreString();
	}
}

private int checkBytes(int[] v) throws IOException {
	int i, x = strm.read();
	if (v != null) {
		for (i = 0; i < v.length && x != v[i]; i++)
			;
		if (i == v.length) {
			throw new IOException("invalid GZIP stream");
		}
	}
	return x;
}

private void ignoreBytes(int nr) throws IOException {
	for (; nr > 0; nr--) {
		strm.read();
	}
}

private void ignoreString() throws IOException {
	int x;
	while ((x = strm.read()) != 0) {
		if (x == -1) {
			throw new IOException("premature GZIP end-of-file");
		}
	}
}

public void close() throws IOException {
	super.close();
}

public int read(byte[] buf, int off, int len) throws IOException {
	int total = super.read(buf, off, len);
	if (total > 0) {
		crc.update(buf, off, total);
	}
	else if (total == -1) {
		if (eos == false) {
			eos = true;
			// Skip the tail on the GZIP
			ignoreBytes(8);
		}
	}
	return (total);
}

}
