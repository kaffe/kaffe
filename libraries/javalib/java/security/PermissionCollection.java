
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

import java.util.Enumeration;

public abstract class PermissionCollection {
	private boolean readOnly = false;

	public PermissionCollection() {
	}

	public abstract boolean implies(Permission permission);

	public abstract Enumeration elements();

	public void setReadOnly() {
		readOnly = true;
	}

	public boolean isReadOnly() {
		return readOnly;
	}

	public String toString() {
		return super.toString();
	}
}


