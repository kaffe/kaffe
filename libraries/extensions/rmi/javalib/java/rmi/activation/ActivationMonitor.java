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
import java.rmi.MarshalledObject;

public interface ActivationMonitor
	extends Remote {

    void inactiveObject(ActivationID id) throws UnknownObjectException, RemoteException;
    void activeObject(ActivationID id, MarshalledObject obj) throws UnknownObjectException, RemoteException;
    void inactiveGroup(ActivationGroupID id, long incarnation) throws UnknownGroupException, RemoteException;

}
