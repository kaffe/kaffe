/*
 * Java core library component.
 *
 * Copyright (c) 2001
 *      Edouard G. Parmelan.  All rights reserved.
 * Copyright (c) 2001
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * Author: Edouard G. Parmelan <egp@free.fr>
 * Checked Spec: JDK 1.3
 */

package java.lang.reflect;

import java.io.PrintStream;
import java.io.PrintWriter;

public class UndeclaredThrowableException
    extends RuntimeException
{
    private Throwable thrown;

    protected UndeclaredThrowableException () {
	super();
	thrown = null;
    }

    public UndeclaredThrowableException (Throwable t) {
	super();
	thrown = t;
    }

    public UndeclaredThrowableException (Throwable t,String s) {
	super(s);
	thrown = t;
    }

    public Throwable getUndeclaredThrowable() {
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
		ps.print ("java.lang.reflect.UndeclaredThrowableException: ");
		thrown.printStackTrace(ps);
	    }
	}
    }

    public void printStackTrace(PrintWriter pw) {
	if (thrown == null)
	    super.printStackTrace(pw);
	else {
	    synchronized (pw) {
		pw.print ("java.lang.reflect.UndeclaredThrowableException: ");
		thrown.printStackTrace(pw);
	    }
	}
    }
}
