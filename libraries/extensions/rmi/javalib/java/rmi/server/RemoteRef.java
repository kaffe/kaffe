/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.rmi.server;

import java.lang.reflect.Method;
import java.io.Externalizable;
import java.rmi.Remote;
import java.rmi.RemoteException;
import java.io.ObjectOutput;

public interface RemoteRef
	extends Externalizable {

public static final long serialVersionUID = 0;
public static final String packagePrefix = "kaffe.rmi.server";

public void invoke(RemoteCall call) throws Exception;
public Object invoke(Remote obj, Method method, Object[] params, long opnum) throws Exception;
public RemoteCall newCall(RemoteObject obj, Operation[] op, int opnum, long hash) throws RemoteException;
public void done(RemoteCall call) throws RemoteException;
public boolean remoteEquals(RemoteRef ref);
public int remoteHashCode();
public String getRefClass(ObjectOutput out);
public String remoteToString();

}
