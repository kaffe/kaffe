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

import java.util.Enumeration;

/** @deprecated */
public abstract class IdentityScope extends Identity {
    protected IdentityScope() {
    }

    public IdentityScope(String name) {
	super(name);
    }

    public IdentityScope(String name, IdentityScope scope) throws KeyManagementException {
	super(name, scope);
    }

    public abstract void addIdentity(Identity identity) throws KeyManagementException;

    public Identity getIdentity(Principal principal) {
	return getIdentity(principal.getName());
    }

    public abstract Identity getIdentity(PublicKey key);

    public abstract Identity getIdentity(String name);

    public static IdentityScope getSystemScope() {
	/* XXX FIXME: needs to be implemented */
	throw new kaffe.util.NotImplemented();
    }

    public abstract Enumeration identities();

    public abstract void removeIdentity(Identity identity) throws KeyManagementException;

    protected static void setSystemScope(IdentityScope scope) {
	/* XXX FIXME: needs to be implemented */
	throw new kaffe.util.NotImplemented();
    }

    public abstract int size();

    public String toString() {
	return "java.security.IdentityScope[name=" + getName() + ",scope=" + getScope() + "size=" + size() + ']';
    }
}
