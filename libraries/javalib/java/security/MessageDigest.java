
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

import java.util.Arrays;

public abstract class MessageDigest extends MessageDigestSpi {
	private static final String ENGINE_CLASS = "MessageDigest";
	private String algorithm;
	private Provider provider;

	protected MessageDigest(String algorithm) {
		this.algorithm = algorithm;
	}

	public static MessageDigest getInstance(String alg)
			throws NoSuchAlgorithmException {
		Security.Engine e =
			Security.getCryptInstance(ENGINE_CLASS, alg);
		MessageDigest d = (MessageDigest)e.engine;
		d.provider = e.provider;
		d.algorithm = alg;
		return d;
	}

	public static MessageDigest getInstance(String alg, String provider)
		    throws NoSuchAlgorithmException, NoSuchProviderException {
		Security.Engine e =
			Security.getCryptInstance(ENGINE_CLASS, alg, provider);
		MessageDigest d = (MessageDigest)e.engine;
		d.provider = e.provider;
		d.algorithm = alg;
		return d;
	}

	public final Provider getProvider() {
		return provider;
	}

	public void update(byte input) {
		engineUpdate(input);
	}

	public void update(byte[] input, int offset, int len) {
		engineUpdate(input, offset, len);
	}

	public void update(byte[] input) {
		update(input, 0, input.length);
	}

	public byte[] digest() {
		byte[] rtn = engineDigest();
		engineReset();
		return rtn;
	}

	public int digest(byte[] buf, int offset, int len)
			throws DigestException {
		int digestLen = engineGetDigestLength();
		if (len < digestLen) {
			throw new DigestException("buf.length < " + digestLen);
		}
		System.arraycopy(digest(), 0, buf, offset, digestLen);
		return digestLen;
	}

	public byte[] digest(byte[] input) {
		update(input);
		return digest();
	}

	// XXX what should we do here?
	public String toString() {
		return super.toString();
	}

	public static boolean isEqual(byte[] digesta, byte[] digestb) {
		return Arrays.equals(digesta, digestb);
	}

	public void reset() {
		engineReset();
	}

	public final String getAlgorithm() {
		return algorithm;
	}

	public final int getDigestLength() {
		return engineGetDigestLength();
	}

	public Object clone() throws CloneNotSupportedException {
		return super.clone();
	}

	private static MessageDigest getInstance(String alg, Provider p)
			throws NoSuchAlgorithmException {

		// See if algorithm name is an alias
		String alias = (String)p.get("Alg.Alias.MessageDigest." + alg);
		if (alias != null) {
			alg = alias;
		}

		// Find class that implements the algorithm
		String cname = (String)p.get("MessageDigest." + alg);
		if (cname == null) {
			throw new NoSuchAlgorithmException(
				"not defined by provider");
		}

		// Instantiate class
		MessageDigest d;
		try {
			d = (MessageDigest)Class.forName(cname).newInstance();
		} catch (ClassNotFoundException e) {
			throw new NoSuchAlgorithmException("class "
				+ cname + " not found");
		} catch (InstantiationException e) {
			throw new NoSuchAlgorithmException("cannot instantiate"
				+ " class " + cname + ": " + e);
		} catch (IllegalAccessException e) {
			throw new NoSuchAlgorithmException("cannot instantiate"
				+ " class " + cname + ": " + e);
		} catch (ClassCastException e) {
			throw new NoSuchAlgorithmException("cannot instantiate"
				+ " class " + cname + ": wrong type");
		}

		// Return it
		d.provider = p;
		d.algorithm = alg;
		return d;
	}
}

