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

protected Socket() {
	if (factory==null) {
		impl=new PlainSocketImpl();
	}
	else {
		impl=factory.createSocketImpl();
	}
}

/**
 * Creates an unconnected Socket with a user-specified SocketImpl. 
 * The impl parameter is an instance of a SocketImpl the subclass wishes 
 * to use on the Socket.
 */
protected Socket(SocketImpl impl) throws SocketException {
	this.impl = impl;
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

public InetAddress getLocalAddress() {
	InetAddress localAddress;
	try {
		localAddress =
			(InetAddress)impl.getOption(SocketOptions.SO_BINDADDR); 
	} catch (Exception e) {
		localAddress = InetAddress.anyLocalAddress;
	}
	return localAddress;
}

public static synchronized void setSocketImplFactory(SocketImplFactory fac) throws IOException {
	factory=fac;
}

public void setSoLinger(boolean on, int timeout) throws SocketException {
	if (on) {
		impl.setOption(SocketOptions.SO_LINGER, new Integer(timeout));
	}
	else {
		impl.setOption(SocketOptions.SO_LINGER, new Boolean(on));
	}
}

public int getSoLinger() throws SocketException {
	return ((Integer) impl.getOption(SocketOptions.SO_LINGER)).intValue();
}

public void setSoTimeout(int timeout) throws SocketException {
	impl.setOption(SocketOptions.SO_TIMEOUT, new Integer(timeout));
}

public int getSoTimeout() throws SocketException {
	return ((Integer) impl.getOption(SocketOptions.SO_TIMEOUT)).intValue();
}

public void setSendBufferSize(int size) throws SocketException {
	impl.setOption(SocketOptions.SO_SNDBUF, new Integer(size));
}

public int getSendBufferSize() throws SocketException {
	return ((Integer) impl.getOption(SocketOptions.SO_SNDBUF)).intValue();
}

public void setReceiveBufferSize(int size) throws SocketException {
	impl.setOption(SocketOptions.SO_RCVBUF, new Integer(size));
}

public int getReceiveBufferSize() throws SocketException {
	return ((Integer) impl.getOption(SocketOptions.SO_RCVBUF)).intValue();
}

public void setTcpNoDelay(boolean on) throws SocketException {
	impl.setOption(SocketOptions.TCP_NODELAY, new Boolean(on));
}

public boolean getTcpNoDelay() throws SocketException {
	return ((Boolean) impl.getOption(SocketOptions.TCP_NODELAY)).booleanValue();
}

public String toString() {
	return impl.toString();
}
}
