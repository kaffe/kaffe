package java.net;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
final public class Socket
{
	private static SocketImplFactory factory = null;
/* This must be a public member, since ServerSocket accept has to access it to setup the socket!
     This is NOT part of the SUN spec.. Thanks AGAIN SUN! */
	SocketImpl impl;

Socket() {
	if (factory==null) {
		impl=new PlainSocketImpl();
	}
	else {
		impl=factory.createSocketImpl();
	}
}

public Socket(InetAddress address, int port) throws IOException {
	this(address, port, true);
}

public Socket(InetAddress address, int port, boolean stream) throws IOException {
	this();

	impl.create(stream);
	impl.connect(address, port);
}

public Socket(String host, int port) throws IOException {
	this(host, port, true);
}

public Socket(String host, int port, boolean stream) throws IOException {
	this(InetAddress.getByName(host), port, stream);
}

public synchronized void close() throws IOException {
	impl.close();
}

public InetAddress getInetAddress() {
	return impl.getInetAddress();
}

public InputStream getInputStream() throws IOException {
	return impl.getInputStream();
}

public int getLocalPort() {
	return impl.getLocalPort();
}

public OutputStream getOutputStream() throws IOException {
	return impl.getOutputStream();
}

public int getPort() {
	return impl.getPort();
}

public static synchronized void setSocketImplFactory(SocketImplFactory fac) throws IOException {
	factory=fac;
}

public String toString() {
	return impl.toString();
}
}
