
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

//import java.security.spec;

// See MessageDigest for a description of why this extends KeyPairGeneratorSpi
// and the weirdness it causes.

public abstract class KeyPairGenerator extends KeyPairGeneratorSpi {
	static final String ENGINE_CLASS = "KeyPairGenerator";
	private final String algorithm;
	private Provider provider;

	protected KeyPairGenerator(String algorithm) {
		this.algorithm = algorithm;
	}

	public String getAlgorithm() {
		return algorithm;
	}

	public static KeyPairGenerator getInstance(String alg)
			throws NoSuchAlgorithmException {
		return getInstance(Security.getCryptInstance(
			ENGINE_CLASS, alg));
	}

	public static KeyPairGenerator getInstance(String alg, String provider)
			throws NoSuchAlgorithmException,
				NoSuchProviderException {
		return getInstance(Security.getCryptInstance(
			ENGINE_CLASS, alg, provider));
	}

	private static KeyPairGenerator getInstance(Security.Engine e) {
		KeyPairGenerator k = (KeyPairGenerator)e.engine;  // weirdness
		k.provider = e.provider;
		return k;
	}

	public final Provider getProvider() {
		return provider;
	}

	public void initialize(int keysize) {
		initialize(keysize, new SecureRandom());
	}

	// this class must be overridden by provider's class
	public void initialize(int keysize, SecureRandom random) {
		throw new UnsupportedOperationException();
	}

/***********
	public void initialize(AlgorithmParameterSpec params)
			throws InvalidAlgorithmParameterException {
		initialize(params, new SecureRandom());
	}

	// this class must be overridden by provider's class
	public void initialize(AlgorithmParameterSpec params, SecureRandom r)
		throw new UnsupportedOperationException();
	}
************/

	public final KeyPair genKeyPair() {
		return /*engine.*/generateKeyPair();
	}
}

