
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

import java.security.AccessController;
import java.security.PrivilegedAction;
import java.security.Provider;

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
			    put("MessageDigest.SHA-1",
				    "kaffe.security.provider.SHA");
			    put("Alg.Alias.MessageDigest.SHA1",
				    "SHA-1");
			    put("Alg.Alias.MessageDigest.SHA",
				    "SHA-1");
			    put("SecureRandom.SHA1PRNG",
				    "kaffe.security.provider.SHA1PRNG");
			    return null;
			}
		    });
	}
}

