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
class WriteAbortedException
  extends ObjectStreamException
{
	public Exception detail;

public WriteAbortedException(String s, Exception e)
	{
	super(s);
	detail = e;
}

public String getMessage()
	{
	if (detail != null) {
		return (((Throwable)this).getMessage() + ":" + detail.getMessage());
	}
	else {
		return (((Throwable)this).getMessage());
	}
}
}
