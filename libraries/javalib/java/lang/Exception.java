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
public class Exception
	extends Throwable
{

private static final long serialVersionUID = -3387516993124229948l;

public Exception () {
	super();
}

public Exception (String s) {
	super(s);
}

/** since 1.4 */
public Exception(Throwable cause) {
	super (cause == null ? null : cause.getMessage(), cause);
}

/** since 1.4 */
public Exception(String mess, Throwable cause) {
	super(mess, cause);
}
}

