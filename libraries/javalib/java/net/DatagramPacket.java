package java.net;


/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
final public class DatagramPacket
{
	private byte[] buf = null;
	private int length = 0;
	private InetAddress address = null;
	private int port = -1;

public DatagramPacket(byte ibuf[], int ilength) {
	this(ibuf, ilength, null, -1);
}

public DatagramPacket(byte ibuf[], int ilength, InetAddress iaddr, int iport) {
	buf=ibuf;
	length=Math.min(ilength, ibuf.length);
	address=iaddr;
	port=iport;
}

public InetAddress getAddress() {
	return address;
}

public byte[] getData() {
	return buf;
}

public int getLength() {
	return length;
}

public int getPort() {
	return port;
}
}
