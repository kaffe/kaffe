package java.net;

import java.io.FileDescriptor;
import java.io.IOException;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class DatagramSocket
{
	private int localPort;
	private DatagramSocketImpl impl;

static {
	System.loadLibrary("net");
}

public DatagramSocket() throws SocketException {
	this(0);
}

public DatagramSocket(int port) throws SocketException {
	System.getSecurityManager().checkListen(port);

	impl = new PlainDatagramSocketImpl();
	impl.create();
	localPort=port;
	impl.bind(localPort, InetAddress.anyLocalAddress);
}

public synchronized void close() {
	impl.close();
}

public void setSoTimeout(boolean on, int timeout) throws SocketException {
	impl.setOption(SocketOptions.SO_TIMEOUT, new Integer(on ? timeout : 0));
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
	return localPort;
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
