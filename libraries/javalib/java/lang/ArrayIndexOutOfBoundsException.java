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
public class ArrayIndexOutOfBoundsException
  extends IndexOutOfBoundsException
{
public ArrayIndexOutOfBoundsException () {
	super();
}

public ArrayIndexOutOfBoundsException (String s) {
	super(s);
}

public ArrayIndexOutOfBoundsException (int index) {
	super(String.valueOf(index));
}
}
