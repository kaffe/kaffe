/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.server;

import java.net.ServerSocket;
import java.io.IOException;

public interface RMIServerSocketFactory {

public ServerSocket createServerSocket(int port) throws IOException;

}
