package java.lang;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class RuntimeException
  extends Exception
{
public RuntimeException () {
	super();
}

public RuntimeException (String s) {
	super(s);
}

public RuntimeException (Throwable c) {
	super(c);
}

public RuntimeException (String s, Throwable c) {
	super(s,c);
}
}
