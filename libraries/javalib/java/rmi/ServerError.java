/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi;

public class ServerError
	extends RemoteException {

public ServerError(String s, Error e) {
	super(s, e);
}

}
