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
public class IncompatibleClassChangeError
  extends LinkageError
{
public IncompatibleClassChangeError () {
	super();
}

public IncompatibleClassChangeError (String s) {
	super(s);
}
}
