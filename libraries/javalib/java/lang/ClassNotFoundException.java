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
public class ClassNotFoundException
  extends Exception
{
private static final long serialVersionUID = 9176873029745254542L;

public ClassNotFoundException () {
	super();
}

public ClassNotFoundException (String s) {
	super(s);
}
}
