/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

import java.rmi.server.RemoteStub;
import java.rmi.Remote;
import java.rmi.RemoteException;
import java.rmi.server.ServerNotActiveException;

public interface ServerRef
	extends RemoteRef {

    long serialVersionUID = 0;

    RemoteStub exportObject(Remote obj, Object data) throws RemoteException;
    String getClientHost() throws ServerNotActiveException;

}
