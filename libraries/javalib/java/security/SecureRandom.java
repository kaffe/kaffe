/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.security;

import java.util.Random;

public class SecureRandom extends Random {

static final String ENGINE_CLASS = "SecureRandom";

private final Provider provider;
private final SecureRandomSpi engine;

public SecureRandom() { 
	try {
		Security.Engine e = Security.getCryptInstance(ENGINE_CLASS);
		provider = e.provider;
		engine = (SecureRandomSpi)e.engine;
	}
	catch (NoSuchAlgorithmException e) {
		throw new Error("no " + ENGINE_CLASS + " found");
	}
}

public SecureRandom(byte[] seed){
	this();
	setSeed(seed);
}

protected SecureRandom(SecureRandomSpi engine, Provider provider) {
	this.engine = engine;
	this.provider = provider;
}

public static SecureRandom getInstance(String alg)
		throws NoSuchAlgorithmException {
	Security.Engine e = Security.getCryptInstance(ENGINE_CLASS, alg);
	return new SecureRandom((SecureRandomSpi)e.engine, e.provider);
}

public static SecureRandom getInstance(String alg, String prov) 
		throws NoSuchAlgorithmException, NoSuchProviderException {
	Security.Engine e = Security.getCryptInstance( ENGINE_CLASS, alg, prov);
	return new SecureRandom((SecureRandomSpi)e.engine, e.provider);
}

public final Provider getProvider() {
	return provider;
}

public void setSeed(byte[] seed) {
	if (engine != null) {
		engine.engineSetSeed(seed);
	}
}

public void setSeed(long seed) {
	byte[] nseed = new byte[8];
	for (int i = 0; i < 8; i++) {
		nseed[i] = (byte)(seed >> (i * 8));
	}
	setSeed(nseed);
}

public void nextBytes(byte[] bytes) {
	engine.engineNextBytes(bytes);
}

protected final int next(int numBits) {
	byte[] res = new byte[(numBits + 7) / 8];
	nextBytes(res);
	return ( (res[0] & 0xFF) |
		((res[1] <<  8) & 0xFF00) |
		((res[2] << 16) & 0xFF0000) |
		((res[3] << 24) & 0xFF000000));
}

public static byte[] getSeed(int numBytes) {
	return new SecureRandom().getSeed(numBytes);
}

public byte[] generateSeed(int numBytes) {
	return engine.engineGenerateSeed(numBytes);
}

}
