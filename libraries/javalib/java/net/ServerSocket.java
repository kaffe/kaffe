/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.net;

import java.lang.String;
import java.io.FileDescriptor;
import java.io.IOException;
import kaffe.net.DefaultSocketImplFactory;
import java.net.SocketOptions;

public class ServerSocket {

private static SocketImplFactory factory = new DefaultSocketImplFactory();

private SocketImpl impl;

public ServerSocket(int port) throws IOException {
	this(port, 50);
}

public ServerSocket(int port, int backlog) throws IOException {
	this(port, backlog, null);
}

/**
 *  Create a server with the specified port, listen backlog, and local 
 * IP address to bind to. The bindAddr argument can be used on a 
 * multi-homed host for a ServerSocket that will only accept connect 
 * requests to one of its addresses.  If bindAddr is null, it will default 
 * accepting connections on any/all local addresses. The port must be 
 * between 0 and 65535, inclusive. 
 *
 * @param port 		the local TCP port
 * @param backlog 	the listen backlog
 * @param bindAddr 	the local InetAddress the server will bind to
 */
public ServerSocket(int port, int backlog, InetAddress bindAddr) throws IOException {
	System.getSecurityManager().checkListen(port);

	if (bindAddr == null) {
		bindAddr = InetAddress.getAnyAddress();
	}
	impl = factory.createSocketImpl();
	impl.create(true);
	impl.bind(bindAddr, port);
	impl.listen(backlog);
}

public Socket accept() throws IOException {
	Socket s = new Socket();
	s.impl.localport = impl.localport;
	implAccept(s);
	return (s);
}

protected final void implAccept(Socket s) throws IOException {
	impl.accept(s.impl);
	System.getSecurityManager().checkAccept(s.getInetAddress().getHostName(), s.getPort());
}

public void close() throws IOException {
	impl.close();
}

public InetAddress getInetAddress() {
        try {
                return ((InetAddress)impl.getOption(SocketOptions.SO_BINDADDR));
        }
        catch (SocketException e1) {
                try {
                        return (InetAddress.getLocalHost());
                }
                catch (UnknownHostException e2) {
                        return (InetAddress.getLoopback());
                }
        }
}

public int getLocalPort() {
	return (impl.getLocalPort());
}

public synchronized int getSoTimeout() throws IOException {
	return ((Integer) impl.getOption(SocketOptions.SO_TIMEOUT)).intValue();
}

public synchronized void setSoTimeout(int timeout) throws SocketException {
	impl.setOption(SocketOptions.SO_TIMEOUT, new Integer(timeout));
}

public static synchronized void setSocketFactory(SocketImplFactory fac) throws IOException {
	factory = fac;
}

public SocketImpl newSocketImpl() {
	return (impl);
}

public String toString() {
	return (impl.toString());
}

}
