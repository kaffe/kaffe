package java.util;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class Random
{
	private long seed;
	final private long p1 = 3141592653L;
	final private long p2 = 2718281829L;
	final private long mod = 34359738368L;

public Random()
	{
	this(System.currentTimeMillis());
}

public Random(long seed)
	{
	this.seed = seed;
}

public long next()
	{
	seed = ((p1 * seed) + p2) % mod;
	return (seed);
}

public void nextBytes(byte[] bytes)
	{
	for (int i = 0; i < bytes.length; i++) {
		bytes[i] = (byte)next();
	}
}

public double nextDouble() {
	return ((((double)next()/(double)mod)) + 1) / 2;
//	return ((double)next()/(double)mod);
}

public float nextFloat() {
	return ((((float)next()/(float)mod)) + 1) / 2;
//	return ((float)next()/(float)mod);
}

public double nextGaussian()
	{
	/* Generate Normally distributed coords from Uniform vars */
	/* From: "The Art Of Computer Programming" Knuth. Vol 2. Pg 117 */

	/* Since we only need one coord, the other isn't calculated */
	double s;
	double v1, v2;

	do {
		v1=(nextDouble()*2.0)-1.0;
		v2=(nextDouble()*2.0)-1.0;

		s=v1*v1+v2*v2;
	}
	while (s>=1);

	double lnS=Math.log(s)/Math.log(Math.E);

	return (v1*Math.sqrt((-2.0*lnS)/s));
}

public int nextInt()
	{
	return ((int)next());
}

public long nextLong()
	{
	return (next());
}

public void setSeed(long seed)
	{
	this.seed = seed;
}
}
