
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.security;

import java.io.IOException;
import java.io.FilterOutputStream;
import java.io.OutputStream;

public class DigestOutputStream extends FilterOutputStream {
	protected MessageDigest digest;
	private boolean on;

	public DigestOutputStream(OutputStream out, MessageDigest digest) {
		super(out);
		this.digest = digest;
		this.on = true;
	}

	public MessageDigest getMessageDigest() {
		return digest;
	}

	public void setMessageDigest(MessageDigest digest) {
		this.digest = digest;
	}

	public void write(int b) throws IOException {
		out.write(b);
		if (on) {
			digest.update((byte)b);
		}
	}

	public void write(byte[] b, int off, int len) throws IOException {
		out.write(b, off, len);
		if (on) {
			digest.update(b, off, len);
		}
	}

	public void on(boolean on) {
		this.on = on;
	}

	public String toString() {
		return "DigestOutputStream(" + out.toString()
			+ ", " + digest.toString() + ")";
	}
}

