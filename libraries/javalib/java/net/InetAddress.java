package java.net;

import java.util.Hashtable;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
final public class InetAddress
{
	private String hostName;
	private int address;
	private int family;
	private static InetAddress localAddress;
	protected static InetAddress anyLocalAddress;
	private static InetAddressImpl impl;

static {
	System.loadLibrary("net");
	impl = new InetAddressImpl();

	byte local[] = {127, 0, 0, 1};
	localAddress = new InetAddress(bytesToInt(local));
	anyLocalAddress = new InetAddress();
	impl.makeAnyLocalAddress(anyLocalAddress);

}

public InetAddress() {}

private InetAddress(int addr)
	{
	address = addr;
	hostName = null;
	family = impl.getInetFamily();
}

private static int bytesToInt(byte addr[]) {
	int p1=((addr[0] & 0xFF)<<24);
	int p2=((addr[1] & 0xFF)<<16);
	int p3=((addr[2] & 0xFF)<<8);
	int p4=(addr[3] & 0xFF);

	int result=p1 | p2 | p3 | p4;

	return result;
}

public boolean equals(Object obj) {
	if ((obj!=null) && (obj instanceof InetAddress)) {
		byte[] thatAddr=((InetAddress)obj).getAddress();
		byte[] thisAddr=((InetAddress)this).getAddress();

		if (thatAddr.length==thisAddr.length) {
			for (int pos=0; pos<thisAddr.length; pos++) {
				if (thisAddr[pos]!=thatAddr[pos]) return false;
			}

			return (getHostName().equals(((InetAddress)obj).getHostName()));
		}
	}

	return false;
}

public byte[] getAddress() {
	if (this==localAddress) {
		return null;
	}
	else {
		return intToBytes(address);
	}
}

public static synchronized InetAddress[] getAllByName(String host) throws UnknownHostException
{
	if (host==null) {
		InetAddress result[]=new InetAddress[1];;
		result[0]=localAddress;

		return result;
	}
	else {
		byte data[][] = impl.lookupAllHostAddr(host);
		int numAddrs=data.length;

		InetAddress addrs[]=new InetAddress[numAddrs];
		for (int i=0; i<numAddrs; i++) {
			addrs[i]=new InetAddress(bytesToInt(data[i]));
		}

		return addrs;
	}
}

public static synchronized InetAddress getByName(String host) throws UnknownHostException
{
	if (host == null || host == "") {
		return (localAddress);
	}
	else {
		InetAddress result = new InetAddress(bytesToInt(impl.lookupHostAddr(host)));

		return (result);
	}
}

public String getHostAddress() {
	if (this==localAddress) {
		return null;
	}
	else {
		byte addr[]=intToBytes(address);
		return (addr[0]&0xFF)+"."+(addr[1]&0xFF)+"."+(addr[2]&0xFF)+"."+(addr[3]&0xFF);
	}
}

public String getHostName()
	{
	try {
		if (hostName == null) {
			hostName = impl.getHostByAddr(address);
		}
		return (hostName);
	}
	catch (UnknownHostException _) {
		return ("<unknown>");
	}
}

public static InetAddress getLocalHost() throws UnknownHostException
{
	String localHostname = impl.getLocalHostName();

	try {
		System.getSecurityManager().checkConnect(localHostname, -1);
	}
	catch (SecurityException se) {
		return localAddress;
	}

	return getByName(localHostname);
}

public int hashCode() {
	return address;
}

private static byte[] intToBytes(int addr) {
	byte result[] = new byte[4];

	result[0]=(byte )((addr & 0xFF000000 ) >> 24);
	result[1]=(byte )((addr & 0x00FF0000 ) >> 16);
	result[2]=(byte )((addr & 0x0000FF00 ) >> 8);
	result[3]=(byte )(addr & 0x000000FF );

	return result;
}

public String toString()
	{
	StringBuffer result = new StringBuffer();

	result.append(getHostName());
	result.append("/");

	/* Do DOT notation */
	byte thisAddr[] = intToBytes(address);

	for (int pos = 0; pos < thisAddr.length-1; pos++) {
		result.append(String.valueOf((int)(thisAddr[pos] & 0xFF)));
		result.append(".");
	}

	result.append(String.valueOf((int)(thisAddr[thisAddr.length-1] & 0xFF)));

	return (result.toString());;
}
}
