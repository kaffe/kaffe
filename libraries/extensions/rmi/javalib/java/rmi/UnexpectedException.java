/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.rmi;

public class UnexpectedException
	extends RemoteException {

public UnexpectedException(String s) {
	super(s);
}

public UnexpectedException(String s, Exception e) {
	super(s, e);
}

}
