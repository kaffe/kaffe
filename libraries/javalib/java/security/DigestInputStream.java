
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
import java.io.InputStream;
import java.io.FilterInputStream;

public class DigestInputStream extends FilterInputStream {
	protected MessageDigest digest;
	private boolean on;

	public DigestInputStream(InputStream in, MessageDigest digest) {
		super(in);
		this.on = true;
		this.digest = digest;
	}

	public MessageDigest getMessageDigest() {
		return digest;
	}

	public void setMessageDigest(MessageDigest digest) {
		this.digest = digest;
	}

	public int read() throws IOException {
		int b = in.read();
		if (!on || b == -1) {
			return b;
		}
		digest.update((byte)b);
		return b;
	}

	public int read(byte[] b, int off, int len) throws IOException {
		int r = in.read(b, off, len);
		if (!on || r == -1) {
			return r;
		}
		digest.update(b, off, r);
		return r;
	}

	public void on(boolean on) {
		this.on = on;
	}

	public String toString() {
		return "DigestInputStream(" + in.toString()
			+ ", " + digest.toString() + ")";
	}
}

