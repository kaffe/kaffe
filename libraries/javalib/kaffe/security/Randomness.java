/*
 * Randomness.java
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

/**
 * Base class for implementations of initial sources of randomness.
 */
public abstract class Randomness
{
	/**
	 * Construct an empty Randomness object.
	 */
	public Randomness()
	{
	}
	
	/**
	 * Fill the given array with some random bits.
	 *
	 * @param bits The array to fill with random bits.
	 */
	public abstract void fill(byte bits[]);

	/**
	 * @see java.lang.Object#toString()
	 */
	public String toString()
	{
		return "Randomness["
			+ "]";
	}
}
