/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi;

public class UnmarshalException
	extends RemoteException {

public static final long serialVersionUID = 594380845140740218l;

public UnmarshalException(String s) {
	super(s);
}

public UnmarshalException(String s, Exception e) {
	super(s, e);
}

}
