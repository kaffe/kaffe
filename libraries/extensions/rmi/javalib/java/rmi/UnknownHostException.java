/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.rmi;

public class UnknownHostException
	extends RemoteException {

public UnknownHostException(String s) {
	super(s);
}

public UnknownHostException(String s, Exception e) {
	super(s, e);
}

}
