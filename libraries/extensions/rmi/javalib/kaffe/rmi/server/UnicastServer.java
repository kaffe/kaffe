/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package kaffe.rmi.server;

import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.ObjectInputStream;
import java.io.ObjectOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.util.Hashtable;
import java.net.UnknownHostException;
import java.rmi.server.ObjID;
import java.rmi.server.UnicastRemoteObject;
import java.rmi.server.UID;
import java.rmi.server.RemoteRef;
import java.rmi.RemoteException;
import java.rmi.NoSuchObjectException;
import kaffe.rmi.dgc.DGCImpl;

public class UnicastServer
	implements ProtocolConstants {

static private Hashtable objects = new Hashtable();
static private DGCImpl dgc;

public static void exportObject(UnicastServerRef obj) {
	startDGC();
	objects.put(obj.objid, obj);
	obj.manager.startServer();
}

private static synchronized void startDGC() {
	if (dgc == null) {
		try {
			dgc = new DGCImpl();
			((UnicastServerRef)dgc.getRef()).exportObject(dgc);
		}
		catch (RemoteException e) {
			e.printStackTrace();
		}
	}
}

public static void dispatch(UnicastConnection conn) throws Exception {
	switch (conn.getDataInputStream().readUnsignedByte()) {
	case MESSAGE_CALL:
		incomingMessageCall(conn);
		break;
	default:
		throw new Exception("bad method type");
	}
}

private static void incomingMessageCall(UnicastConnection conn) throws IOException {
	ObjectInputStream in = conn.getObjectInputStream();

	ObjID objid = ObjID.read(in);
	int method = in.readInt();
	long hash = in.readLong();

//System.out.println("ObjID: " + objid + ", method: " + method + ", hash: " + hash);

	// Use the objid to locate the relevant UnicastServerRef
	UnicastServerRef uref = (UnicastServerRef)objects.get(objid);
	Object returnval;
	int returncode = RETURN_ACK;
	if (uref != null) {
		try {
			// Dispatch the call to it.
			returnval = uref.incomingMessageCall(conn, method, hash);
		}
		catch (Exception e) {
			returnval = e;
			returncode = RETURN_NACK;
		}
	}
	else {
		returnval = new NoSuchObjectException("");
		returncode = RETURN_NACK;
	}

	conn.getDataOutputStream().writeByte(MESSAGE_CALL_ACK);

	ObjectOutputStream out = conn.getObjectOutputStream();

	out.writeByte(returncode);
	(new UID()).write(out);
	out.writeObject(returnval);

	out.flush();
}

}
