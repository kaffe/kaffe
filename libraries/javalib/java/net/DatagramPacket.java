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
private int offset;
private int length;
private InetAddress address;
private int port;

private static final int DEFAULT_RECEIVE_PORT = -1;
private static final int MAX_PORT = 65535;
private static final int MIN_LENGTH = 0;
private static final int MIN_OFFSET = 0;
private static final int MIN_PORT = 0;

/**
 * Receive buffer.
 */
public DatagramPacket(byte ibuf[], int ilength) {
	this(ibuf, MIN_OFFSET, ilength);
}

public DatagramPacket(byte ibuf[], int ioffset, int ilength) {
	this.setData(ibuf, ioffset, ilength);
 	port = DEFAULT_RECEIVE_PORT;
	/* address remains null until the first packet is received. */
}

/**
 * Send buffer.
 */
public DatagramPacket(byte ibuf[], int ilength, InetAddress iaddr, int iport) {
	this(ibuf, MIN_OFFSET, ilength, iaddr, iport);
}

public DatagramPacket(byte ibuf[],
		      int ioffset,
		      int ilength,
		      InetAddress iaddr,
		      int iport) {
	this.setData(ibuf, ioffset, ilength);
	this.setAddress(iaddr);
	this.setPort(iport);
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

public synchronized int getOffset() {
	return (offset);
}

public synchronized int getPort() {
	return (port);
}

synchronized void prime() {
	this.address = new InetAddress();
}
 
public synchronized void setAddress(InetAddress addr) {
	address = addr;
}

public synchronized void setData(byte[] newbuf) {
	setData(newbuf, MIN_OFFSET, newbuf.length);
}

public synchronized void setData(byte[] newbuf, int off, int len) {
	if (off > newbuf.length - len || off < MIN_OFFSET || len < MIN_LENGTH) { 
		throw new IllegalArgumentException("illegal length or offset");
	}
	buf = newbuf;
	offset = off;
	length = Math.min(len, buf.length);
}
	
public synchronized void setLength(int ilength) {
	if ((offset + ilength <= buf.length) && (offset + ilength >= offset)) {
		length = ilength;
	}
	else {
		throw new IllegalArgumentException("illegal length");
	}
	length = ilength;
}

public synchronized void setPort(int newport) {
	if (newport < MIN_PORT || newport > MAX_PORT) {
		throw new IllegalArgumentException("Port out of range:" + newport);
	}
	port = newport;
}

}
