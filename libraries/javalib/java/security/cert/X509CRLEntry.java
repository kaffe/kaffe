/*
 * X509CRLEntry.java
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

import java.math.BigInteger;
import java.util.Date;

public abstract class X509CRLEntry
    implements X509Extension
{
    public X509CRLEntry()
    {
    }
    
    public abstract byte[] getEncoded()
	throws CRLException;

    public abstract BigInteger getSerialNumber();

    public abstract Date getRevocationDate();

    public abstract boolean hasExtensions();

    public abstract String toString();
    
    public boolean equals(Object obj)
    {
	boolean retval = false;

	if( obj instanceof X509CRLEntry )
	{
	    X509CRLEntry entry = (X509CRLEntry)obj;

	    try
	    {
		byte objEncoding[] = entry.getEncoded();
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
