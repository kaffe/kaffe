
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

package kaffe.security.provider;

import java.security.*;

public final class Kaffe extends Provider {
	private static final String INFO = "Kaffe security provider";

	static {
		System.loadLibrary("security");
	}

	public Kaffe() {
		super("KAFFE", 1.0, INFO);

		// Register our supported algorithms
		AccessController.doPrivileged(
		    new PrivilegedAction() {
			public Object run() {
			    put("MessageDigest.MD2",
				    "kaffe.security.provider.MD2");
			    put("MessageDigest.MD4",
				    "kaffe.security.provider.MD4");
			    put("MessageDigest.MD5",
				    "kaffe.security.provider.MD5");
			    put("MessageDigest.SHA",
				    "kaffe.security.provider.SHA");
			    return null;
			}
		    });
	}
}

