/*
 * DSAParamaterSpec.java
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

import java.security.interfaces.DSAParams;

public class DSAParameterSpec
    implements AlgorithmParameterSpec, DSAParams
{
    private final BigInteger p;
    private final BigInteger q;
    private final BigInteger g;
    
    public DSAParameterSpec(BigInteger p,
			    BigInteger q,
			    BigInteger g)
    {
	/* XXX Are these checks valid? */
	if( p.signum() < 0 )
	    throw new IllegalArgumentException();
	if( q.signum() < 0 )
	    throw new IllegalArgumentException();
	if( g.signum() < 0 )
	    throw new IllegalArgumentException();
	this.p = p;
	this.q = q;
	this.g = g;
    }

    public BigInteger getP()
    {
	return this.p;
    }

    public BigInteger getQ()
    {
	return this.q;
    }

    public BigInteger getG()
    {
	return this.g;
    }

    public String toString()
    {
	return "DSAParameterSpec[p="
	    + this.p
	    + "; q="
	    + this.q
	    + "; g="
	    + this.g
	    + "]";
    }
}
