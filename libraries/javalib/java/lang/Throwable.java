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

	// This is what Sun's JDK1.1 "serialver java.lang.Throwable" spits out
	private static final long serialVersionUID = -3042686055658047285L;

public Throwable() {
	message = null;
	fillInStackTrace();
}

public Throwable(String mess) {
	message = mess;
	fillInStackTrace();
}

native public Throwable fillInStackTrace();

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
