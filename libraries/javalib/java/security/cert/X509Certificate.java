/*
 * X509Certificate.java
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

import java.util.Date;

import java.math.BigInteger;

import java.security.Principal;

public abstract class X509Certificate
    extends Certificate
    implements X509Extension
{
    public X509Certificate()
    {
	super("X.509");
    }
    
    public abstract void checkValidity()
	throws CertificateExpiredException,
	       CertificateNotYetValidException;

    public abstract void checkValidity(Date date)
	throws CertificateExpiredException,
	       CertificateNotYetValidException;

    public abstract int getVersion();

    public abstract BigInteger getSerialNumber();

    public abstract Principal getIssuerDN();

    public abstract Principal getSubjectDN();

    public abstract Date getNotBefore();

    public abstract Date getNotAfter();

    public abstract byte[] getTBSCertificate()
	throws CertificateEncodingException;

    public abstract byte[] getSignature();

    public abstract String getSigAlgName();

    public abstract String getSigAlgOID();

    public abstract byte[] getSigAlgParams();

    public abstract boolean[] getIssuerUniqueID();

    public abstract boolean[] getSubjectUniqueID();

    public abstract boolean[] getKeyUsage();

    public abstract int getBasicConstraints();
}
