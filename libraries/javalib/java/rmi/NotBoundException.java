/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi;

public class NotBoundException 
	extends Exception {

private static final long serialVersionUID = -1857741824849069317l;

public NotBoundException() {
	super();
}

public NotBoundException(String s) {
	super (s);
}

}
