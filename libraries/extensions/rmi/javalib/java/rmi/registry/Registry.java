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

public static int REGISTRY_PORT = 1099;

public Remote lookup(String name) throws RemoteException, NotBoundException, AccessException;

public void bind(String name, Remote obj) throws RemoteException, AlreadyBoundException, AccessException;

public void unbind(String name) throws RemoteException, NotBoundException, AccessException;

public void rebind(String name, Remote obj) throws RemoteException, AccessException;

public String[] list() throws RemoteException, AccessException;

}
