package java.lang.reflect;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class InvocationTargetException
  extends Exception
{
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
}
