/*
 * Java core library component.
 *
 * Copyright (c) 2002
 *	Dalibor Topic.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Dalibor Topic <robilad@yahoo.com>
 */

package java.security;

import java.io.Serializable;
import java.util.ArrayList;
import java.util.Collection;

public abstract class Identity implements Principal, Serializable {
    private String name;
    private IdentityScope scope;
    private Collection certs = new ArrayList();
    private String info;
    private PublicKey key;

    private static final Certificate [] CERTIFICATE_ARRAY_DUMMY = new Certificate [0];

    public Identity() {
    }

    public Identity(String name) {
	this.name = name;
    }

    public Identity(String name, IdentityScope scope) throws KeyManagementException {
	if (scope.getIdentity(name) != null) {
	    throw new KeyManagementException("There is already an identity " + name + " in identity scope " + scope);
	}

	this.name = name;
	this.scope = scope;
    }

    public void addCertificate(Certificate certificate) throws KeyManagementException {
	if (getPublicKey() != null) {
	    if (getPublicKey() != certificate.getPublicKey()) {
		throw new KeyManagementException("Public key in the certificate being added conflicts with identity's public key");
	    }
	}
	else {
	    setPublicKey(certificate.getPublicKey());	    
	}

	certs.add(certificate);
    }

    public Certificate[] certificates() {
	return (Certificate []) certs.toArray(CERTIFICATE_ARRAY_DUMMY);
    }

    public final boolean equals(Object identity) {
	if (this == identity) {
	    return true;
	}

	if (identity instanceof Identity) {
	    Identity other = (Identity) identity;
	    if (other.getName() == getName()
		&& other.getScope() == getScope()) {
		return true;
	    }
	    else {
		return identityEquals(other);
	    }
	}

	return false;
    }

    public String getInfo() {
	return info;
    }

    public final String getName() {
	return name;
    }

    public PublicKey getPublicKey() {
	return key;
    }

    public final IdentityScope getScope() {
	return scope;
    }

    public int hashCode() {
	/* how the hash code is computed
	 * is not specified in the spec.
	 */
	return (name == null ? 0 : getName().hashCode())
	    + (scope == null ? 0 : getScope().hashCode());
    }

    protected boolean identityEquals(Identity identity) {
	return getName() == identity.getName()
	    && getPublicKey() == identity.getPublicKey();
    }

    public void removeCertificate(Certificate certificate) throws KeyManagementException {
	if (certs.remove(certificate) == false) {
	    throw new KeyManagementException("Certificate " + certificate + " is missing, so it can not be removed");
	}
    }

    public void setInfo(String info) {
	this.info = info;
    }

    public void setPublicKey(PublicKey key) throws KeyManagementException {
	if (getScope() != null && getScope().getIdentity(key) != null) {
	    throw new KeyManagementException("There is already an identity in identity scope " + scope + " with public key " + key);
	}

	this.key = key;
	this.certs = new ArrayList();
    }

    public String toString() {
	return "java.security.Identity[name=" + name + ",scope=" + scope + ']';
    }

    public String toString(boolean detailed) {
	if (detailed) {
	    return "java.security.Identity[name=" + name + ",scope=" + scope + ",info=" + info + ",key=" + key + ",certificates=" + certs + ']';
	}
	else {
	    return toString();
	}
    }
}
