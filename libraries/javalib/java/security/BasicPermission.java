
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

public abstract class BasicPermission extends Permission
		implements Serializable {

	public BasicPermission(String name) {
		super(name);
	}

	public BasicPermission(String name, String actions) {
		super(name);
	}

	public boolean implies(Permission p) {
		if (!p.getClass().equals(getClass())) {
			return false;
		}
		String myName = getName();
		String pName = p.getName();
		if (pName.equals(myName)) {
			return true;
		}
		if (myName.length() < 2) {
			return false;
		}
		if (myName.charAt(myName.length() - 2) == '.'
		    && myName.charAt(myName.length() - 1) == '*'
		    && myName.regionMatches(0, pName, 0, myName.length() - 1)) {
			return true;
		}
		return false;
	}

	public boolean equals(Object obj) {
		if (obj == null) {
			return false;
		}
		if (!obj.getClass().equals(getClass())) {
			return false;
		}
		return getName().equals(((BasicPermission)obj).getName());
	}

	public int hashCode() {
		return getName().hashCode();
	}

	public String getActions() {
		return "";
	}

/************* XXX implement me
	public PermissionCollection newPermissionCollection() {
	}
*************/

}

