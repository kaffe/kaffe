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
private NetworkInterface ni;

public MulticastSocket() throws IOException {
	this(0);
}

public MulticastSocket(int port) throws IOException {
	super(port, null);
}

private static void checkMulticastAddress(InetAddress addr) throws IOException {
	if (addr == null)
	{
		throw new IllegalArgumentException("Null multicast address");
	}
	if (!addr.isMulticastAddress())
	{
		throw new IOException("InetAddress "
				      + addr
				      + " is not a multicast address");
	}
	
        SecurityManager sm = System.getSecurityManager();
        if (sm != null)
	{
                sm.checkMulticast(addr);
	}
}

private static void checkMulticastAddress(SocketAddress addr)
	throws IOException
{
	InetSocketAddress isa;
	
	if( !(addr instanceof InetSocketAddress) )
	{
		throw new IllegalArgumentException(
			"Unknown SocketAddress type: "
			+ addr.getClass().getName());
	}

	isa = (InetSocketAddress)addr;
	checkMulticastAddress(isa.getAddress());
}

protected void init(int port, InetAddress bindAddr) throws SocketException {
	impl.setOption(SocketOptions.SO_REUSEADDR, new Boolean(true));
	super.init(port, bindAddr);
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
	impl.joinGroup(new InetSocketAddress(mcastaddr, 0), this.ni);
}

public void leaveGroup(InetAddress mcastaddr) throws IOException {
        checkMulticastAddress(mcastaddr);
	impl.leaveGroup(new InetSocketAddress(mcastaddr, 0), this.ni);
}

public void joinGroup(SocketAddress mcastaddr,
		      NetworkInterface netIf) throws IOException {
        checkMulticastAddress(mcastaddr);
	impl.joinGroup(mcastaddr, netIf);
}

public void leaveGroup(SocketAddress mcastaddr,
		       NetworkInterface netIf) throws IOException {
        checkMulticastAddress(mcastaddr);
	impl.leaveGroup(mcastaddr, netIf);
}

public synchronized void send(DatagramPacket p, byte ttl) throws IOException {
        checkMulticastAddress(p.getAddress());
	byte ottl = getTTL();
	setTTL(ttl);
	super.send(p);
	setTTL(ottl);
}

public void setInterface(InetAddress inf) throws SocketException {
	this.iface = inf;
	this.ni = NetworkInterface.getByInetAddress(inf);
	impl.setOption(SocketOptions.IP_MULTICAST_IF, this.iface);
}

public InetAddress getInterface() throws SocketException {
	return this.iface;
}

public void setNetworkInterface(NetworkInterface ni) throws SocketException {
	this.ni = ni;
	this.iface = this.ni.getPrimaryAddress();
	impl.setOption(SocketOptions.IP_MULTICAST_IF, this.iface);
}

public NetworkInterface getNetworkInterface() {
	return this.ni;
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
