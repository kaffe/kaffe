
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *	Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.security;

public class SecureClassLoader extends ClassLoader {
	protected SecureClassLoader(ClassLoader parent) {
		super(parent);
	}

	protected SecureClassLoader() {
	}

	protected final Class defineClass(String name, byte[] b,
			int off, int len, CodeSource cs) {
	    if (cs == null) {
		return defineClass(name, b, off, len);
	    }
	    else {
		return defineClass(name, b, off, len, new ProtectionDomain(cs, null));
	    }
	}

	protected PermissionCollection getPermissions(CodeSource cs) {
		return null;	// XXX implement me
	}
}

