/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

import java.net.Socket;
import java.net.ServerSocket;
import java.io.IOException;
import kaffe.rmi.server.RMIDefaultSocketFactory;

public abstract class RMISocketFactory
	implements RMIClientSocketFactory, RMIServerSocketFactory {

static private RMISocketFactory defaultFactory;
static private RMISocketFactory currentFactory;
static private RMIFailureHandler currentHandler;

static {
	defaultFactory = new RMIDefaultSocketFactory();
	currentFactory = defaultFactory;
}

public RMISocketFactory() {
}

public abstract Socket createSocket(String host, int port) throws IOException;

public abstract ServerSocket createServerSocket(int port) throws IOException;

public static void setSocketFactory(RMISocketFactory fac) throws IOException {
	currentFactory = fac;
}

public static RMISocketFactory getSocketFactory() {
	return (currentFactory);
}

public static RMISocketFactory getDefaultSocketFactory() {
	return (defaultFactory);
}

public static void setFailureHandler(RMIFailureHandler fh) {
	currentHandler = fh;
}

public static RMIFailureHandler getFailureHandler() {
	return (currentHandler);
}

}
