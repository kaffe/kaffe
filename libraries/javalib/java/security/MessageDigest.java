
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

// Note: it is a historical screwup that this class extends MessageDigestSpi.
// It should not but does. Unfortunately, MessageDigestSpi is abstract, and
// this class inherits that abstractness. That is why we must be able to
// cast the engine we get from the provider to a MessageDigest object instead
// of a MessageDigestSpi object (see ***note below). Normally this class
// would keep an instance of MessageDigestSpi in a private field, but instead
// 'this' is that instance, so we don't need a separate field for it.

public abstract class MessageDigest extends MessageDigestSpi {
	private static final String ENGINE_CLASS = "MessageDigest";
	private final String algorithm;
   /**	private MessageDigestSpi engine; **/
	private Provider provider;

	protected MessageDigest(String algorithm) {
		this.algorithm = algorithm;
	}

	public static MessageDigest getInstance(String algorithm)
			throws NoSuchAlgorithmException {
		return getInstance(Security.getCryptInstance(
			ENGINE_CLASS, algorithm));
	}

	public static MessageDigest getInstance(String algorithm, String prov)
			throws NoSuchAlgorithmException,
				NoSuchProviderException {
		return getInstance(Security.getCryptInstance(
			ENGINE_CLASS, algorithm, prov));
	}

	private static MessageDigest getInstance(Security.Engine e) {
		MessageDigest md = (MessageDigest)e.getEngine(); // ***note
		// should be: md = new MessageDigest(e.algorithm);
	  /**	md.engine = (MessageDigestSpi)e.engine;	  **/
		md.provider = e.getProvider();
		return md;
	}

	public final Provider getProvider() {
		return provider;
	}

	public void update(byte input) {
		/*engine.*/engineUpdate(input);
	}

	public void update(byte[] input, int offset, int len) {
		/*engine.*/engineUpdate(input, offset, len);
	}

	public void update(byte[] input) {
		update(input, 0, input.length);
	}

	public byte[] digest() {
		byte[] rtn = /*engine.*/engineDigest();
		/*engine.*/engineReset();
		return rtn;
	}

	public int digest(byte[] buf, int offset, int len)
			throws DigestException {
		int digestLen = /*engine.*/engineGetDigestLength();
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
		/*engine.*/engineReset();
	}

	public final String getAlgorithm() {
		return algorithm;
	}

	public final int getDigestLength() {
		return /*engine.*/engineGetDigestLength();
	}

	public Object clone() throws CloneNotSupportedException {
		return super.clone();
	}
}

