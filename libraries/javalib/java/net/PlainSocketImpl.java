package java.net;

import java.io.FileDescriptor;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
class PlainSocketImpl
  extends SocketImpl
{
static {
	System.loadLibrary("net");
}

protected synchronized void accept(SocketImpl s) throws IOException {
	socketAccept(s);
}

protected synchronized int available() throws IOException {
	return socketAvailable();
}

protected synchronized void bind(InetAddress address, int lport) throws IOException {
	this.address=address;
	this.localport=lport;
	socketBind(address, lport);
}

protected synchronized void close() throws IOException {
	socketClose();
}

protected void connect(InetAddress address, int port) throws IOException {
	this.address=address;
	this.port=port;
	socketConnect(address, port);
}

protected void connect(String host, int port) throws IOException {
	connect(InetAddress.getByName(host), port);
}

protected synchronized void create(boolean stream) throws IOException {
	fd=new FileDescriptor();
	address=new InetAddress();
	socketCreate(stream);
}

protected synchronized void finalize() throws IOException {
	close();
}

protected synchronized InputStream getInputStream() throws IOException {
	return new SocketInputStream(this); 
}

protected synchronized OutputStream getOutputStream() throws IOException {
	return new SocketOutputStream(this);
}

protected synchronized void listen(int count) throws IOException {
	socketListen(count);
}

native private void socketAccept(SocketImpl sock);

native private int  socketAvailable();

native private void socketBind(InetAddress addr, int port);

native private void socketClose();

native private void socketConnect(InetAddress addr, int port);

/* Inherits:-
  private FileDescriptor fd;
  private InetAddress address;
  private int localport;
  private int port */
native private void socketCreate(boolean stream);

native private void socketListen(int count);
}
