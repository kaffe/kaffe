/*
 * KeyFactory.java
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

import java.security.spec.InvalidKeySpecException;
import java.security.spec.KeySpec;

public class KeyFactory
{
    private static final String ENGINE_CLASS = "KeyFactory";
    
    private KeyFactorySpi keyFacSpi;
    private Provider provider;
    private String algorithm;
    
    protected KeyFactory(KeyFactorySpi keyFacSpi,
			 Provider provider,
			 String algorithm)
    {
	this.keyFacSpi = keyFacSpi;
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

    public final PublicKey generatePublic(KeySpec keySpec)
	throws InvalidKeySpecException
    {
	return this.keyFacSpi.engineGeneratePublic(keySpec);
    }
    
    public final PrivateKey generatePrivate(KeySpec keySpec)
	throws InvalidKeySpecException
    {
	return this.keyFacSpi.engineGeneratePrivate(keySpec);
    }

    public final KeySpec getKeySpec(Key key, Class keySpec)
	throws InvalidKeySpecException
    {
	return this.keyFacSpi.engineGetKeySpec(key, keySpec);
    }

    public final Key translateKey(Key key)
	throws InvalidKeyException
    {
	return this.keyFacSpi.engineTranslateKey(key);
    }
    
    public static KeyFactory getInstance(String algorithm)
	throws NoSuchAlgorithmException
    {
	return getInstance(Security.getCryptInstance(ENGINE_CLASS,
						     algorithm));
    }

    public static KeyFactory getInstance(String algorithm, String provider)
	throws NoSuchAlgorithmException,
	       NoSuchProviderException
    {
	return getInstance(Security.getCryptInstance(ENGINE_CLASS,
						     algorithm,
						     provider));
    }

    private static KeyFactory getInstance(Security.Engine e)
    {
	KeyFactorySpi spi;
	KeyFactory retval;

	spi = (KeyFactorySpi)e.getEngine();
	retval = new KeyFactory(spi, e.getProvider(), e.getAlgorithm());
	return retval;
    }
}
