/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi;

public class ServerException
	extends RemoteException {

public static final long serialVersionUID = -4775845313121906682l;

public ServerException(String s) {
	super(s);
}

public ServerException(String s, Exception e) {
	super(s, e);
}

}
