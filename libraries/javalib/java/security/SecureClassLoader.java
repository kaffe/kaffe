
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
	private ClassLoader parent;

	protected SecureClassLoader(ClassLoader parent) {
		System.getSecurityManager().checkCreateClassLoader();
		this.parent = parent;
	}

	protected SecureClassLoader() {
		this(SecureClassLoader.class.getClassLoader());
	}

	protected final Class defineClass(String name, byte[] b,
			int off, int len, CodeSource cs) {
		return defineClass(name, b, off, len);
	}

	protected PermissionCollection getPermissions(CodeSource cs) {
		return null;	// XXX implement me
	}
}

