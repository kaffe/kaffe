package java.net;

import java.io.FileDescriptor;
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
abstract public class DatagramSocketImpl
  implements SocketOptions
{
	protected int localPort;
	protected FileDescriptor fd;

public DatagramSocketImpl()
	{
	localPort = 0;
	fd = null;
}

abstract protected void bind(int lport, InetAddress laddr) throws SocketException;

abstract protected void close();

abstract protected void create() throws SocketException;

protected FileDescriptor getFileDescriptor()
	{
	return (fd);
}

protected int getLocalPort()
	{
	return (localPort);
}

abstract protected byte getTTL() throws IOException;

abstract protected void join(InetAddress inetaddr) throws IOException;

abstract protected void leave(InetAddress inetaddr) throws IOException;

abstract protected int peek(InetAddress i) throws IOException;

abstract protected void receive(DatagramPacket p) throws IOException;

abstract protected void send(DatagramPacket p) throws IOException;

abstract protected void setTTL(byte ttl) throws IOException;
}
