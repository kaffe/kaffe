/*
 * Java comm library component.
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package javax.comm;

import java.util.Vector;
import java.util.Hashtable;
import java.util.Enumeration;
import java.io.FileDescriptor;

public class CommPortIdentifier {

public static final int PORT_SERIAL = 1;
public static final int PORT_PARALLEL = 2;

private static Hashtable portlist = new Hashtable();

private CommPort port;
private int type;
private String owner;
private Vector listeners;

private CommPortIdentifier(CommPort p, int t) {
	port = p;
	type = t;
	owner = null;
	listeners = new Vector(2);
}

public static Enumeration getPortIdentifiers() {
	return (portlist.elements());
}

public static CommPortIdentifier getPortIdentifier(String portName) throws NoSuchPortException {
	CommPortIdentifier c = (CommPortIdentifier)portlist.get(portName);
	if (c == null) {
		throw new NoSuchPortException();
	}
	return (c);
}

public static CommPortIdentifier getPortIdentifier(CommPort port) throws NoSuchPortException {
	return (getPortIdentifier(port.getName()));
}

public static void addPortName(String portName, int portType, CommDriver driver) {
	driver.initialize();
	CommPort port = driver.getCommPort(portName, portType);
	portlist.put(portName, new CommPortIdentifier(port, portType));
}

public String getName() {
	return (port.getName());
}

public int getPortType() {
	return (type);
}

public String getCurrentOwner() {
	return (owner);
}

public synchronized boolean isCurrentlyOwned() {
	return (owner != null);
}

public synchronized CommPort open(String appname, int timeout) throws PortInUseException {
	if (owner != null) {
		int len = listeners.size();
		for (int i = 0; i < len; i++) {
			((CommPortOwnershipListener)listeners.elementAt(i)).ownershipChange(CommPortOwnershipListener.PORT_OWNERSHIP_REQUESTED);
		}
		if (owner != null) {
			throw new PortInUseException(owner);
		}
	}
	owner = appname;

	return (port);
}

public CommPort open(FileDescriptor fd) throws UnsupportedCommOperationException {
	throw new UnsupportedCommOperationException();
}

public void addPortOwnershipListener(CommPortOwnershipListener listener) {
	listeners.addElement(listener);
}

public void removePortOwnershipListener(CommPortOwnershipListener listener) {
	listeners.removeElement(listener);
}

}
