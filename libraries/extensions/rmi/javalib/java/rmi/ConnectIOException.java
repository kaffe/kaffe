/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi;

public class ConnectIOException 
	extends RemoteException {


public ConnectIOException(String s) {
	super(s);
}

public ConnectIOException(String s, Exception e) {
	super (s, e);
}

}
