
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

import java.security.spec.AlgorithmParameterSpec;

// See MessageDigest for a description of why this extends SignatureSpi
// and the weirdness it causes.

public abstract class Signature extends SignatureSpi {
	static final String ENGINE_CLASS = "Signature";
	protected static final int UNINITIALIZED = 0;
	protected static final int SIGN = 2;
	protected static final int VERIFY = 3;
	protected int state;
	private final String algorithm;
	private Provider provider;

	protected Signature(String algorithm) {
		this.algorithm = algorithm;
	}

	public static Signature getInstance(String algorithm)
			throws NoSuchAlgorithmException {
		return getInstance(Security.getCryptInstance(
			ENGINE_CLASS, algorithm));
	}

	public static Signature getInstance(String algorithm, String provider)
			throws NoSuchAlgorithmException,
				NoSuchProviderException {
		return getInstance(Security.getCryptInstance(
			ENGINE_CLASS, algorithm, provider));
	}

	private static Signature getInstance(Security.Engine e) {
		Signature s = (Signature)e.getEngine();
		s.state = UNINITIALIZED;
		s.provider = e.getProvider();
		return s;
	}

	public final Provider getProvider() {
		return provider;
	}

	public final void initVerify(PublicKey publicKey)
			throws InvalidKeyException {
		/*engine.*/engineInitVerify(publicKey);
	}

	public final void initSign(PrivateKey privateKey)
			throws InvalidKeyException {
		/*engine.*/engineInitSign(privateKey);
	}

	public final void initSign(PrivateKey privateKey, SecureRandom random)
			throws InvalidKeyException {
		/*engine.*/engineInitSign(privateKey, random);
	}

	public final byte[] sign() throws SignatureException {
		return /*engine.*/engineSign();
	}

	public final int sign(byte[] outbuf, int offset, int len)
			throws SignatureException {
		byte[] sig = sign();
		if (len < sig.length) {
			throw new SignatureException();
		}
		System.arraycopy(sig, 0, outbuf, offset, sig.length);
		return sig.length;
	}

	public final boolean verify(byte[] signature)
			throws SignatureException {
		return /*engine.*/engineVerify(signature);
	}

	public final void update(byte b) throws SignatureException {
		/*engine.*/engineUpdate(b);
	}

	public final void update(byte[] data) throws SignatureException {
		/*engine.*/engineUpdate(data, 0, data.length);
	}

	public final void update(byte[] data, int off, int len)
			throws SignatureException {
		/*engine.*/engineUpdate(data, off, len);
	}

	public final String getAlgorithm() {
		return algorithm;
	}

	// XXX what to do here?
	public String toString() {
		return super.toString();
	}

	public final void setParameter(String param, Object value)
			throws InvalidParameterException {
		/*engine.*/engineSetParameter(param, value);
	}

	public final void setParameter(AlgorithmParameterSpec params)
		throws InvalidAlgorithmParameterException {
		/*engine.*/engineSetParameter(params);
	}

	public final Object getParameter(String param)
			throws InvalidParameterException {
		return /*engine.*/engineGetParameter(param);
	}

	public Object clone() throws CloneNotSupportedException {
		throw new CloneNotSupportedException();
	}
}

