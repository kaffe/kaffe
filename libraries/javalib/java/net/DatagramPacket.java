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

final public class DatagramPacket {

private byte[] buf;
private int length;
private InetAddress address;
private int port;

private static final int DEFAULT_PORT = -1;
 
/**
 * Receive buffer.
 */
public DatagramPacket(byte ibuf[], int ilength) {

	this(ibuf, ilength, getDefaultAddress(), DEFAULT_PORT);
}

/**
 * Send buffer.
 */
public DatagramPacket(byte ibuf[], int ilength, InetAddress iaddr, int iport) {
	if (ilength < 0 || ilength > ibuf.length) {
		throw new IllegalArgumentException("illegal length or offset");
	}

	buf = ibuf;
	length = Math.min(ilength, ibuf.length);
	address = iaddr;
	port = iport;
}

public synchronized InetAddress getAddress() {
	return (address);
}

public synchronized byte[] getData() {
	return (buf);
}

private static InetAddress getDefaultAddress() {
	InetAddress addr;

	try {
		addr = InetAddress.getLocalHost();
	}
	catch (UnknownHostException e) {
		addr = InetAddress.getLoopback();
	}

	return addr;
}

public synchronized int getLength() {
        return (length);
}

public synchronized int getPort() {
	return (port);
}

public synchronized void setAddress(InetAddress addr) {
	address = addr;
}

public synchronized void setData(byte[] newbuf) {
	if (newbuf == null) {
		throw new NullPointerException();
	}
	buf = newbuf;
	length = Math.min(length, buf.length);
}

public synchronized void setLength(int newlen) {
	if (newlen < 0 || newlen > buf.length) {
		throw new IllegalArgumentException();
	}
	length = newlen;
}

public synchronized void setPort(int newport) {
	if (newport < 0 || newport > 65535) {
		throw new IllegalArgumentException();
	}
	port = newport;
}

}
