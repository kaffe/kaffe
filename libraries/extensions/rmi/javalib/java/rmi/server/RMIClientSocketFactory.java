/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.rmi.server;

import java.net.Socket;
import java.io.IOException;

public interface RMIClientSocketFactory {

public Socket createSocket(String host, int port) throws IOException;

}
