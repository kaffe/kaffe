/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.dgc;

import java.io.Serializable;
import java.rmi.server.UID;
import java.net.InetAddress;
import java.net.UnknownHostException;

public final class VMID
	implements Serializable {

static final long serialVersionUID = -538642295484486218L;
static final boolean areWeUnique;
static byte[] localAddr;

private byte[] addr;
private UID uid;

static {
	byte[] addr;
	boolean awu = true;
	try {
		addr = InetAddress.getLocalHost().getAddress();
		if (addr[0] == 127 && addr[1] == 0 && addr[2] == 0 && addr[3] == 1) {
			awu = false;
		}
	}
	catch (UnknownHostException _) {
		addr = new byte[]{ 127, 0, 0, 1 };
		awu = false;
	}
	localAddr = addr;
	areWeUnique = awu;
}

public VMID() {
	addr = localAddr;
	uid = new UID();
}

public static boolean isUnique() {
	return (areWeUnique);
}

public int hashCode() {
	return (super.hashCode());
}

public boolean equals(Object obj) {
	if (!(obj instanceof VMID)) {
		return (false);
	}
	VMID other = (VMID)obj;
	if (addr.length != other.addr.length) {
		return (false);
	}
	for (int i = addr.length - 1; i >= 0; i--) {
		if (addr[i] != other.addr[i]) {
			return (false);
		}
	}
	return (uid.equals(other.uid));
}

public String toString() {
	StringBuffer buf = new StringBuffer("[VMID: ");
	for (int i = 0; i < addr.length; i++) {
		if (i > 0) {
			buf.append(".");
		}
		buf.append(Integer.toString(addr[i]));
	}
	buf.append(" ");
	buf.append(uid.toString());
	buf.append("]");

	return (buf.toString());
}

}
