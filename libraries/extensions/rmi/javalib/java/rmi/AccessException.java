/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.rmi;

public class AccessException 
	extends RemoteException {

public static final long serialVersionUID = 6314925228044966088l;

public AccessException(String s) {
	super(s);
}

public AccessException(String s, Exception e) {
	super (s, e);
}

}
