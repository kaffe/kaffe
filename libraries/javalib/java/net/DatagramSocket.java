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
import kaffe.net.DefaultDatagramSocketImplFactory;

public class DatagramSocket {

static private DatagramSocketImplFactory factory = new DefaultDatagramSocketImplFactory();

protected DatagramSocketImpl impl;

public DatagramSocket() throws SocketException {
	this(0, null);
}

public DatagramSocket(int port) throws SocketException {
	this(port, null);
}

public DatagramSocket(int port, InetAddress bindAddr) throws SocketException {
	System.getSecurityManager().checkListen(port);
	if (bindAddr == null) {
		bindAddr = InetAddress.getAnyAddress();
	}
	impl = factory.createDatagramSocketImpl();
	impl.create();
	impl.bind(port, bindAddr);
}

public synchronized void close() {
	impl.close();
}

public void setSoTimeout(int timeout) throws SocketException {
	impl.setOption(SocketOptions.SO_TIMEOUT, new Integer(timeout));
}

public int getSoTimeout() throws SocketException {
	return ((Integer) impl.getOption(SocketOptions.SO_TIMEOUT)).intValue();
}

public int getLocalPort() {
	return (impl.getLocalPort());
}

public InetAddress getLocalAddress() {
	try {
		return ((InetAddress)impl.getOption(SocketOptions.SO_BINDADDR));
	} catch (Exception e) {
		return (InetAddress.getLoopback());
	}
}

public synchronized void receive(DatagramPacket p) throws IOException {
	synchronized (p) {
		impl.receive(p);
	}
}

public void send(DatagramPacket p) throws IOException  {
	System.getSecurityManager().checkConnect(p.getAddress().getHostName(), p.getPort());
	synchronized (p) {
		impl.send(p);
	}
}

}
