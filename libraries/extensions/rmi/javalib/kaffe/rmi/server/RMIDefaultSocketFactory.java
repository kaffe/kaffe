/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.rmi.server;

import java.rmi.server.RMISocketFactory;
import java.io.IOException;
import java.net.Socket;
import java.net.ServerSocket;

public class RMIDefaultSocketFactory
	extends RMISocketFactory {

public RMIDefaultSocketFactory() {
}

public Socket createSocket(String host, int port) throws IOException {
	return (new Socket(host, port));
}

public ServerSocket createServerSocket(int port) throws IOException {
	return (new ServerSocket(port));
}

}
