
/*
 * Java core library component.
 *
 * Copyright (c) 1999
 *	Archie L. Cobbs.  All rights reserved.
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Archie L. Cobbs <archie@whistle.com>
 */

package java.security;

import java.io.PrintStream;
import java.io.PrintWriter;

public class PrivilegedActionException extends Exception {
	private final Exception e;

	public PrivilegedActionException(Exception e) {
		super();
		this.e = e;
	}

	public Exception getException() {
		return e;
	}

	public void printStackTrace() {
		e.printStackTrace();
	}

	public void printStackTrace(PrintStream p) {
		e.printStackTrace(p);
	}

	public void printStackTrace(PrintWriter p) {
		e.printStackTrace(p);
	}
}

