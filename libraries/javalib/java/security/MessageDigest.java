
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
// this class inherits that abstractness. As a consequence, a number of classes
// that should extend MessageDigestSpi extend MessageDigest instead.

// On the other hand, *some* classes that should extend MessageDigestSpi
// *do* extend it. We need to handle that too.

// Therefore, this class proxies all calls to its engine-- but the engine is,
// by default, 'this'. When we need to construct a MessageDigest object
// wrapping a MessageDigestSpi object, we use a concrete subclass of
// MessageDigest, NonSpiMessageDigest, and change its 'engine' field
// to the actual MessageDigestSpi. Calling engine methods on a
// NonSpiMessageDigest object throws an UnsupportedOperationException.

public abstract class MessageDigest extends MessageDigestSpi {
	private static final String ENGINE_CLASS = "MessageDigest";
	private final String algorithm;
        private MessageDigestSpi engine = this;
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
		MessageDigest md;
		Object o = e.getEngine();
		if(o instanceof MessageDigest) {
			md = (MessageDigest)o;
		} else {
			md = new NonSpiMessageDigest(e.algorithm);
			md.engine = (MessageDigestSpi)o;
		}

		md.provider = e.getProvider();
		return md;
	}

	public final Provider getProvider() {
		return provider;
	}

	public void update(byte input) {
		engine.engineUpdate(input);
	}

	public void update(byte[] input, int offset, int len) {
		engine.engineUpdate(input, offset, len);
	}

	public void update(byte[] input) {
		update(input, 0, input.length);
	}

	public byte[] digest() {
		byte[] rtn = engine.engineDigest();
		engine.engineReset();
		return rtn;
	}

	public int digest(byte[] buf, int offset, int len)
			throws DigestException {
		int digestLen = engine.engineGetDigestLength();
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
		engine.engineReset();
	}

	public final String getAlgorithm() {
		return algorithm;
	}

	public final int getDigestLength() {
		return engine.engineGetDigestLength();
	}

	public Object clone() throws CloneNotSupportedException {
		return super.clone();
	}

	private static String NONSPI_MSG =
		"This MessageDigest is not a MessageDigestSpi. "+
		"MessageDigestSpi methods should not be used "+
		"on MessageDigest objects."; 

	private static class NonSpiMessageDigest extends MessageDigest {
		protected NonSpiMessageDigest(String algorithm) {
			super(algorithm);
		}
		protected int engineGetDigestLength() {
			throw new UnsupportedOperationException(NONSPI_MSG);
		}
		protected void engineUpdate(byte input) {
			throw new UnsupportedOperationException(NONSPI_MSG);
		}
		protected void engineUpdate(byte[] input, int offset, int len) {
			throw new UnsupportedOperationException(NONSPI_MSG);
		}
		protected byte[] engineDigest() {
			throw new UnsupportedOperationException(NONSPI_MSG);
		}
		protected int engineDigest(byte[] buf, int offset, int len) {
			throw new UnsupportedOperationException(NONSPI_MSG);
		}
		protected void engineReset() {
			throw new UnsupportedOperationException(NONSPI_MSG);
		}
	}
}

