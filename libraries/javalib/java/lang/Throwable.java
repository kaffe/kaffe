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
	private Throwable cause = this;

	// This is what Sun's JDK1.1 "serialver java.lang.Throwable" spits out
	private static final long serialVersionUID = -3042686055658047285L;

public Throwable() {
	this((String)null);
}

public Throwable(String mess) {
	message = mess;
	fillInStackTrace();
}

/** since 1.4 */
public Throwable(Throwable cause) {
	this (cause == null ? null : cause.getMessage(), cause);
}

/** since 1.4 */
public Throwable(String mess, Throwable cause) {
	this(mess);
	initCause(cause);
}

native public Throwable fillInStackTrace();

private boolean causeIsSet() {
	return this.cause != this;
}

/** since 1.4 */
public Throwable getCause() {
	return causeIsSet() ? cause : null;
}

public String getLocalizedMessage() {
	return (getMessage());
}

public String getMessage() {
	return (message);
}

/** since 1.4 */
public Throwable initCause(Throwable cause) {
	if (cause == this) {
		throw new IllegalArgumentException();
	}
	else if (causeIsSet()) {
		throw new IllegalStateException();
	}
	else {
		this.cause = cause;
		return this;
	}
}

public void printStackTrace() {
	printStackTrace(System.err);
}

public void printStackTrace(PrintStream s) {
	s.println(this.toString());
	printStackTrace0(s);
}

public void printStackTrace(PrintWriter s) {
	s.println(this.toString());
	printStackTrace0(s);
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
