/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

import java.rmi.RemoteException;

public class SkeletonMismatchException
	extends RemoteException {

private static final long serialVersionUID = -7780460454818859281l;

public SkeletonMismatchException(String s) {
	super(s);
}

}
