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
public class InvalidClassException
  extends ObjectStreamException
{
	public String classname;

public InvalidClassException(String s)
	{
	super(s);
	classname = null;
}

public InvalidClassException(String c, String s)
	{
	super(s);
	classname = c;
}

public String getMessage()
	{
	if (classname != null) {
		return (classname + ((Throwable)this).getMessage());
	}
	else {
		return (((Throwable)this).getMessage());
	}
}
}
