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

import java.io.Serializable;
import java.util.StringTokenizer;

final public class InetAddress
  implements Serializable {

private static InetAddressImpl impl;

private String hostName;
private int address;
private int family;

static {
	impl = new InetAddressImpl();
}

private InetAddress(String name, int addr) {
	family = impl.getInetFamily();
	hostName = name;
	address = addr;
}

InetAddress() {
}

public boolean equals(Object obj) {

	if (this == obj) {
		return (true);
	}
	if (!(obj instanceof InetAddress)) {
		return (false);
	}

	InetAddress that = (InetAddress)obj;
	byte[] thatAddr = that.getAddress();
	byte[] thisAddr = getAddress();

	for (int pos = thisAddr.length - 1; pos >= 0; pos--) {
		if (thisAddr[pos] != thatAddr[pos]) {
			return (false);
		}
	}
	if (!getHostName().equals(that.getHostName())) {
		return (false);
	}
	return (true);
}

public byte[] getAddress() {
	byte result[] = new byte[4];

	result[0] = (byte)((address >> 24) & 0xFF);
	result[1] = (byte)((address >> 16) & 0xFF);
	result[2] = (byte)((address >>  8) & 0xFF);
	result[3] = (byte) (address        & 0xFF);

	return (result);
}

public static InetAddress[] getAllByName(String host) throws UnknownHostException {
	if (host == null || host.equals("")) {
		throw new UnknownHostException(host == null ? "" : host);
	}

	InetAddress[] addrs;
	if (Character.isDigit(host.charAt(0))) {
		addrs = new InetAddress[1];
		addrs[0] = getByName(host);
	}
	else {
		int data[] = impl.lookupAllHostAddr(host);
		int nr = data.length;
		addrs = new InetAddress[nr];
		for (int i = 0; i < nr; i++) {
			addrs[i] = new InetAddress(host, data[i]);
		}
	}
	return (addrs);
}

public static synchronized InetAddress getByName(String host) throws UnknownHostException {
	if (host == null) {
		return (InetAddress.getLoopback());
	}
	int ip;
	if (!Character.isDigit(host.charAt(0))) {
		ip = impl.lookupHostAddr(host);
	}
	else {
		ip = 0;
		StringTokenizer tok = new StringTokenizer(host, ".");
		while (tok.hasMoreElements()) {
			String s = tok.nextToken();
			ip <<= 8;
			try {
				ip |= Integer.parseInt(s);
			}
			catch (NumberFormatException _) {
			}
		}
		host = null;
	}
	return (new InetAddress(host, ip));
}

public String getHostAddress() {
	byte b[] = getAddress();
	StringBuffer buf = new StringBuffer();
	buf.append(Integer.toString(b[0] & 0xFF));
	buf.append(".");
	buf.append(Integer.toString(b[1] & 0xFF));
	buf.append(".");
	buf.append(Integer.toString(b[2] & 0xFF));
	buf.append(".");
	buf.append(Integer.toString(b[3] & 0xFF));
	return (buf.toString());
}

public String getHostName() {
	try {
		if (hostName == null) {
			hostName = impl.getHostByAddr(address);
		}
	}
	catch (UnknownHostException _) {
		hostName = getHostAddress();
	}
	return (hostName);
}

public static InetAddress getLocalHost() throws UnknownHostException {
	try {
		String name = impl.getLocalHostName();
		System.getSecurityManager().checkConnect(name, 0);
		return (getByName(name));
	}
	catch (SecurityException se) {
		return (InetAddress.getLoopback());
	}
}

public int hashCode() {
	return (address);
}

public boolean isMulticastAddress() {
	if ((address & 0xE0000000) == 0xE0000000) {
		return (true);
	}
	else {
		return (false);
	}
}

public String toString() {
	StringBuffer result = new StringBuffer();

	result.append(getHostName());
	result.append("/");
	result.append(getHostAddress());

	return (result.toString());;
}

static InetAddress getLoopback() {
	return (new InetAddress("loopback", 0x7F000001));
}

static InetAddress getAnyAddress() {
	InetAddress a = new InetAddress();
	impl.makeAnyLocalAddress(a);
	return (a);
}


}
