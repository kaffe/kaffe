/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package kaffe.rmi.dgc;

import java.rmi.dgc.DGC;
import java.rmi.dgc.Lease;
import java.rmi.dgc.VMID;
import java.rmi.server.ObjID;
import java.rmi.RemoteException;
import java.rmi.server.UnicastRemoteObject;
import java.rmi.server.RMISocketFactory;
import kaffe.rmi.server.UnicastServerRef;

public class DGCImpl
	extends UnicastRemoteObject implements DGC {

public DGCImpl() throws RemoteException {
	super(new UnicastServerRef(new ObjID(ObjID.DGC_ID), 0, RMISocketFactory.getSocketFactory()));
}

public Lease dirty(ObjID[] ids, long sequenceNum, Lease lease) throws RemoteException {
	System.out.println("DGCImpl.dirty - not implemented");
	return (lease);
}

public void clean(ObjID[] ids, long sequenceNum, VMID vmid, boolean strong) throws RemoteException {
	System.out.println("DGCImpl.clean - not implemented");
}

}
