
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

package java.net;

import java.security.BasicPermission;

public final class NetPermission extends BasicPermission {

	public NetPermission(String name) {
		super(name);
	}

	public NetPermission(String name, String actions) {
		super(name, actions);
	}
}

