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

import java.io.FileDescriptor;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

abstract public class SocketImpl
	implements SocketOptions
{
	protected FileDescriptor fd;
	protected InetAddress address;
	protected int port;
	protected int localport;

abstract protected void accept(SocketImpl s) throws IOException;

abstract protected int available() throws IOException;

abstract protected void bind(InetAddress host, int port) throws IOException;

abstract protected void close() throws IOException;

abstract protected void connect(InetAddress address, int port) throws IOException;

abstract protected void connect(String host, int port) throws IOException;

abstract protected void create(boolean stream) throws IOException;

protected FileDescriptor getFileDescriptor() {
	return fd;
}

protected InetAddress getInetAddress() {
	try {
		return (InetAddress) getOption(SO_BINDADDR);
	} catch (SocketException e) {
		return address;
	}
}

abstract protected InputStream getInputStream() throws IOException;

protected int getLocalPort() {
	return localport;
}

abstract protected OutputStream getOutputStream() throws IOException;

protected int getPort() {
	return port;
}

abstract protected void listen(int backlog) throws IOException;

public String toString() {
	return "ServerSocket[addr="+address.toString()+",port="+String.valueOf(port)+",localport="+String.valueOf(localport)+"]";
}

}
