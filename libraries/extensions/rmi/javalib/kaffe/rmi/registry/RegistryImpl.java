/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package kaffe.rmi.registry;

import java.rmi.registry.Registry;
import java.rmi.RemoteException;
import java.rmi.NotBoundException;
import java.rmi.AccessException;
import java.rmi.AlreadyBoundException;
import java.rmi.Remote;
import java.rmi.server.UnicastRemoteObject;
import java.rmi.server.ObjID;
import java.util.Hashtable;
import java.util.Enumeration;
import java.rmi.server.RMISocketFactory;
import java.rmi.server.RMIClientSocketFactory;
import java.rmi.server.RMIServerSocketFactory;
import kaffe.rmi.server.UnicastServerRef;

public class RegistryImpl
	extends UnicastRemoteObject implements Registry {

private Hashtable bindings = new Hashtable();

public RegistryImpl(int port) throws RemoteException {
	this(port, RMISocketFactory.getSocketFactory(), RMISocketFactory.getSocketFactory());
}

public RegistryImpl(int port, RMIClientSocketFactory cf, RMIServerSocketFactory sf) throws RemoteException {
	super(new UnicastServerRef(new ObjID(ObjID.REGISTRY_ID), port, sf));
}

public Remote lookup(String name) throws RemoteException, NotBoundException, AccessException {
	Object obj = bindings.get(name);
	if (obj == null) {
		throw new NotBoundException(name);
	}
	return ((Remote)obj);
}

public void bind(String name, Remote obj) throws RemoteException, AlreadyBoundException, AccessException {
	if (bindings.containsKey(name)) {
		throw new AlreadyBoundException(name);
	}
	bindings.put(name, obj);
}

public void unbind(String name) throws RemoteException, NotBoundException, AccessException {
	Object obj = bindings.remove(name);
	if (obj == null) {
		throw new NotBoundException(name);
	}
}

public void rebind(String name, Remote obj) throws RemoteException, AccessException {
	bindings.put(name, obj);
}

public String[] list() throws RemoteException, AccessException {
	int size = bindings.size();
	String[] strings = new String[size];
	Enumeration e = bindings.keys();
	for (int i = 0; i < size; i++) {
		strings[i] = (String)e.nextElement();
	}
	return (strings);
}

public static void main(String[] args) {
	int port = Registry.REGISTRY_PORT;
	if (args.length > 0) {
		try {
			port = Integer.parseInt(args[0]);
		}
		catch (NumberFormatException _) {
			System.err.println("Bad port number - using default");
		}
	}

	try {
		RegistryImpl impl = new RegistryImpl(port);
		((UnicastServerRef)impl.getRef()).exportObject(impl);
	}
	catch (RemoteException _) {
		System.err.println("Registry failed");
	}
}

}
