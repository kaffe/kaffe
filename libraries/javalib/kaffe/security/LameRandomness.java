/*
 * LameRandomness.java
 *
 * Copyright (c) 2004 The University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

package kaffe.security;

import java.util.Random;

/**
 * Lame implementation of kaffe.security.Randomness, just uses a value from
 * java.util.Random.
 */
public class LameRandomness
    extends Randomness
{
	/**
	 * Construct an empty LameRandomness object.
	 */
	public LameRandomness()
	{
	}
	
	/**
	 * Fill the bits array using java.util.Random#nextBytes().
	 *
	 * @see kaffe.security.Randomness#fill()
	 */
	public void fill(byte bits[])
	{
		new Random().nextBytes(bits);
	}
	
	/**
	 * @see java.lang.Object#toString()
	 */
	public String toString()
	{
		return "LameRandomness["
			+ super.toString()
			+ "]";
	}
}
