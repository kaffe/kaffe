/*
 * Certificate.java
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

import java.io.ObjectStreamException;
import java.io.Serializable;
import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.PublicKey;
import java.security.SignatureException;

public abstract class Certificate
    implements Serializable
{
    protected static class CertificateRep
	implements Serializable
    {
	private String type;
	private byte data[];
	
	protected CertificateRep(String type, byte data[])
	{
	    this.type = type;
	    this.data = data;
	}

	protected Object readResolve()
	    throws ObjectStreamException
	{
	    return null; // XXX
	}
    }

    private String type;

    protected Certificate(String type)
    {
	this.type = type;
    }

    public abstract byte[] getEncoded() throws CertificateEncodingException;

    public abstract PublicKey getPublicKey();

    public String getType()
    {
	return this.type;
    }
    
    public boolean equals(Object obj)
    {
	boolean retval = false;

	if( obj instanceof Certificate )
	{
	    Certificate cert = (Certificate)obj;

	    try
	    {
		byte objEncoding[] = cert.getEncoded();
		byte myEncoding[] = this.getEncoded();
		
		if( (objEncoding != null) &&
		    (myEncoding != null) &&
		    (objEncoding.length == myEncoding.length) )
		{
		    int lpc;

		    retval = true;
		    for( lpc = 0; (lpc < objEncoding.length) && retval; lpc++ )
		    {
			if( objEncoding[lpc] != myEncoding[lpc] )
			    retval = false;
		    }
		}
	    }
	    catch(CertificateEncodingException e)
	    {
	    }
	}
	return retval;
    }

    public int hashCode()
    {
	int retval = 0;
	
	try
	{
	    byte enc[] = this.getEncoded();
	    
	    if( enc != null )
	    {
		switch( enc.length )
		{
		case 1:
		    retval = enc[0];
		    break;
		case 2:
		    retval = ((enc[0] << 8) |
			      (enc[1]));
		    break;
		case 3:
		    retval = ((enc[0] << 16) |
			      (enc[1] <<  8) |
			      (enc[2]));
		    break;
		default:
		    retval = ((enc[0] << 24) |
			      (enc[1] << 16) |
			      (enc[2] <<  8) |
			      (enc[3]));
		    break;
		}
	    }
	}
	catch(CertificateEncodingException e)
	{
	}
	return retval;
    }

    public abstract String toString();

    public abstract void verify(PublicKey key)
	throws CertificateException,
	       NoSuchAlgorithmException,
	       InvalidKeyException,
	       NoSuchProviderException,
	       SignatureException;

    public abstract void verify(PublicKey key, String sigProvider)
	throws CertificateException,
	       NoSuchAlgorithmException,
	       InvalidKeyException,
	       NoSuchProviderException,
	       SignatureException;

    protected Object writeReplace()
	throws ObjectStreamException
    {
	try
	{
	    return new CertificateRep(this.type, this.getEncoded());
	}
	catch(CertificateEncodingException e)
	{
	    return null;
	}
    }
}
