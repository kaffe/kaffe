/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

import java.rmi.Remote;
import java.rmi.server.RemoteCall;

public interface Skeleton {

public void dispatch(Remote obj, RemoteCall theCall, int opnum, long hash) throws Exception;
public Operation[] getOperations();

}
