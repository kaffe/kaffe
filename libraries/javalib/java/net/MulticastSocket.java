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

import java.io.IOException;

public class MulticastSocket
  extends DatagramSocket {

private InetAddress iface;

public MulticastSocket() throws IOException {
	this(0);
}

public MulticastSocket(int port) throws IOException {
	super(port, null);
	iface = InetAddress.getLocalHost();
}

protected void init(int port, InetAddress bindAddr) throws SocketException {
	impl.setOption(SocketOptions.SO_REUSEADDR, new Boolean(true));
	super.init(port, bindAddr);
}

public InetAddress getInterface() throws SocketException {
	return (iface);
}

public byte getTTL() throws IOException {
	return (impl.getTTL());
}

public void joinGroup(InetAddress mcastaddr) throws IOException {
	impl.join(mcastaddr);
}

public void leaveGroup(InetAddress mcastaddr) throws IOException {
	impl.leave(mcastaddr);
}

public synchronized void send(DatagramPacket p, byte ttl) throws IOException {
	byte ottl = getTTL();
	setTTL(ttl);
	super.send(p);
	setTTL(ottl);
}

public void setInterface(InetAddress inf) throws SocketException {
	iface = inf;
}

public void setTTL(byte ttl) throws IOException {
	impl.setTTL(ttl);
}

}
