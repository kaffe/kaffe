
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

public class ProtectionDomain {
	private final CodeSource cs;
	private final PermissionCollection pc;

	public ProtectionDomain(CodeSource cs, PermissionCollection pc) {
		this.cs = cs;
		this.pc = pc;
		if (pc != null)
			pc.setReadOnly();
	}

	public final CodeSource getCodeSource() {
		return cs;
	}

	public final PermissionCollection getPermissions() {
		return pc;
	}

	public boolean implies(Permission p) {
		return pc.implies(p);
	}

	public String toString() {
		return "{" + cs + ", " + pc + "}";
	}
}

