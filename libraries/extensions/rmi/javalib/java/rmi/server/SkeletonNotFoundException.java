/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

import java.rmi.RemoteException;

public class SkeletonNotFoundException
	extends RemoteException {

public SkeletonNotFoundException(String s) {
	super(s);
}

public SkeletonNotFoundException(String s, Exception e) {
	super(s, e);
}

}
