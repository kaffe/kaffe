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

/**
 * Receive buffer.
 */
public DatagramPacket(byte ibuf[], int ilength) {
	buf = ibuf;
	if (ilength < buf.length) {
		length = ilength;
	}
	else {
		length = buf.length;
	}
	try {
		address = InetAddress.getLocalHost();
	}
	catch (UnknownHostException _) {
		address = InetAddress.getLoopback();
	}
	port = -1;
}

/**
 * Send buffer.
 */
public DatagramPacket(byte ibuf[], int ilength, InetAddress iaddr, int iport) {
	buf = ibuf;
	if (ilength < buf.length) {
		length = ilength;
	}
	else {
		length = buf.length;
	}
	address = iaddr;
	port = iport;
}

public synchronized InetAddress getAddress() {
	if (port == -1) {
		return (null);
	}
	else {
		return (address);
	}
}

public synchronized byte[] getData() {
	return (buf);
}

public synchronized int getLength() {
	if (port == -1) {
		return (0);
	}
	else {
		return (length);
	}
}

public synchronized int getPort() {
	return (port);
}

public synchronized void setAddress(InetAddress addr) {
	address = addr;
}

public synchronized void setData(byte[] newbuf) {
	buf = newbuf;
}

public synchronized void setLength(int newlen) {
	if (newlen > buf.length) {
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
