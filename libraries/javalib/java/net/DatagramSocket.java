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

public class DatagramSocket
{
	private DatagramSocketImpl impl;
	static Class DatagramSocketImplClass;

static {
	System.loadLibrary("net");
	/* set the socket implementation class to use by default */
	DatagramSocketImplClass = PlainDatagramSocketImpl.class;
}

/**
 * Create an unbound datagram socket with a user-specified datagram socket
 * implementation.  Note that the subclass is responsible for setting up 
 * the supplied implementation.
 *
 * This constructor is not part of Sun's API or implementation.  However,
 * it follows the model used in java.net.Socket.  Sun's Rosanna Lee has 
 * promised to revisit the issue in JDK 1.3.  For more information, see <a 
 * href="http://developer.javasoft.com/developer/bugParade/bugs/4027637.html">
 * here</a>.  Once 1.3 is published, we will change this class to obey the
 * yet to be defined model of using user-supplied DatagramSocketImpls.
 *
 * @author Godmar Back <gback@cs.utah.edu>
 */
protected DatagramSocket(DatagramSocketImpl impl) throws SocketException {
	this.impl = impl;
}

public DatagramSocket() throws SocketException {
	this(0, null);
}

public DatagramSocket(int port) throws SocketException {
	this(port, null);
}

public DatagramSocket(int port, InetAddress bindAddr) throws SocketException {
	System.getSecurityManager().checkListen(port);

	try {
	    impl = (DatagramSocketImpl)DatagramSocketImplClass.newInstance();
	} catch (Exception e) {
	    // transform failure into SocketException
	    throw new SocketException(
		"Couldn't create DatagramSocketImpl from class " +
		DatagramSocketImplClass.getName() + ":" + e);
	}
	impl.create();
	if (bindAddr == null)
	    bindAddr = InetAddress.anyLocalAddress;
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

protected synchronized void finalize() {
	close();
}

public int getLocalPort() {
	return impl.getLocalPort();
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

public synchronized void receive(DatagramPacket p) throws IOException {
	System.getSecurityManager().checkRead(impl.fd);	// is this correct?
	impl.receive(p);
}

public void send(DatagramPacket p) throws IOException  {
	System.getSecurityManager().checkConnect(p.getAddress().getHostName(), p.getPort());

	impl.send(p);
}
}
