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
public class StringIndexOutOfBoundsException
  extends IndexOutOfBoundsException
{
public StringIndexOutOfBoundsException () {
	super();
}

public StringIndexOutOfBoundsException (String s) {
	super(s);
}

public StringIndexOutOfBoundsException(int index) {
	super(String.valueOf(index));
}
}
