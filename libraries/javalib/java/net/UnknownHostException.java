package java.net;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class UnknownHostException
  extends java.io.IOException
{
public UnknownHostException () {
	super();
}

public UnknownHostException (String s) {
	super(s);
}

public UnknownHostException (Throwable cause) {
	super(cause);
}

public UnknownHostException (String s, Throwable cause) {
	super(s, cause);
}
}
