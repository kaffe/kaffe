/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998, 2001
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Checked Spec: JDK 1.3
 */

package java.lang.reflect;

import java.io.PrintStream;
import java.io.PrintWriter;

public class InvocationTargetException
  extends Exception
{
	private static final long serialVersionUID = 4085088731926701167L;
	private Throwable thrown;

protected InvocationTargetException () {
	super();
	thrown = null;
}

public InvocationTargetException (Throwable t,String s) {
	super(s);
	thrown = t;
}

public InvocationTargetException (Throwable t) {
	super();
	thrown = t;
}

public Throwable getTargetException() {
	return (thrown);
}

public void printStackTrace() {
	printStackTrace(System.err);
}

public void printStackTrace(PrintStream ps) {
	if (thrown == null)
		super.printStackTrace(ps);
	else {
		synchronized (ps) {
			ps.print ("java.lang.reflect.InvocationTargetException: ");
			thrown.printStackTrace(ps);
		}
	}
}

public void printStackTrace(PrintWriter pw) {
	if (thrown == null)
		super.printStackTrace(pw);
	else {
		synchronized (pw) {
			pw.print ("java.lang.reflect.InvocationTargetException: ");
			thrown.printStackTrace(pw);
		}
	}
}
}
