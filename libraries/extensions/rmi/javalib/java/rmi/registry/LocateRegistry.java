/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.registry;

import java.io.IOException;
import java.rmi.RemoteException;
import java.rmi.server.RMIClientSocketFactory;
import java.rmi.server.RMIServerSocketFactory;
import java.rmi.server.RMISocketFactory;
import java.rmi.server.RemoteRef;
import java.rmi.server.ObjID;
import java.net.Socket;

import kaffe.rmi.server.UnicastRef;
import kaffe.rmi.server.UnicastServerRef;
import kaffe.rmi.registry.RegistryImpl;
import kaffe.rmi.registry.RegistryImpl_Stub;

public final class LocateRegistry {

public static Registry getRegistry() throws RemoteException {
	return (getRegistry("localhost", Registry.REGISTRY_PORT));
}

public static Registry getRegistry(int port) throws RemoteException {
	return (getRegistry("localhost", port));
}

public static Registry getRegistry(String host) throws RemoteException {
	return (getRegistry(host, Registry.REGISTRY_PORT));
}

public static Registry getRegistry(String host, int port) throws RemoteException {
	return (getRegistry(host, port, RMISocketFactory.getSocketFactory()));
}

public static Registry getRegistry(String host, int port, RMIClientSocketFactory csf) throws RemoteException {
	RemoteRef ref = new UnicastRef(new ObjID(ObjID.REGISTRY_ID), host, port, csf);
	return (new RegistryImpl_Stub(ref));
}

public static Registry createRegistry(int port) throws RemoteException {
	return (createRegistry(port, RMISocketFactory.getSocketFactory(), RMISocketFactory.getSocketFactory()));
}

public static Registry createRegistry(int port, RMIClientSocketFactory csf, RMIServerSocketFactory ssf) throws RemoteException {
	RegistryImpl impl = new RegistryImpl(port, csf, ssf);
	((UnicastServerRef)impl.getRef()).exportObject(impl);
	return (impl);
}

}
