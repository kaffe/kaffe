package java.net;

import java.io.IOException;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class MulticastSocket
  extends DatagramSocket
{
	private DatagramSocketImpl impl;

public MulticastSocket() throws IOException
{
	impl = null;
}

public MulticastSocket(int port) throws IOException
{
	impl = new PlainDatagramSocketImpl();
	impl.create();
	impl.bind(port, InetAddress.getByName("224.0.0.0"));
}

public InetAddress getInterface() throws SocketException
{
	// Not sure what to do with this ...
	return (null);
}

public byte getTTL() throws IOException
{
	return (impl.getTTL());
}

public void joinGroup(InetAddress mcastaddr) throws IOException
{
	impl.join(mcastaddr);
}

public void leaveGroup(InetAddress mcastaddr) throws IOException
{
	impl.leave(mcastaddr);
}

public synchronized void send(DatagramPacket p, byte ttl) throws IOException
{
	byte ottl = impl.getTTL();
	impl.setTTL(ttl);
	impl.send(p);
	impl.setTTL(ottl);
}

public void setInterface(InetAddress inf) throws SocketException
{
	// Not sure what to do with this ...
}

public void setTTL(byte ttl) throws IOException
{
	impl.setTTL(ttl);
}
}
