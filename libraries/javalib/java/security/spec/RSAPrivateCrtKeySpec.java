/*
 * RSAPrivateCrtKeySpec.java
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

public class RSAPrivateCrtKeySpec
    extends RSAPrivateKeySpec
{
    private final BigInteger publicExponent;
    private final BigInteger primeP;
    private final BigInteger primeQ;
    private final BigInteger primeExponentP;
    private final BigInteger primeExponentQ;
    private final BigInteger crtCoefficient;

    public RSAPrivateCrtKeySpec(BigInteger modulus,
				BigInteger publicExponent,
				BigInteger privateExponent,
				BigInteger primeP,
				BigInteger primeQ,
				BigInteger primeExponentP,
				BigInteger primeExponentQ,
				BigInteger crtCoefficient)
    {
	super(modulus,
	      privateExponent);
	this.publicExponent = publicExponent;
	this.primeP = primeP;
	this.primeQ = primeQ;
	this.primeExponentP = primeExponentP;
	this.primeExponentQ = primeExponentQ;
	this.crtCoefficient = crtCoefficient;
    }

    public BigInteger getPublicExponent()
    {
	return this.publicExponent;
    }
    
    public BigInteger getPrimeP()
    {
	return this.primeP;
    }

    public BigInteger getPrimeQ()
    {
	return this.primeQ;
    }

    public BigInteger getPrimeExponentP()
    {
	return this.primeExponentP;
    }

    public BigInteger getPrimeExponentQ()
    {
	return this.primeExponentQ;
    }

    public BigInteger getCrtCoefficient()
    {
	return this.crtCoefficient;
    }

    public String toString()
    {
	return "RSAPrivateCrtKeySpec[publicExponent="
	    + this.publicExponent
	    + "; primeP="
	    + this.primeP
	    + "; primeQ="
	    + this.primeQ
	    + "; primeExponentP="
	    + this.primeExponentP
	    + "; primeExponentQ="
	    + this.primeExponentQ
	    + "; crtCoefficient="
	    + this.crtCoefficient
	    + "]";
    }
}
