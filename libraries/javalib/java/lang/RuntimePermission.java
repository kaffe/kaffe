
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

package java.lang;

import java.security.BasicPermission;

public final class RuntimePermission extends BasicPermission {

	public RuntimePermission(String name) {
		super(name);
	}

	public RuntimePermission(String name, String actions) {
		super(name, actions);
		if (actions != null) {
			throw new IllegalArgumentException();
		}
	}
}

