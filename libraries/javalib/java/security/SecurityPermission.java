
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

public final class SecurityPermission extends BasicPermission {

	public SecurityPermission(String name) {
		super(name);
	}

	public SecurityPermission(String name, String actions) {
		super(name, actions);
		if (actions != null) {
			throw new IllegalArgumentException();
		}
	}
}

