/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

import java.lang.reflect.Method;
import java.io.Externalizable;
import java.rmi.Remote;
import java.rmi.RemoteException;
import java.io.ObjectOutput;

public interface RemoteRef
	extends Externalizable {

    long serialVersionUID = 0;
    String packagePrefix = "kaffe.rmi.server";

    void invoke(RemoteCall call) throws Exception;
    Object invoke(Remote obj, Method method, Object[] params, long opnum) throws Exception;
    RemoteCall newCall(RemoteObject obj, Operation[] op, int opnum, long hash) throws RemoteException;
    void done(RemoteCall call) throws RemoteException;
    boolean remoteEquals(RemoteRef ref);
    int remoteHashCode();
    String getRefClass(ObjectOutput out);
    String remoteToString();

}
