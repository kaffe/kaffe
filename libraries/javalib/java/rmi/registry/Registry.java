/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.registry;

import java.rmi.RemoteException;
import java.rmi.NotBoundException;
import java.rmi.AccessException;
import java.rmi.AlreadyBoundException;
import java.rmi.Remote;

public interface Registry
	extends Remote {

    int REGISTRY_PORT = 1099;

    Remote lookup(String name) throws RemoteException, NotBoundException, AccessException;

    void bind(String name, Remote obj) throws RemoteException, AlreadyBoundException, AccessException;

    void unbind(String name) throws RemoteException, NotBoundException, AccessException;

    void rebind(String name, Remote obj) throws RemoteException, AccessException;

    String[] list() throws RemoteException, AccessException;

}
