/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */


package java.text;

public class ParseException extends Exception {

private int offset;

public ParseException(String mess, int off) {
	super(mess);
	offset = off;
}

public int getErrorOffset() {
	return (offset);
}

};
