/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * NB!!!! THIS DOES NOT ACTUALLY IMPLEMENT SHA1PRNG - it uses random and
 *        is a place holder.
 *
 */

package kaffe.security.provider;

import java.security.SecureRandomSpi;
import java.util.Random;

public class SHA1PRNG extends SecureRandomSpi {

private Random rand;

public SHA1PRNG() {
	rand = new Random();
}

protected void engineSetSeed(byte[] seed) {
	long sd = ((seed[0] & 0xFF) |
		  ((seed[1] & 0xFF) << 8)  |
		  ((seed[2] & 0xFF) << 16) |
		  ((seed[3] & 0xFF) << 24) |
		  ((seed[4] & 0xFF) << 32) |
		  ((seed[5] & 0xFF) << 40) |
		  ((seed[6] & 0xFF) << 48) |
		  ((seed[7] & 0xFF) << 54));
	rand.setSeed(sd);
}

protected void engineNextBytes(byte[] bytes) {
	rand.nextBytes(bytes);
}

protected byte[] engineGenerateSeed(int numBytes) {
	return (new byte[numBytes]);
}

}
