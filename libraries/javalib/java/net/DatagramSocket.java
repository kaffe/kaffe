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
	private FileDescriptor fd = new FileDescriptor();

static {
	System.loadLibrary("net");
}

public DatagramSocket() throws SocketException {
	this(0);
}

public DatagramSocket(int port) throws SocketException {
	System.getSecurityManager().checkListen(port);

	datagramSocketCreate();
	localPort=port;
	datagramSocketBind(localPort);
}

public synchronized void close() {
	datagramSocketClose();
}

native private int datagramSocketBind(int port);

native private void datagramSocketClose();

native private void datagramSocketCreate();

native private int datagramSocketPeek(InetAddress iaddr);

native private void datagramSocketReceive(DatagramPacket pkt);

native private void datagramSocketSend(DatagramPacket pkt);

protected synchronized void finalize() {
	close();
}

public int getLocalPort() {
	return localPort;
}

public synchronized void receive(DatagramPacket p) throws IOException {
	System.getSecurityManager().checkConnect(p.getAddress().getHostName(), p.getPort());

	datagramSocketReceive(p);
}

public void send(DatagramPacket p) throws IOException  {
	System.getSecurityManager().checkConnect(p.getAddress().getHostName(), p.getPort());

	datagramSocketSend(p);
}
}
