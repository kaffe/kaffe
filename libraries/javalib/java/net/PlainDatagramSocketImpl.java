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

/* XXX: This class should not be public!
 * It's public right now cause it's accessed from kaffe.net.
 */
public class PlainDatagramSocketImpl
  extends DatagramSocketImpl {

private int timeout;

static {
	System.loadLibrary("net");
}

protected void create() throws SocketException {
	datagramSocketCreate();
}

protected void close() {
	datagramSocketClose();
}

public void setOption(int option, Object data) throws SocketException {
	final boolean disable = (data instanceof Boolean) && !((Boolean) data).booleanValue();
	if (disable) {
		data = new Integer(0);			// the common case
	}
	switch (option) {
	case SO_SNDBUF:
	case SO_RCVBUF:
	case SO_LINGER:
	case SO_REUSEADDR:
		break;
	case SO_TIMEOUT:
		timeout = ((Integer)data).intValue();
		return;
	case SO_BINDADDR:
		throw new SocketException("Read-only socket option");
	case IP_MULTICAST_IF:
		if (disable) {			// makes no sense
			return;
		}
		data = (InetAddress) data;	// verify object type
		break;
	default:
		throw new SocketException("Unknown socket option");
	}
	socketSetOption(option, data);
}

public Object getOption(int option) throws SocketException {
	switch (option) {
	case SO_SNDBUF:
	case SO_RCVBUF:
	case SO_LINGER:
	case SO_REUSEADDR:
		return new Integer(socketGetOption(option));
	case SO_TIMEOUT:
		return new Integer(timeout);
	case SO_BINDADDR:
	case IP_MULTICAST_IF:
		int val = socketGetOption(option);
		try {
			return InetAddress.getByName(
			    ((int) ((val >> 24) & 0xff)) + "." +
			    ((int) ((val >> 16) & 0xff)) + "." +
			    ((int) ((val >>  8) & 0xff)) + "." +
			    ((int) ((val      ) & 0xff)) );
		} catch (UnknownHostException e) {
			throw new Error("impossible result");
		}
	default:
		throw new SocketException("Unknown socket option");
	}
}

protected void finalize() throws Throwable {
	close();
	super.finalize();
}

protected void setTimeToLive(byte ttl) throws IOException {
	setTTL(ttl);
}

protected byte getTimeToLive() throws IOException {
	return getTTL();
}

protected native void bind(int lport, InetAddress laddr) throws SocketException;
protected native void send(DatagramPacket p) throws IOException;
protected native int peek(InetAddress i) throws IOException;
protected native void receive(DatagramPacket p) throws IOException;
protected native void setTTL(byte ttl) throws IOException;
protected native byte getTTL() throws IOException;
protected native void join(InetAddress inetaddr) throws IOException;
protected native void leave(InetAddress inetaddr) throws IOException;
protected native void socketSetOption(int option, Object value);
protected native int socketGetOption(int option);
private native void datagramSocketCreate();
private native void datagramSocketClose();

}
