/*
 * UnixRandomness.java
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

import java.io.DataInputStream;
import java.io.FileInputStream;
import java.io.IOException;
import java.util.Random;

/**
 * Unix implementation of kaffe.security.Randomness, reads from "/dev/urandom".
 */
public class UnixRandomness
    extends Randomness
{
	/**
	 * The file name of the random device.
	 *
	 * XXX Use a property.
	 */
	private static final String FILE_NAME = "/dev/urandom";

	/**
	 * The input stream bound to the random device.
	 */
	private final DataInputStream dis;

	/**
	 * Construct a UnixRandomness object.
	 *
	 * @exception IOException if the random device cannot be opened.
	 */
	public UnixRandomness()
		throws IOException
	{
		this.dis = new DataInputStream(new FileInputStream(FILE_NAME));
	}

	/**
	 * Fill the bits array with data from the random device.
	 *
	 * @see kaffe.security.Randomness#fill()
	 */
	public synchronized void fill(byte bits[])
	{
		try
		{
			this.dis.readFully(bits);
		}
		catch(IOException e)
		{
			/* XXX Better way to fall back? */
			new Random().nextBytes(bits);
		}
	}

	/**
	 * @see java.lang.Object#toString()
	 */
	public String toString()
	{
		return "UnixRandomness["
			+ super.toString()
			+ "]";
	}
}
