
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


public final class SHA extends UpdateDigest {
	public static final String DIGEST_NAME = "SHA-1";
	public static final int DIGEST_LENGTH = 20;

	public SHA() {
		super(DIGEST_NAME);
	}

	protected int engineGetDigestLength() {
		return DIGEST_LENGTH;
	}

	protected native void Init();
	protected native void Update(byte[] input, int offset, int len);
	protected native void Final(byte[] digest, int offset);
}

