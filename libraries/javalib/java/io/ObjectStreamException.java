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
abstract public class ObjectStreamException
  extends IOException
{
protected ObjectStreamException()
	{
	super();
}

protected ObjectStreamException(String s)
	{
	super(s);
}
}
