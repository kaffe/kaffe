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

private static void checkMulticastAddress(InetAddress addr) throws IOException {
	if (!addr.isMulticastAddress())
		throw new IOException("InetAddress " + addr + " is not a multicast address");
	
        SecurityManager sm = System.getSecurityManager();
        if (sm != null)
                sm.checkMulticast(addr);
}

protected void init(int port, InetAddress bindAddr) throws SocketException {
	impl.setOption(SocketOptions.SO_REUSEADDR, new Boolean(true));
	super.init(port, bindAddr);
}

public InetAddress getInterface() throws SocketException {
	return (iface);
}

public int getTimeToLive() throws IOException {
	return getTTL();
}

/**
 * @deprecated.
 */
public byte getTTL() throws IOException {
	return (impl.getTTL());
}

public void joinGroup(InetAddress mcastaddr) throws IOException {
        checkMulticastAddress(mcastaddr);
	impl.join(mcastaddr);
}

public void leaveGroup(InetAddress mcastaddr) throws IOException {
        checkMulticastAddress(mcastaddr);
	impl.leave(mcastaddr);
}

public synchronized void send(DatagramPacket p, byte ttl) throws IOException {
        checkMulticastAddress(p.getAddress());
	byte ottl = getTTL();
	setTTL(ttl);
	super.send(p);
	setTTL(ottl);
}

public void setInterface(InetAddress inf) throws SocketException {
	iface = inf;
}

public void setTimeToLive(int ttl)  throws IOException {
	if (ttl < 0 || ttl > 255) {
		throw new IllegalArgumentException("ttl out of range");
	}

	setTTL((byte) ttl);
}

/**
 * @deprecated.
 */
public void setTTL(byte ttl) throws IOException {
	impl.setTTL(ttl);
}

}
