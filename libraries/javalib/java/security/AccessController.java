
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

// This class is not fully implemented yet
public final class AccessController {

	// This class is not instantiable
	private AccessController() {
	}

	public static Object doPrivileged(PrivilegedAction action) {
		return action.run();
	}

	public static Object doPrivileged(PrivilegedAction action,
					AccessControlContext context) {
		return action.run();
	}

	public static Object doPrivileged(PrivilegedExceptionAction action)
				throws PrivilegedActionException {
		try {
			return action.run();
		} catch (Exception e) {
			throw new PrivilegedActionException(e);
		}
	}

	public static Object doPrivileged(PrivilegedExceptionAction action,
					AccessControlContext context)
				throws PrivilegedActionException {
		try {
			return action.run();
		} catch (Exception e) {
			throw new PrivilegedActionException(e);
		}
	}

	public static AccessControlContext getContext() {
		return null;
	}

	public static void checkPermission(Permission perm)
				throws AccessControlException {
		return;
	}
}

