/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi;

public class NoSuchObjectException
	extends RemoteException {

public static final long serialVersionUID = 6619395951570472985L;

public NoSuchObjectException(String s) {
	super(s);
}

}
