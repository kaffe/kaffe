
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

public abstract class MessageDigestSpi {

	public MessageDigestSpi() {
	}

	protected int engineGetDigestLength() {
		return 0;
	}

	protected abstract void engineUpdate(byte input);

	protected abstract void engineUpdate(byte[] input, int offset, int len);

	protected abstract byte[] engineDigest();

	protected int engineDigest(byte[] buf, int offset, int len)
			throws DigestException {
		engineReset();
		return 0;
	}

	protected abstract void engineReset();

	public Object clone() throws CloneNotSupportedException {
		throw new CloneNotSupportedException();
	}
}

