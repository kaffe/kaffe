
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

public abstract class Permission implements Guard, Serializable {
	private String name;

	public Permission(String name) {
		if( name == null )
		{
			throw new NullPointerException(
				"Null name not allowed for Permission");
		}
		if( name.equals("") )
		{
			throw new IllegalArgumentException(
				"Empty name not allowed for Permission");
		}
		this.name = name;
	}

	public void checkGuard(Object object) throws SecurityException {
		SecurityManager sm = System.getSecurityManager();
		if (sm != null)
			sm.checkPermission(this);
	}

	public abstract boolean implies(Permission permission);

	public abstract boolean equals(Object obj);

	public abstract int hashCode();

	public final String getName() {
		return name;
	}

	public abstract String getActions();

	public PermissionCollection newPermissionCollection() {
		return null;
	}

	public String toString() {
		StringBuffer b = new StringBuffer();
		b.append('(');
		b.append(this.getClass().getName());
		b.append(' ');
		b.append(getName());
		b.append(' ');
		b.append(getActions());
		b.append(')');
		return new String(b);
	}
}

