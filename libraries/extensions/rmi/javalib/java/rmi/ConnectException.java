/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi;

public class ConnectException 
	extends RemoteException {


public ConnectException(String s) {
	super(s);
}

public ConnectException(String s, Exception e) {
	super (s, e);
}

}
