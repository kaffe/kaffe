/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang;

import java.io.PrintStream;
import java.io.PrintWriter;
import java.io.Serializable;

public class Throwable extends Object implements Serializable
{
	/*
	 * NB: this is a place where native code stores private
	 * data.  The gc will mark the memory referenced, but its allocation
	 * type is such that it won't be walked further.
	 * Do not use kaffe.util.Ptr here.
	 */
	private transient Object backtrace;
	private String message;
	private boolean setCause;
	private Throwable cause;

	// This is what Sun's JDK1.1 "serialver java.lang.Throwable" spits out
	private static final long serialVersionUID = -3042686055658047285L;

public Throwable() {
	this.message = null;
	this.setCause = false;
	this.cause = null;
	fillInStackTrace();
}

public Throwable(String mess) {
	this.message = mess;
	this.setCause = false;
	this.cause = null;
	fillInStackTrace();
}

public Throwable(Throwable cause) {
	this.message = (cause == null) ? null : cause.getMessage();
	this.setCause = true;
	this.cause = cause;
	fillInStackTrace();
}

public Throwable(String mess, Throwable cause) {
	this.message = mess;
	this.setCause = true;
	this.cause = cause;
	fillInStackTrace();
}

native public Throwable fillInStackTrace();

public Throwable getCause() {
	return this.cause;
}

public Throwable initCause(Throwable th) {
	// can be called *at most once*, cannot be called if appropriate constructor 
	// was invoked
	if (this.setCause)
		throw new IllegalStateException("Cause can be set at most once on an exception");

	if (th == this)
		throw new IllegalArgumentException("A throwable cannot be its own cause");

	this.setCause = true;
	this.cause = th;
	return this;
}

// public StackTraceElement[] getStackTrace()
// {
// 	return StackTraceElement.fromBackTrace(this.backtrace);
// }

public String getLocalizedMessage() {
	return (getMessage());
}

public String getMessage() {
	return (message);
}

public void printStackTrace() {
	printStackTrace(System.err);
}

public void printStackTrace(PrintStream s) {
	s.println(this.toString());
	printStackTrace0(s);

	// include the cause, XXX see JDK doc for suggestion on reducing verbosity of redundant traces
	if (this.setCause)
	{
		if (this.cause == null)
			s.println("Caused by: <null cause>");
		else
		{
			s.print("Caused by: ");
			cause.printStackTrace(s);
		}
	}
}

public void printStackTrace(PrintWriter s) {
	s.println(this.toString());
	printStackTrace0(s);

	// include the cause, XXX see JDK doc for suggestion on reducing verbosity of redundant traces
	if (this.setCause)
	{
		if (this.cause == null)
			s.println("Caused by: <null cause>");
		else
		{
			s.print("Caused by: ");
			cause.printStackTrace(s);
		}
	}

}

native private void printStackTrace0(Object s);

public String toString() {
	if (message != null) {
		return (this.getClass().getName() + ": " + getMessage());
	}
	else {
		return (this.getClass().getName());
	}
}
}
