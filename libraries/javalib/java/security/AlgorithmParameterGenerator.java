/*
 * AlgorithmParameterGenerator.java
 *
 * Copyright (c) 2001, 2003 University of Utah and the Flux Group.
 * All rights reserved.
 *
 * This file is licensed under the terms of the GNU Public License.
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 * Contributed by the Flux Research Group, Department of Computer Science,
 * University of Utah, http://www.cs.utah.edu/flux/
 */

package java.security;

import java.security.spec.AlgorithmParameterSpec;

public class AlgorithmParameterGenerator
{
    private static final String ENGINE_CLASS = "AlgorithmParameterGenerator";
    
    private AlgorithmParameterGeneratorSpi paramGenSpi;
    private Provider provider;
    private String algorithm;
    
    protected AlgorithmParameterGenerator(
	AlgorithmParameterGeneratorSpi paramGenSpi,
	Provider provider,
	String algorithm)
    {
	this.paramGenSpi = paramGenSpi;
	this.provider = provider;
	this.algorithm = algorithm;
    }

    public final Provider getProvider()
    {
	return this.provider;
    }
    
    public final String getAlgorithm()
    {
	return this.algorithm;
    }

    public final void init(int size)
    {
	this.paramGenSpi.engineInit(size, new SecureRandom());
    }

    public final void init(int size, SecureRandom random)
    {
	this.paramGenSpi.engineInit(size, random);
    }

    public final void init(AlgorithmParameterSpec genParamSpec)
	throws InvalidAlgorithmParameterException
    {
	this.paramGenSpi.engineInit(genParamSpec, new SecureRandom());
    }

    public final void init(AlgorithmParameterSpec genParamSpec,
			   SecureRandom random)
	throws InvalidAlgorithmParameterException
    {
	this.paramGenSpi.engineInit(genParamSpec, random);
    }

    public final AlgorithmParameters generateParameters()
    {
	return this.paramGenSpi.engineGenerateParameters();
    }
    
    public static AlgorithmParameterGenerator getInstance(String algorithm)
	throws NoSuchAlgorithmException
    {
	return getInstance(Security.getCryptInstance(ENGINE_CLASS, algorithm));
    }

    public static AlgorithmParameterGenerator getInstance(String algorithm,
							  String provider)
	throws NoSuchAlgorithmException,
	       NoSuchProviderException
    {
	return getInstance(Security.getCryptInstance(ENGINE_CLASS,
						     algorithm,
						     provider));
    }

    private static AlgorithmParameterGenerator getInstance(Security.Engine e)
    {
	AlgorithmParameterGeneratorSpi spi;
	AlgorithmParameterGenerator retval;

	spi = (AlgorithmParameterGeneratorSpi)e.getEngine();
	retval = new AlgorithmParameterGenerator(spi,
						 e.getProvider(),
						 e.getAlgorithm());
	return retval;
    }
}
