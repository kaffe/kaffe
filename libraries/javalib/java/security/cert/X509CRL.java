/*
 * X509CRL.java
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

import java.util.Set;
import java.util.Date;

import java.math.BigInteger;

import java.security.PublicKey;
import java.security.Principal;
import java.security.SignatureException;
import java.security.InvalidKeyException;
import java.security.NoSuchProviderException;
import java.security.NoSuchAlgorithmException;

public abstract class X509CRL
    extends CRL
    implements X509Extension
{
    protected X509CRL()
    {
	super("X.509");
    }
    
    public abstract byte[] getEncoded()
	throws CRLException;
    
    public abstract void verify(PublicKey key)
	throws CRLException,
	       NoSuchAlgorithmException,
	       InvalidKeyException,
	       NoSuchProviderException,
	       SignatureException;

    public abstract void verify(PublicKey key,
				String sigProvider)
	throws CRLException,
	       NoSuchAlgorithmException,
	       InvalidKeyException,
	       NoSuchProviderException,
	       SignatureException;

    public abstract int getVersion();

    public abstract Principal getIssuerDN();

    public abstract Date getThisUpdate();

    public abstract Date getNextUpdate();

    public abstract X509CRLEntry getRevokedCertificate(BigInteger serialNumbe);

    public abstract Set getRevokedCertificates();

    public abstract byte[] getTBSCertList()
	throws CRLException;

    public abstract byte[] getSignature();

    public abstract String getSigAlgName();

    public abstract String getSigAlgOID();

    public abstract byte[] getSigAlgParams();

    public boolean equals(Object obj)
    {
	boolean retval = false;

	if( obj instanceof X509CRL )
	{
	    X509CRL crl = (X509CRL)obj;

	    try
	    {
		byte objEncoding[] = crl.getEncoded();
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
	    catch(CRLException e)
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
	catch(CRLException e)
	{
	}
	return retval;
    }
}
