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

public abstract class Signer extends Identity {
    private KeyPair keys;

    protected Signer() {
    }

    public Signer(String name) {
	super(name);
    }

    public Signer(String name, IdentityScope scope) throws KeyManagementException {
	super(name, scope);
    }

    public PrivateKey getPrivateKey() {
	return keys == null ? null : keys.getPrivate();
    }

    public final void setKeyPair(KeyPair pair) throws InvalidParameterException, KeyException {
	keys = pair;
    }

    public String toString() {
	return "java.security.Signer[name=" + getName() + ",scope=" + getScope() + ",privateKey=" + getPrivateKey();
    }
}
