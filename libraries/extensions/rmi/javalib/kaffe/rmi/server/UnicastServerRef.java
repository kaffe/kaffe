/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.rmi.server;

import java.net.ServerSocket;
import java.net.Socket;
import java.net.InetAddress;
import java.net.UnknownHostException;
import java.rmi.Remote;
import java.rmi.RemoteException;
import java.rmi.server.RemoteStub;
import java.rmi.server.ObjID;
import java.rmi.server.ServerRef;
import java.rmi.server.RemoteRef;
import java.rmi.server.ServerNotActiveException;
import java.rmi.server.RMIClientSocketFactory;
import java.rmi.server.RMIServerSocketFactory;
import java.rmi.server.UID;
import java.rmi.server.Skeleton;
import java.rmi.server.RemoteCall;
import java.lang.reflect.Constructor;
import java.lang.reflect.Method;
import java.lang.reflect.InvocationTargetException;
import java.lang.Thread;
import java.lang.Exception;
import java.io.IOException;
import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import kaffe.util.LongHashtable;

public class UnicastServerRef
	extends UnicastRef {

final static private Class[] stubprototype = new Class[] { RemoteRef.class };

Remote myself;
private Skeleton skel;
private RemoteStub stub;
private LongHashtable methods;

public UnicastServerRef(ObjID id, int port, RMIServerSocketFactory ssf) {
	super(id);
	manager = UnicastConnectionManager.getInstance(port, ssf);
}

public RemoteStub exportObject(Remote obj) throws RemoteException {
	if (myself == null) {
		myself = obj;

		// Find and install the stub
		Class cls = obj.getClass();
		stub = (RemoteStub)getHelperClass(cls, "_Stub");
		if (stub == null) {
			throw new RemoteException("failed to export: " + cls);
		}

		// Find and install the skeleton (if there is one)
		skel = (Skeleton)getHelperClass(cls, "_Skel");

		// Build hash of methods which may be called.
		buildMethodHash(obj.getClass());

		// Export it.
		UnicastServer.exportObject(this);
	}

	return (stub);
}

private Object getHelperClass(Class cls, String type) {
	try {   
		String classname = cls.getName();
		Class scls = Class.forName(classname + type);
		if (type.equals("_Stub")) {
			try {
				// JDK 1.2 stubs
				Constructor con = scls.getConstructor(stubprototype);
				return (con.newInstance(new Object[]{this}));
			}
			catch (NoSuchMethodException e) {
			}
			catch (InstantiationException e) {
			}
			catch (IllegalAccessException e) {
			}
			catch (IllegalArgumentException e) {
			}
			catch (InvocationTargetException e) {
			}
			// JDK 1.1 stubs
			RemoteStub stub = (RemoteStub)scls.newInstance();
			UnicastRemoteStub.setStubRef(stub, this);
			return (stub);
		}
		else {
			// JDK 1.1 skel
			return (scls.newInstance());
		}
	}
	catch (ClassNotFoundException e) {
	}
	catch (InstantiationException e) {
	}
	catch (IllegalAccessException e) {
	}
	return (null);
}

public String getClientHost() throws ServerNotActiveException {
	throw new kaffe.util.NotImplemented();
}

private void buildMethodHash(Class cls) {
	methods = new LongHashtable();
	Method[] meths = cls.getMethods();
	for (int i = 0; i < meths.length; i++) {
		/* Don't need to include any java.xxx related stuff */
		if (meths[i].getDeclaringClass().getName().startsWith("java.")) {
			continue;
		}
		long hash = RMIHashes.getMethodHash(meths[i]);
		methods.put(hash, meths[i]);
//System.out.println("meth = " + meths[i] + ", hash = " + hash);
	}
}

public Object incomingMessageCall(UnicastConnection conn, int method, long hash) throws Exception {
//System.out.println("method = " + method + ", hash = " + hash);
	// If method is -1 then this is JDK 1.2 RMI - so use the hash
	// to locate the method
	if (method == -1) {
		Method meth = (Method)methods.get(hash);
//System.out.println("class = " + myself.getClass() + ", meth = " + meth);
		if (meth == null) {
			throw new NoSuchMethodException();
		}

		ObjectInputStream in = conn.getObjectInputStream();
		int nrargs = meth.getParameterTypes().length;
		Object[] args = new Object[nrargs];
		for (int i = 0; i < nrargs; i++) {
			/** 
			 * For debugging purposes - we don't handle CodeBases
			 * quite right so we don't always find the stubs.  This
			 * lets us know that.
			 */
			try {
				args[i] = in.readObject();
			}
			catch (Exception t) {
				t.printStackTrace();
				throw t;
			}
		}
		return (meth.invoke(myself, args));
	}
	// Otherwise this is JDK 1.1 style RMI - we find the skeleton
	// and invoke it using the method number.  We wrap up our
	// connection system in a UnicastRemoteCall so it appears in a
	// way the Skeleton can handle.
	else {
		if (skel == null) {
			throw new NoSuchMethodException();
		}
		UnicastRemoteCall call = new UnicastRemoteCall(conn);
		skel.dispatch(myself, call, method, hash);
		return (call.returnValue());
	}
}

}
