
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

public abstract class SignatureSpi {
	protected SecureRandom appRandom;

	public SignatureSpi() {
	}

	protected abstract void engineInitVerify(PublicKey publicKey)
			throws InvalidKeyException;

	protected abstract void engineInitSign(PrivateKey privateKey)
			throws InvalidKeyException;

	// non-abstract for backwards compatibility
	protected void engineInitSign(PrivateKey privateKey, SecureRandom r)
			throws InvalidKeyException {
		throw new UnsupportedOperationException();
	}

	protected abstract void engineUpdate(byte b)
			throws SignatureException;

	protected abstract void engineUpdate(byte[] b, int off, int len)
			throws SignatureException;

	protected abstract byte[] engineSign() throws SignatureException;

	// non-abstract for backwards compatibility
	protected int engineSign(byte[] outbuf, int offset, int len)
			throws SignatureException {
		throw new UnsupportedOperationException();
	}

	protected abstract boolean engineVerify(byte[] sigBytes)
			throws SignatureException;

	protected abstract void engineSetParameter(String param, Object value)
			throws InvalidParameterException;

/******
	// non-abstract for backwards compatibility
	protected void engineSetParameter(AlgorithmParameterSpec params)
			throws InvalidAlgorithmParameterException {
		throw new UnsupportedOperationException();
	}
*******/

	protected abstract Object engineGetParameter(String param)
			throws InvalidParameterException;

	public Object clone() throws CloneNotSupportedException {
		throw new CloneNotSupportedException();
	}
}

