package java.net;

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
public class UnknownServiceException
  extends java.io.IOException
{
public UnknownServiceException () {
	super();
}

public UnknownServiceException (String s) {
	super(s);
}
}
