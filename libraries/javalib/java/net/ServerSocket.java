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

import java.io.FileDescriptor;
import java.io.IOException;

public class ServerSocket
{
	private static SocketImplFactory factory = null;
	private SocketImpl impl;

ServerSocket() {
	if (factory==null) {
		impl=new PlainSocketImpl();
	}
	else {
		impl=factory.createSocketImpl();
	}
}

public ServerSocket(int port) throws IOException {
	this(port, 50, InetAddress.anyLocalAddress);
}

public ServerSocket(int port, int backlog) throws IOException {
	this(port, backlog, InetAddress.anyLocalAddress);
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
	this();

	System.getSecurityManager().checkListen(port);

	if (bindAddr == null)
		bindAddr = InetAddress.anyLocalAddress;
	impl.create(true);
	impl.bind(bindAddr, port);
	impl.listen(backlog);
}

public Socket accept() throws IOException {
	Socket result=new Socket();
	result.impl.fd=new FileDescriptor();
	result.impl.address=new InetAddress();

	impl.accept(result.impl);

	System.getSecurityManager().checkAccept(result.getInetAddress().getHostName(), result.getPort());

	return result;
}

public void close() throws IOException {
	impl.close();
}

public InetAddress getInetAddress() {
	return impl.getInetAddress();
}

public int getLocalPort() {
	return impl.getLocalPort();
}

public SocketImpl newSocketImpl() {
	return impl;
}

public static synchronized void setSocketFactory(SocketImplFactory fac) throws IOException {
	if (factory==null) {
		factory=fac;
	}
	else {
		throw new SocketException();
	}
}

public String toString() {
	return impl.toString();
}
}
