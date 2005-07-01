/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2003, 2004
 *      Kaffe.org's developers. Some parts from GNU Classpath.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package gnu.java.net;

import java.io.FileDescriptor;
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocketImpl;
import java.net.InetAddress;
import java.net.NetworkInterface;
import java.net.SocketAddress;
import java.net.SocketException;
import java.net.UnknownHostException;

/* XXX: This class should not be public!
 * It's public right now cause it's accessed from kaffe.net.
 */
public class PlainDatagramSocketImpl
  extends DatagramSocketImpl {

private int timeout;
private int native_fd;
  
  /**
   * Lock object to serialize threads wanting to receive 
   */
  private final Object RECEIVE_LOCK = new Object();
  
  /**
   * Lock object to serialize threads wanting to send 
   */
  private final Object SEND_LOCK = new Object();


static {
	System.loadLibrary("net");
}

public PlainDatagramSocketImpl() {
	timeout = -1; // = NOTIMEOUT
	native_fd = -1;
}

public int getNativeFD() {
	return native_fd;
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
  synchronized(this)
    {
      if (native_fd != -1)
	close();
    }
  super.finalize();
}

protected void setTimeToLive(int ttl) throws IOException {
	setTTL((byte)ttl);
}

protected int getTimeToLive() throws IOException {
	return getTTL();
}

protected void sendUrgentData(int data) throws IOException {
	// TODO: replace that dummy function with a real one.
}

protected void send(DatagramPacket packet) throws IOException
{
  synchronized(SEND_LOCK)
    {
      send0(packet);
    }
}

protected void receive(DatagramPacket packet) throws IOException
{
  synchronized(RECEIVE_LOCK)
    {
      receive0(packet);
    }
}

protected native int peekData(DatagramPacket p) throws IOException;
protected native void bind(int lport, InetAddress laddr) throws SocketException;
protected native void send0(DatagramPacket p) throws IOException;
protected native int peek(InetAddress i) throws IOException;
protected native void receive0(DatagramPacket p) throws IOException;
protected native void setTTL(byte ttl) throws IOException;
protected native byte getTTL() throws IOException;
protected native void join(InetAddress inetaddr) throws IOException;
protected native void leave(InetAddress inetaddr) throws IOException;
protected native void joinGroup(SocketAddress inetaddr,
                                NetworkInterface netIf) throws IOException;
protected native void leaveGroup(SocketAddress inetaddr,
                                 NetworkInterface netIf) throws IOException;
protected native void socketSetOption(int option, Object value);
protected native int socketGetOption(int option);
private native void datagramSocketCreate();
private native void datagramSocketClose();
protected native void setBlocking(boolean block);

}
