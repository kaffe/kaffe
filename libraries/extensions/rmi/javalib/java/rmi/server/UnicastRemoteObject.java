/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.rmi.server;

import java.rmi.RemoteException;
import java.rmi.Remote;
import java.rmi.server.RemoteRef;
import java.rmi.NoSuchObjectException;
import kaffe.rmi.server.UnicastServerRef;

public class UnicastRemoteObject
	extends RemoteServer {

protected UnicastRemoteObject() throws RemoteException {
	this(0);
}

protected UnicastRemoteObject(int port) throws RemoteException {
	this(port, RMISocketFactory.getSocketFactory(), RMISocketFactory.getSocketFactory());
}

protected UnicastRemoteObject(int port, RMIClientSocketFactory csf, RMIServerSocketFactory ssf) throws RemoteException {
	super(new UnicastServerRef(new ObjID(), port, ssf));
}

protected UnicastRemoteObject(RemoteRef ref) throws RemoteException {
	super((UnicastServerRef)ref);
}

public Object clone() throws CloneNotSupportedException {
	throw new kaffe.util.NotImplemented();
}

public static RemoteStub exportObject(Remote obj) throws RemoteException {
	UnicastServerRef sref = (UnicastServerRef)((RemoteObject)obj).getRef();
	return (sref.exportObject(obj));
}

public static Remote exportObject(Remote obj, int port) throws RemoteException {
	return (exportObject(obj));
}

public static Remote exportObject(Remote obj, int port, RMIClientSocketFactory csf, RMIServerSocketFactory ssf) throws RemoteException {
	return (exportObject(obj));
}

public static boolean unexportObject(Remote obj, boolean force) throws NoSuchObjectException {
	throw new kaffe.util.NotImplemented();
}

}
