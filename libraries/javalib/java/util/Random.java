/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util;

import java.io.Serializable;

/**
 * This is implemented as described on pages 646 and up in the JLS
 * by Gosling, Steel, and Joy.  It says "Java must use all the algorithms
 * shown here for the class Random, for the sake of absolute portability
 * of Java code."
 */
public class Random implements Serializable
{
	private long seed;
	private boolean haveNextNextGaussian;
	private double nextNextGaussian;

	private static final long serialVersionUID = 3905348978240129619L;
	private static final long p1 = 0x5DEECE66DL;
	private static final long p2 = 0xBL;
	private static final long mask = ((1L << 48) - 1);

public Random() {
	this(System.currentTimeMillis());
}

public Random(long seed) {
	/* To call a non-final method from a constructor is bad programming 
	 * practice.  However, we must do it because the JLS says so.
	 */
	setSeed(seed);
}

protected int next(int bits) {
	seed = ((p1 * seed) + p2) & mask;
	return ((int)(seed >>> (48 - bits)));
}

public void nextBytes(byte[] bytes) {
	try {
		for (int i = 0; i < bytes.length; i += 4) {
			int next = next(32);
			bytes[i + 0] = (byte)((next      ) & 0xff);
			bytes[i + 1] = (byte)((next >>  8) & 0xff);
			bytes[i + 2] = (byte)((next >> 16) & 0xff);
			bytes[i + 3] = (byte)((next >> 24) & 0xff);
		}
	} catch (ArrayIndexOutOfBoundsException _) { }
}

public double nextDouble() {
	return ((((long)next(26) << 27) + next(27)) / (double)(1L << 53));
}

public boolean nextBoolean() {
	return (next(1) != 0);
}

public float nextFloat() {
	return (next(24) / ((float)(1 << 24)));
}

public double nextGaussian() {
	if (haveNextNextGaussian) {
		haveNextNextGaussian = false;
		return nextNextGaussian;
	} else {
		double s;
		double v1, v2;

		do {
			v1 = (nextDouble()*2.0)-1.0;
			v2 = (nextDouble()*2.0)-1.0;

			s = v1*v1+v2*v2;
		} while (s>=1);

		double norm = Math.sqrt(-2 * Math.log(s)/s);
		nextNextGaussian = v2 * norm;
		haveNextNextGaussian = true;
		return (v1 * norm);
	}
}

public int nextInt() {
	return (next(32));
}

public int nextInt(int n) {     
	if (n <= 0) {
		throw new IllegalArgumentException("n must be positive: " + n);
	}

	if ((n & -n) == n)  {
		return ((int)((n * (long)next(31)) >> 31));
	}
	int bits, val;
	do {
		bits = next(31);
		val = bits % n;
	} while (bits - val + (n-1) < 0);     
	return (val); 
}

public long nextLong() {
	return (((long)next(32) << 32) + next(32));
}

synchronized public void setSeed(long seed) {
    this.seed = (seed ^ p1) & mask;
    haveNextNextGaussian = false;
}
}
