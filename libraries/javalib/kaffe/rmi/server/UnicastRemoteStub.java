/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package kaffe.rmi.server;

import java.rmi.server.RemoteStub;
import java.rmi.server.RemoteRef;

public class UnicastRemoteStub
	extends RemoteStub {

public static void setStubRef(RemoteStub stub, RemoteRef ref) {
	setRef(stub, ref);
}

}
