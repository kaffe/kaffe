/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

import java.io.OutputStream;
import java.io.PrintStream;

public abstract class RemoteServer
	extends RemoteObject {

protected RemoteServer() {
	super();
}

protected RemoteServer(RemoteRef ref) {
	super(ref);
}

public static String getClientHost() throws ServerNotActiveException {
	throw new kaffe.util.NotImplemented();
}

public static void setLog(OutputStream out) {
	throw new kaffe.util.NotImplemented();
}

public static PrintStream getLog() {
	throw new kaffe.util.NotImplemented();
}

}
