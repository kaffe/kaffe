/*
 * CertificateFactory.java
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

package java.security.cert;

import kaffe.security.Engine;

import java.io.InputStream;

import java.util.Collection;

import java.security.Provider;
import java.security.NoSuchProviderException;
import java.security.NoSuchAlgorithmException;

public class CertificateFactory
{
    private static final String ENGINE_CLASS = "CertificateFactory";
    
    private CertificateFactorySpi certFacSpi;
    private Provider provider;
    private String type;
    
    protected CertificateFactory(CertificateFactorySpi certFacSpi,
				 Provider provider,
				 String type)
    {
	this.certFacSpi = certFacSpi;
	this.provider = provider;
	this.type = type;
    }

    public final Provider getProvider()
    {
	return this.provider;
    }

    public final String getType()
    {
	return this.type;
    }

    public final Certificate generateCertificate(InputStream inStream)
	throws CertificateException
    {
	return this.certFacSpi.engineGenerateCertificate(inStream);
    }

    public final Collection generateCertificates(InputStream inStream)
	throws CertificateException
    {
	return this.certFacSpi.engineGenerateCertificates(inStream);
    }

    public final CRL generateCRL(InputStream inStream)
	throws CRLException
    {
	return this.certFacSpi.engineGenerateCRL(inStream);
    }
    
    public final Collection generateCRLs(InputStream inStream)
	throws CRLException
    {
	return this.certFacSpi.engineGenerateCRLs(inStream);
    }
    
    public static final CertificateFactory getInstance(String type)
	throws CertificateException
    {
	try
	{
	    return getInstance(Engine.getCryptInstance(ENGINE_CLASS,
						       type));
	}
	catch(NoSuchAlgorithmException e)
	{
	    // XXX Is this right?
	    throw new CertificateException("No such type");
	}
    }

    public static final CertificateFactory getInstance(String type,
						       String provider)
	throws CertificateException,
	       NoSuchProviderException
    {
	try
	{
	    return getInstance(Engine.getCryptInstance(ENGINE_CLASS,
						       type,
						       provider));
	}
	catch(NoSuchAlgorithmException e)
	{
	    // XXX Is this right?
	    throw new CertificateException("No such type");
	}
    }

    private static CertificateFactory getInstance(Engine e)
    {
	return new CertificateFactory((CertificateFactorySpi)e.getEngine(),
				      e.getProvider(),
				      e.getAlgorithm());
    }
}
