package java.io;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

import java.lang.String;

public class CharConversionException
  extends IOException
{
public CharConversionException()
	{
	super();
}

public CharConversionException(String s)
	{
	super(s);
}
}
