package java.io;

import java.lang.String;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class EOFException
  extends IOException
{
public EOFException () {
	super();
}

public EOFException (String s) {
	super(s);
}
}
