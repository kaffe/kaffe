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
public class IllegalArgumentException
  extends RuntimeException
{
public IllegalArgumentException () {
	super();
}

public IllegalArgumentException (String s) {
	super(s);
}
    
public IllegalArgumentException (Throwable th) {
	super(th);
}

public IllegalArgumentException (String s, Throwable th) {
	super(s, th);
}
}
