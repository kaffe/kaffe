/*
 * RSAKeyGenParameterSpec.java
 *
 * Copyright (c) 2001 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

package java.security.spec;

import java.math.BigInteger;

public class RSAKeyGenParameterSpec
    implements AlgorithmParameterSpec
{
    public static final BigInteger F0 = new BigInteger("3");
    public static final BigInteger F4 = new BigInteger("65537");

    private final int keysize;
    private final BigInteger publicExponent;
    
    public RSAKeyGenParameterSpec(int keysize, BigInteger publicExponent)
    {
	this.keysize = keysize;
	this.publicExponent = publicExponent;
    }

    public int getKeysize()
    {
	return this.keysize;
    }

    public BigInteger getPublicExponent()
    {
	return this.publicExponent;
    }

    public String toString()
    {
	return "RSAKeyGenParameterSpec[keysize="
	    + this.keysize
	    + "; publicExponent="
	    + this.publicExponent
	    + "]";
    }
}
