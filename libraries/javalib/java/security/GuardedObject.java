
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

import java.io.Serializable;

public class GuardedObject implements Serializable {
	private final Object object;
	private final Guard guard;

	public GuardedObject(Object object, Guard guard) {
		this.guard = guard;
		this.object = object;
	}

	public Object getObject() throws SecurityException {
		guard.checkGuard(object);
		return object;
	}
}

