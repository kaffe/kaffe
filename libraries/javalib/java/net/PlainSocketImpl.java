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
import java.io.InputStream;
import java.io.OutputStream;

/* XXX: This class should not be public!
 * It's public right now cause it's accessed from kaffe.net.
 */
public class PlainSocketImpl
  extends SocketImpl {

private InputStream in;
private OutputStream out;
protected int timeout;
private boolean closed;
private boolean blocking;
private boolean connecting;

static {
	System.loadLibrary("net");
}

public PlainSocketImpl() {
        timeout = -1; // As defined in jsyscall.h
	blocking = true;
}

protected void accept(SocketImpl s) throws IOException {
	socketAccept(s);
}

protected synchronized int available() throws IOException {
	return closed ? 0 : socketAvailable();
}

protected void bind(InetAddress address, int lport) throws IOException {
	socketBind(address, lport);
}

protected synchronized void close() throws IOException {
	if( !closed )
		socketClose();
	closed = true;
	in = null;
	out = null;
}

protected void connect(String host, int port) throws IOException {
	connect(InetAddress.getByName(host), port);
}

protected void connect(InetAddress address, int port) throws IOException {
	socketConnect(address, port, this.timeout);
}

protected void connect(SocketAddress address, int timeout) throws IOException {
        InetSocketAddress iaddr = (InetSocketAddress)address;
	
	socketConnect(iaddr.getAddress(), iaddr.getPort(), timeout);
}

protected void create(boolean stream) throws IOException {
	socketCreate(stream);
}

protected void finalize() throws Throwable {
	close();
	super.finalize();
}

protected synchronized InputStream getInputStream() throws IOException {
	if (in == null) {
		in = new SocketInputStream(this); 
	}
	return (in);
}

protected synchronized OutputStream getOutputStream() throws IOException {
	if (out == null) {
		out = new SocketOutputStream(this);
	}
	return (out);
}

protected void listen(int count) throws IOException {
	socketListen(count);
}

protected void setTcpNoDelay(boolean on) throws SocketException {
	socketSetOption(TCP_NODELAY, new Integer(on ? 1 : 0));
}

protected boolean getTcpNoDelay() throws SocketException {
	return socketGetOption(TCP_NODELAY) != 0;
}

protected void setSoLinger(boolean on, int howlong) throws SocketException {
	socketSetOption(SO_LINGER, new Integer(on ? howlong : 0));
}

protected int getSoLinger() throws SocketException {
	return socketGetOption(SO_LINGER);
}

protected void setSoTimeout(int timeout) throws SocketException {
	setOption(SocketOptions.SO_TIMEOUT, new Integer(timeout));
}

protected int getSoTimeout() throws SocketException {
	return ((Integer)getOption(SocketOptions.SO_TIMEOUT)).intValue();
}

protected void setSendBufferSize(int size) throws SocketException {
	socketSetOption(SO_SNDBUF, new Integer(size));
}

protected int getSendBufferSize() throws SocketException {
	return socketGetOption(SO_SNDBUF);
}

protected void setReceiveBufferSize(int size) throws SocketException {
	socketSetOption(SO_RCVBUF, new Integer(size));
}

protected int getReceiveBufferSize() throws SocketException {
	return socketGetOption(SO_RCVBUF);
}

public void setOption(int option, Object data) throws SocketException {
	final boolean disable = (data instanceof Boolean) && !((Boolean) data).booleanValue();
	int timo;

	if (disable) {
		data = new Integer(0);			// the common case
	}
	switch (option) {
	case SO_SNDBUF:
	case SO_RCVBUF:
	case SO_LINGER:
	case SO_REUSEADDR:
		break;
	case TCP_NODELAY:
		data = new Integer(disable ? 0 : 1);
		break;
	case SO_TIMEOUT:
		timo = ((Integer)data).intValue();
		if (timo < 0) {
			throw new IllegalArgumentException("timeout < 0");
		}
		// This is for our infinite timeout, 0 is reserved for
		// some non-blocking operation.
		if (timo == 0)
			this.timeout = -1;
		else
			this.timeout = timo;
		return;

	case SO_BINDADDR:
		throw new SocketException("Read-only socket option");
	case IP_MULTICAST_IF:
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
	case TCP_NODELAY:
		return new Boolean(socketGetOption(option) != 0);
	case SO_TIMEOUT:
		if (timeout == -1)
			return new Integer(0);
		else
			return new Integer(timeout);
	case SO_BINDADDR:
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
	case IP_MULTICAST_IF:
	default:
		throw new SocketException("Unknown socket option");
	}
}

protected int read(byte[] buf, int offset, int len) throws IOException {
	if (closed)
		return -1;
	int r = socketRead(buf, offset, len);
	if (r > 0 || len == 0) {
		return (r);
	}
	else {
		return (-1);
	}
}

protected void write(byte[] buf, int offset, int len) throws IOException {
	if (closed)
		throw new IOException("socket closed");
	socketWrite(buf, offset, len);
}

public native void socketSetOption(int option, Object data) throws SocketException;
public native int socketGetOption(int option) throws SocketException;
protected native void socketAccept(SocketImpl sock);
protected native int  socketAvailable();
protected native void socketBind(InetAddress addr, int port);
protected native void socketClose();
protected native void socketConnect(InetAddress addr, int port, int timeout);
protected native void socketCreate(boolean stream);
protected native void socketListen(int count);
protected native int socketRead(byte[] buf, int offset, int len) throws IOException;
protected native void socketWrite(byte[] buf, int offset, int len) throws IOException;

// This function are principally for the NIO implementation of sockets.
protected native void waitForConnection() throws IOException;
protected native void setBlocking(boolean blocking);

}
