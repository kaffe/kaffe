
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.security;

public class NoSuchProviderException extends GeneralSecurityException {

	public NoSuchProviderException() {
		super();
	}

	public NoSuchProviderException(String msg) {
		super(msg);
	}
}

