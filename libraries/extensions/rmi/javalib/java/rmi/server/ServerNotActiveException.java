/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

import java.lang.Exception;

public class ServerNotActiveException
	extends Exception {

public ServerNotActiveException() {
	super();
}

public ServerNotActiveException(String s) {
	super(s);
}

}
