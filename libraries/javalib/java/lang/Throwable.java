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

public class Throwable extends Object
{
	private Object backtrace = null;
	private String message = null;

public Throwable()
	{
	message = null;
	fillInStackTrace();
}

public Throwable(String mess)
	{
	message = mess;
	fillInStackTrace();
}

native public Throwable fillInStackTrace();

public String getLocalizedMessage()
	{
	return (getMessage());
}

public String getMessage()
	{
	return (message);
}

public void printStackTrace()
	{
	printStackTrace(System.err);
}

public void printStackTrace(PrintStream s)
	{
	if (message != null) {
		s.println(this.getClass().getName() + ": " + message);
	}
	else {
		s.println(this.getClass().getName());
	}
	printStackTrace0(s);
}

public void printStackTrace(PrintWriter s)
	{
	if (message != null) {
		s.println(this.getClass().getName() + ": " + message);
	}
	else {
		s.println(this.getClass().getName());
	}
	printStackTrace0(s);
}

native private void printStackTrace0(Object s);

public String toString()
	{
	return (this.getClass().getName() + ": " + message);
}
}
