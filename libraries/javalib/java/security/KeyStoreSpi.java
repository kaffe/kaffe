/*
 * KeyStoreSpi.java
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
import java.io.InputStream;
import java.io.OutputStream;
import java.security.cert.Certificate;
import java.security.cert.CertificateException;
import java.util.Date;
import java.util.Enumeration;

public abstract class KeyStoreSpi
{
    public KeyStoreSpi()
    {
    }

    public abstract Key engineGetKey(String alias,
				     char[] password)
	throws NoSuchAlgorithmException,
	       UnrecoverableKeyException;

    public abstract Certificate[] engineGetCertificateChain(String alias);

    public abstract Certificate engineGetCertificate(String alias);

    public abstract Date engineGetCreationDate(String alias);

    public abstract void engineSetKeyEntry(String alias,
					   Key key,
					   char[] password,
					   Certificate[] chain)
	throws KeyStoreException;

    public abstract void engineSetKeyEntry(String alias,
					   byte[] key,
					   Certificate[] chain)
	throws KeyStoreException;

    public abstract void engineSetCertificateEntry(String alias,
						   Certificate cert)
	throws KeyStoreException;

    public abstract void engineDeleteEntry(String alias)
	throws KeyStoreException;

    public abstract Enumeration engineAliases();

    public abstract boolean engineContainsAlias(String alias);

    public abstract int engineSize();

    public abstract boolean engineIsKeyEntry(String alias);

    public abstract boolean engineIsCertificateEntry(String alias);

    public abstract String engineGetCertificateAlias(Certificate cert);

    public abstract void engineStore(OutputStream stream,
				     char[] password)
	throws IOException,
	       NoSuchAlgorithmException,
	       CertificateException;

    public abstract void engineLoad(InputStream stream,
				    char[] password)
	throws IOException,
	       NoSuchAlgorithmException,
	       CertificateException;
}
