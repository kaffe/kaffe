
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

public final class  AccessControlContext {

/************* XXX implement me
	public AccessControlContext(ProtectionDomain[] context) {
	}
*************/

	public void checkPermission(Permission perm)
			throws AccessControlException {
	}

	public boolean equals(Object obj) {
		return obj == this;
	}

	public int hashCode() {
		return ((Object)this).hashCode();
	}
}

