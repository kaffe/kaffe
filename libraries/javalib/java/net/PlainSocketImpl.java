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

import java.lang.String;
import java.io.FileDescriptor;
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
private int timeout;

static {
	System.loadLibrary("net");
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
	closed = true;
	in = null;
	out = null;
	socketClose();
}

protected void connect(String host, int port) throws IOException {
	connect(InetAddress.getByName(host), port);
}

protected void connect(InetAddress address, int port) throws IOException {
	socketConnect(address, port);
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
	int r = socketRead(buf, offset, len);
	if (r > 0 || len == 0) {
		return (r);
	}
	else {
		return (-1);
	}
}

protected void write(byte[] buf, int offset, int len) throws IOException {
	socketWrite(buf, offset, len);
}

public native void socketSetOption(int option, Object data) throws SocketException;
public native int socketGetOption(int option) throws SocketException;
native private void socketAccept(SocketImpl sock);
native private int  socketAvailable();
native private void socketBind(InetAddress addr, int port);
native private void socketClose();
native private void socketConnect(InetAddress addr, int port);
native private void socketCreate(boolean stream);
native private void socketListen(int count);
native private int socketRead(byte[] buf, int offset, int len) throws IOException;
native private void socketWrite(byte[] buf, int offset, int len) throws IOException;

}
