/*
 * KeyStore.java
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

import java.util.Date;
import java.util.Enumeration;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

import java.security.cert.Certificate;
import java.security.cert.CertificateException;

public class KeyStore
{
    private static final String ENGINE_CLASS = "KeyStore";
    
    private KeyStoreSpi keyStoreSpi;
    private Provider provider;
    private String type;
    
    protected KeyStore(KeyStoreSpi keyStoreSpi,
		       Provider provider,
		       String type)
    {
	this.keyStoreSpi = keyStoreSpi;
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

    public final Key getKey(String alias, char[] password)
	throws KeyStoreException,
	       NoSuchAlgorithmException,
	       UnrecoverableKeyException
    {
	return this.keyStoreSpi.engineGetKey(alias, password);
    }
    
    public final Certificate[] getCertificateChain(String alias)
	throws KeyStoreException
    {
	return this.keyStoreSpi.engineGetCertificateChain(alias);
    }

    public final Certificate getCertificate(String alias)
	throws KeyStoreException
    {
	return this.keyStoreSpi.engineGetCertificate(alias);
    }

    public final Date getCreationDate(String alias)
	throws KeyStoreException
    {
	return this.keyStoreSpi.engineGetCreationDate(alias);
    }
    
    public final void setKeyEntry(String alias,
				  Key key,
				  char[] password,
				  Certificate[] chain)
	throws KeyStoreException
    {
	this.keyStoreSpi.engineSetKeyEntry(alias, key, password, chain);
    }
    
    public final void setKeyEntry(String alias,
				  byte[] key,
				  Certificate[] chain)
	throws KeyStoreException
    {
	this.keyStoreSpi.engineSetKeyEntry(alias, key, chain);
    }

    public final void setCertificateEntry(String alias,
					  Certificate cert)
	throws KeyStoreException
    {
	this.keyStoreSpi.engineSetCertificateEntry(alias, cert);
    }

    public final void deleteEntry(String alias)
	throws KeyStoreException
    {
	this.keyStoreSpi.engineDeleteEntry(alias);
    }

    public final Enumeration aliases()
    {
	return this.keyStoreSpi.engineAliases();
    }

    public final boolean containsAlias(String alias)
	throws KeyStoreException
    {
	return this.keyStoreSpi.engineContainsAlias(alias);
    }

    public final int size()
    {
	return this.keyStoreSpi.engineSize();
    }

    public final boolean isKeyEntry(String alias)
	throws KeyStoreException
    {
	return this.keyStoreSpi.engineIsKeyEntry(alias);
    }

    public final boolean isCertificateEntry(String alias)
	throws KeyStoreException
    {
	return this.keyStoreSpi.engineIsCertificateEntry(alias);
    }
    
    public final String getCertificateAlias(Certificate cert)
	throws KeyStoreException
    {
	return this.keyStoreSpi.engineGetCertificateAlias(cert);
    }

    public final void store(OutputStream stream,
			    char[] password)
	throws KeyStoreException,
	       IOException,
	       NoSuchAlgorithmException,
	       CertificateException
    {
	this.keyStoreSpi.engineStore(stream, password);
    }

    public final void load(InputStream stream,
			   char[] password)
	throws IOException,
	       NoSuchAlgorithmException,
	       CertificateException
    {
	this.keyStoreSpi.engineLoad(stream, password);
    }

    public static final String getDefaultType()
    {
	String retval;

	if( (retval = Security.getProperty("keystore.type")) == null )
	{
	    retval = "JKS";
	}
	return retval;
    }
    
    public static KeyStore getInstance(String type)
	throws KeyStoreException
    {
	try
	{
	    return getInstance(Security.getCryptInstance(ENGINE_CLASS,
							 type));
	}
	catch(NoSuchAlgorithmException e)
	{
	    // XXX Is this right?
	    throw new KeyStoreException("No such type");
	}
    }

    public static KeyStore getInstance(String type,
				       String provider)
	throws KeyStoreException,
	       NoSuchProviderException
    {
	try
	{
	    return getInstance(Security.getCryptInstance(ENGINE_CLASS,
							 type,
							 provider));
	}
	catch(NoSuchAlgorithmException e)
	{
	    // XXX Is this right?
	    throw new KeyStoreException("No such type");
	}
    }

    private static KeyStore getInstance(Security.Engine e)
    {
	return new KeyStore((KeyStoreSpi)e.getEngine(),
			    e.getProvider(),
			    e.getAlgorithm());
    }
}
