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

private InetAddress address;	// remote address if connected, else null
private int port;		// remote port

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
	init(port, bindAddr);
}

protected void init(int port, InetAddress bindAddr) throws SocketException {
	impl.bind(port, bindAddr);
}

public synchronized void close() {
	impl.close();
}

public synchronized void connect(InetAddress address, int port) {
	if (port < 0 || port > 65535) {
		throw new IllegalArgumentException();
	}
	checkRemote(address, port);
	this.address = address;
	this.port = port;
}

public synchronized void disconnect() {
	this.address = null;
	this.port = 0;
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
		while (true) {
			impl.receive(p);
			if (this.address == null) {
				System.getSecurityManager().checkAccept(
				    p.getAddress().getHostName(), p.getPort());
				break;
			} else if (this.address.equals(p.getAddress())
			    && this.port == p.getPort()) {
				break;
			}
		}
	}
}

public void send(DatagramPacket p) throws IOException  {
	synchronized (p) {
		if (this.address == null) {		// not connected
			if (p.getAddress() == null
			    || p.getPort() == -1) {
				throw new IOException("no destination");
			}
			checkRemote(p.getAddress(), p.getPort());
		} else if (p.getAddress() == null) {
			p.setAddress(this.address);
			p.setPort(this.port);
		} else {
			if (!p.getAddress().equals(this.address)
			    || this.port != p.getPort()) {
				throw new IllegalArgumentException("address mismatch");
			}
		}
		impl.send(p);
	}
}

private void checkRemote(InetAddress addr, int port) {
	if (addr.isMulticastAddress()) {
		System.getSecurityManager().checkMulticast(addr);
	} else {
		System.getSecurityManager().checkConnect(addr.getHostName(), port);
	}
}

}
