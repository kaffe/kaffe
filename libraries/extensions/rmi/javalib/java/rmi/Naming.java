/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi;

import java.net.MalformedURLException;
import java.net.URL;
import java.rmi.registry.Registry;
import java.rmi.registry.LocateRegistry;

public final class Naming {

public static Remote lookup(String name) throws NotBoundException, MalformedURLException, RemoteException {
	URL u = new URL("http:" + name);
	return (getRegistry(u).lookup(u.getFile().substring(1)));
}

public static void bind(String name, Remote obj) throws AlreadyBoundException, MalformedURLException, RemoteException {
	URL u = new URL("http:" + name);
	getRegistry(u).bind(u.getFile().substring(1), obj);
}

public static void unbind(String name) throws RemoteException, NotBoundException, MalformedURLException {
	URL u = new URL("http:" + name);
	getRegistry(u).unbind(u.getFile().substring(1));
}

public static void rebind(String name, Remote obj) throws RemoteException, MalformedURLException {
	URL u = new URL("http:" + name);
	getRegistry(u).rebind(u.getFile().substring(1), obj);
}

public static String[] list(String name) throws RemoteException, MalformedURLException {
	return (getRegistry(new URL("http:" + name)).list());
}

private static Registry getRegistry(URL u) throws RemoteException {
	if (u.getPort() == -1) {
		return (LocateRegistry.getRegistry(u.getHost()));
	}
	else {
		return (LocateRegistry.getRegistry(u.getHost(), u.getPort()));
	}
}

}
