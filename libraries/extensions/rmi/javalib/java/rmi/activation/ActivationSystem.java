/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.activation;

import java.rmi.Remote;
import java.rmi.RemoteException;

public interface ActivationSystem
	extends Remote {

public static final int SYSTEM_PORT = 0; // XXX

public ActivationID registerObject(ActivationDesc desc) throws ActivationException, UnknownGroupException, RemoteException;
public void unregisterObject(ActivationID id) throws ActivationException, UnknownObjectException, RemoteException;
public ActivationGroupID registerGroup(ActivationGroupDesc desc) throws ActivationException, RemoteException;
public ActivationMonitor activeGroup(ActivationGroupID id, ActivationInstantiator group, long incarnation) throws UnknownGroupException, ActivationException, RemoteException;
public void unregisterGroup(ActivationGroupID id) throws ActivationException, UnknownGroupException, RemoteException;
public void shutdown() throws RemoteException;
public ActivationDesc setActivationDesc(ActivationID id, ActivationDesc desc) throws ActivationException, UnknownObjectException, UnknownGroupException, RemoteException;
public ActivationGroupDesc setActivationGroupDesc(ActivationGroupID id, ActivationGroupDesc desc) throws ActivationException, UnknownGroupException, RemoteException;
public ActivationDesc getActivationDesc(ActivationID id) throws ActivationException, UnknownObjectException, RemoteException;
public ActivationGroupDesc getActivationGroupDesc(ActivationGroupID id) throws ActivationException, UnknownGroupException, RemoteException;

}
