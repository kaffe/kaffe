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

import java.lang.IllegalArgumentException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import kaffe.net.DefaultSocketImplFactory;

public class Socket {

private static SocketImplFactory factory = new DefaultSocketImplFactory();

SocketImpl impl;
boolean bound;

public Socket(InetAddress address, int port) throws IOException {
    this(address, port, true);
}

public Socket(String host, int port) throws UnknownHostException, IOException {
    this(InetAddress.getByName(host), port, true);
}

public Socket(InetAddress address, int port, InetAddress localAddr, int localPort) throws IOException {
    if (localAddr == null)
        throw new NullPointerException();
    connect(address, port, true, localAddr, localPort, -1);
}
    
public Socket(String host, int port, InetAddress localAddr, int localPort) throws IOException {
    this(InetAddress.getByName(host), port, localAddr, localPort);
}

/**
 * @deprecated.
 */
public Socket(String host, int port, boolean stream) throws IOException {
    this(InetAddress.getByName(host), port, stream);
}

/**
 * @deprecated.
 */
public Socket(InetAddress address, int port, boolean stream) throws IOException {
    connect(address, port, stream, null, 0, -1);
}

/**
 * @since 1.4
 */
public void connect(SocketAddress address) throws IOException {
    if (isClosed())
	throw new SocketException("Socket is closed");
    if (address == null)
	throw new IllegalArgumentException();

    if (address instanceof InetSocketAddress) {
	InetSocketAddress inetaddress = (InetSocketAddress)address;

	connect(inetaddress.getAddress(), inetaddress.getPort(), true,
		null, 0, -1);
    } else
	throw new IllegalArgumentException();
}

/**
 * @since 1.4
 */
public void connect(SocketAddress address, int timeout) throws IOException {
    if (isClosed())
	throw new SocketException("Socket is closed");
    if (address == null)
	throw new IllegalArgumentException();

    if (address instanceof InetSocketAddress) {
	InetSocketAddress inetaddress = (InetSocketAddress)address;

	connect(inetaddress.getAddress(), inetaddress.getPort(), true,
		null, 0, timeout);
   } else
	throw new IllegalArgumentException();
}

public void bind(SocketAddress address) throws IOException
{
    if (isClosed())
	throw new SocketException("Socket is closed");
    if (!(address instanceof InetSocketAddress))
        throw new IllegalArgumentException();
    if (isBound())
	throw new SocketException("Already bound");

    InetSocketAddress iaddr = (InetSocketAddress)address;

    impl = factory.createSocketImpl();
    try {
        impl.create(true);
	if (address != null)
	    impl.bind(iaddr.getAddress(), iaddr.getPort());
    } catch (IOException ioe) {
	try {
	    impl.close();
	} catch (IOException _) {
	}
	throw ioe;
    }
    bound = true;
}

private void connect(InetAddress address, int port, boolean stream,
                     InetAddress localAddr, int localPort, int timeout) throws IOException {
    impl = factory.createSocketImpl();
    try {
        impl.create(stream);
        if (localAddr != null)
            impl.bind(localAddr, localPort);
        impl.connect(address, port);
    } catch (IOException ioe) {
        try {
            impl.close();
        }
        catch (IOException _) {
        }
        throw ioe;
    }
    bound = true;
}

protected Socket(SocketImpl simpl) throws SocketException {
	impl = simpl;
	bound = true;
}

protected Socket() {
	impl = factory.createSocketImpl();
}

public synchronized void close() throws IOException {
	impl.close();
	bound = false;
}

public boolean isBound() {
	return bound;
}

public boolean isClosed() {
	return impl.fd.valid();
}

public InetAddress getInetAddress() {
	return impl.getInetAddress();
}

public InputStream getInputStream() throws IOException {
	return impl.getInputStream();
}

public int getLocalPort() {
	return impl.getLocalPort();
}

public OutputStream getOutputStream() throws IOException {
	return impl.getOutputStream();
}

public int getPort() {
	return impl.getPort();
}

public InetAddress getLocalAddress() {
	try {
            return (InetAddress)impl.getOption(SocketOptions.SO_BINDADDR); 
	} catch (SocketException e) { }
	try {
		return InetAddress.getByName("0.0.0.0");
	} catch (UnknownHostException e2) { }
	return null;
}

public SocketAddress getLocalSocketAddress() {
	InetAddress addr;

	try {
		addr = (InetAddress) impl.getOption(SocketOptions.SO_BINDADDR);
	} catch (SocketException e) {
		return null;
	}
	
	return new InetSocketAddress(addr, getPort());
}

public static synchronized void setSocketImplFactory(SocketImplFactory fac) throws IOException {
	SecurityManager sm = System.getSecurityManager();
	if( sm != null )
		sm.checkSetFactory();
	factory = fac;
}

public void setSoLinger(boolean on, int timeout) throws SocketException {
	if (on) {
		impl.setOption(SocketOptions.SO_LINGER, new Integer(timeout));
	}
	else {
		impl.setOption(SocketOptions.SO_LINGER, new Boolean(on));
	}
}

public int getSoLinger() throws SocketException {
	return ((Integer) impl.getOption(SocketOptions.SO_LINGER)).intValue();
}

public void setSoTimeout(int timeout) throws SocketException {
	impl.setOption(SocketOptions.SO_TIMEOUT, new Integer(timeout));
}

public int getSoTimeout() throws SocketException {
	return ((Integer) impl.getOption(SocketOptions.SO_TIMEOUT)).intValue();
}

public void setSendBufferSize(int size) throws SocketException {
	impl.setOption(SocketOptions.SO_SNDBUF, new Integer(size));
}

public int getSendBufferSize() throws SocketException {
	return ((Integer) impl.getOption(SocketOptions.SO_SNDBUF)).intValue();
}

public void setReceiveBufferSize(int size) throws SocketException {
	impl.setOption(SocketOptions.SO_RCVBUF, new Integer(size));
}

public int getReceiveBufferSize() throws SocketException {
	return ((Integer) impl.getOption(SocketOptions.SO_RCVBUF)).intValue();
}

public void setTcpNoDelay(boolean on) throws SocketException {
	impl.setOption(SocketOptions.TCP_NODELAY, new Boolean(on));
}

public boolean getTcpNoDelay() throws SocketException {
	return ((Boolean) impl.getOption(SocketOptions.TCP_NODELAY)).booleanValue();
}

public String toString() {
	return impl.toString();
}
}
