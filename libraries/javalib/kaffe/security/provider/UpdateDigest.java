
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

package kaffe.security.provider;

import java.security.DigestException;
import java.security.MessageDigest;

// See java.security.MessageDigest for explanation of why this class
// extends MessageDigest instead of MessageDigestSpi.

public abstract class UpdateDigest extends MessageDigest/*Spi*/ {
	private byte[] context;

	protected UpdateDigest(String algorithm) {
		super(algorithm);
		engineReset();
	}

	protected void engineUpdate(byte input) {
		Update(new byte[] { input }, 0, 1);
	}

	protected void engineUpdate(byte[] input, int offset, int len) {
		Update(input, offset, len);
	}

	protected byte[] engineDigest() {
		byte[] digest = new byte[engineGetDigestLength()];
		Final(digest, 0);
		engineReset();
		return digest;
	}

	protected int engineDigest(byte[] buf, int offset, int len)
			throws DigestException {
		if (len < engineGetDigestLength()) {
			throw new DigestException();
		}
		Final(buf, offset);
		engineReset();
		return engineGetDigestLength();
	}

	protected void engineReset() {
		Init();
	}

	public Object clone() throws CloneNotSupportedException {
		UpdateDigest d = (UpdateDigest)super.clone();
		d.context = new byte[context.length];
		System.arraycopy(context, 0, d.context, 0, context.length);
		return d;
	}

	// Initialize the context
	protected abstract void Init();

	// Update the context with new data
	protected abstract void Update(byte[] input, int offset, int len);

	// Finalize context and generate digest
	protected abstract void Final(byte[] digest, int offset);
}


