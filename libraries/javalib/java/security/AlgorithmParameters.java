/*
 * AlgorithmParameters.java
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

package java.security;

import java.io.IOException;

import kaffe.security.Engine;

import java.security.spec.AlgorithmParameterSpec;
import java.security.spec.InvalidParameterSpecException;

public class AlgorithmParameters
{
    private static final String ENGINE_CLASS = "AlgorithmParameters";
    
    private AlgorithmParametersSpi paramSpi;
    private Provider provider;
    private String algorithm;
    
    protected AlgorithmParameters(AlgorithmParametersSpi paramSpi,
				  Provider provider,
				  String algorithm)
    {
	this.paramSpi = paramSpi;
	this.provider = provider;
	this.algorithm = algorithm;
    }
    
    public final String getAlgorithm()
    {
	return this.algorithm;
    }

    public final Provider getProvider()
    {
	return this.provider;
    }
    
    public final void init(AlgorithmParameterSpec paramSpec)
	throws InvalidParameterSpecException
    {
	this.paramSpi.engineInit(paramSpec);
    }

    public final void init(byte[] params)
	throws IOException
    {
	this.paramSpi.engineInit(params);
    }

    public final void init(byte[] params, String format)
	throws IOException
    {
	this.paramSpi.engineInit(params, format);
    }

    public final AlgorithmParameterSpec getParameterSpec(Class paramSpec)
	throws InvalidParameterSpecException
    {
	return this.paramSpi.engineGetParameterSpec(paramSpec);
    }
    
    public final byte[] getEncoded()
	throws IOException
    {
	return this.paramSpi.engineGetEncoded();
    }

    public final byte[] getEncoded(String format)
	throws IOException
    {
	return this.paramSpi.engineGetEncoded(format);
    }

    public final String toString()
    {
	return this.paramSpi.engineToString();
    }
    
    public static AlgorithmParameters getInstance(String algorithm)
	throws NoSuchAlgorithmException
    {
	return getInstance(Engine.getCryptInstance(ENGINE_CLASS,
						   algorithm));
    }

    public static AlgorithmParameters getInstance(String algorithm,
						  String provider)
	throws NoSuchAlgorithmException,
	       NoSuchProviderException
    {
	return getInstance(Engine.getCryptInstance(ENGINE_CLASS,
						   algorithm,
						   provider));
    }

    private static AlgorithmParameters getInstance(Engine e)
    {
	AlgorithmParametersSpi spi;
	AlgorithmParameters retval;

	spi = (AlgorithmParametersSpi)e.getEngine();
	retval = new AlgorithmParameters(spi,
					 e.getProvider(),
					 e.getAlgorithm());
	return retval;
    }
}
