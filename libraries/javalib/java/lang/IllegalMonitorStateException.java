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
public class IllegalMonitorStateException
  extends RuntimeException
{
public IllegalMonitorStateException () {
	super();
}

public IllegalMonitorStateException (String s) {
	super(s);
}
}
