/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package kaffe.rmi.server;

import java.rmi.server.RMISocketFactory;
import java.rmi.server.RMIServerSocketFactory;
import java.rmi.server.RMIClientSocketFactory;
import java.rmi.RemoteException;
import kaffe.rmi.server.UnicastConnection;
import java.util.Hashtable;
import java.net.Socket;
import java.net.ServerSocket;
import java.io.IOException;
import java.io.ObjectOutput;
import java.io.ObjectInput;
import java.net.InetAddress;
import java.net.UnknownHostException;

public class UnicastConnectionManager
	implements Runnable, ProtocolConstants {

private static String localhost;
private static Hashtable servers = new Hashtable();

private Thread serverThread;
private ServerSocket ssock;
String serverName;
int serverPort;
private RMIServerSocketFactory serverFactory;
private RMIClientSocketFactory clientFactory;

static {
        try {
                localhost = InetAddress.getLocalHost().getHostName();
        }
        catch (UnknownHostException _) {
                localhost = "localhost";
        }
}

private UnicastConnectionManager(String host, int port, RMIClientSocketFactory csf) {
	ssock = null;
	serverName = host;
	serverPort = port;
	serverFactory = null;
	clientFactory = csf;
}

private UnicastConnectionManager(int port, RMIServerSocketFactory ssf) {
	try {
		ssock = ssf.createServerSocket(port);
		serverPort = ssock.getLocalPort();
	}
	catch (IOException _) {
		try {
			ssock = ssf.createServerSocket(0);
			serverPort = ssock.getLocalPort();
		}
		catch (IOException __) {
			ssock = null;
			serverPort = 0;
		}
	}
	serverName = localhost;
	serverFactory = ssf;
	clientFactory = null;
}

/**
 * Return a client connection manager which will connect to the given
 * host/port.
 */
public static synchronized UnicastConnectionManager getInstance(String host, int port, RMIClientSocketFactory csf) {
//System.out.println("getInstance: " + host + "," + port + "," + csf);
	if (csf == null) {
		csf = RMISocketFactory.getSocketFactory();
	}
	TripleKey key = new TripleKey(host, port, csf);
	UnicastConnectionManager man = (UnicastConnectionManager)servers.get(key);
	if (man == null) {
		man = new UnicastConnectionManager(host, port, csf);
		servers.put(key, man);
	}
	return (man);
}

/**
 * Return a server connection manager which will accept connection on the
 * given port.
 */
public static synchronized UnicastConnectionManager getInstance(int port, RMIServerSocketFactory ssf) {
//System.out.println("getInstance: " + port + "," + ssf);
	if (ssf == null) {
		ssf = RMISocketFactory.getSocketFactory();
	}
	TripleKey key = new TripleKey(localhost, port, ssf);
	UnicastConnectionManager man = (UnicastConnectionManager)servers.get(key);
	if (man == null) {
		man = new UnicastConnectionManager(port, ssf);
		// The provided port might not be the set port.
		key.port = man.serverPort;
		servers.put(key, man);
	}
	return (man);
}

/**
 * Get a connection from this manager.
 */
public UnicastConnection getConnection() throws IOException {
	if (ssock == null) {
		return (getClientConnection());
	}
	else {
		return (getServerConnection());
	}
}

/**
 * Accept a connection to this server.
 */
private UnicastConnection getServerConnection() throws IOException {
	Socket sock = ssock.accept();
	UnicastConnection conn = new UnicastConnection(this, sock);
	conn.acceptConnection();
//System.out.println("Server connection " + conn);
	return (conn);
}

/**
 * Make a conection from this client to the server.
 */
private UnicastConnection getClientConnection() throws IOException {
	Socket sock = clientFactory.createSocket(serverName, serverPort);
	UnicastConnection conn = new UnicastConnection(this, sock);
	conn.makeConnection(DEFAULT_PROTOCOL);
//System.out.println("Client connection " + conn);
	return (conn);
}

/**
 * Discard a connection when we're done with it - maybe it can be
 * recycled.
 */
public void discardConnection(UnicastConnection conn) {
//System.out.println("Discarding connection " + conn);
	conn.disconnect();
}

/**
 * Start a server on this manager if it's a server socket and we've not
 * already got one running.
 */
public void startServer() {
	synchronized(this) {
		if (ssock == null || serverThread != null) {
			return;
		}
		serverThread = new Thread(this);
	}
	serverThread.start();
}

/**
 * Server thread for connection manager.
 */
public void run() {
	for (;;) {
		try {
//System.out.println("Waiting for connection on " + serverPort);
			UnicastConnection conn = getServerConnection();
			(new Thread(conn)).start();
		}
		catch (Exception e) {
			e.printStackTrace();
		}
	}
}

/**
 * Serialization routine.
 */
void write(ObjectOutput out) throws IOException {
        out.writeUTF(serverName);
        out.writeInt(serverPort);
}

/**
 * Serialization routine.
 */
static UnicastConnectionManager read(ObjectInput in) throws IOException {
        String host = in.readUTF();
        int port = in.readInt();
	RMIClientSocketFactory csf = ((RMIObjectInputStream)in).manager.clientFactory;
        return (getInstance(host, port, csf));
}

}

/**
 * This is use as the hashkey for the client/server connections.
 */
class TripleKey {

String host;
int port;
Object other;

TripleKey(String host, int port, Object other) {
	this.host = host;
	this.port = port;
	this.other = other;
}

/**
 * Hash code just include the host and other - we ignore the port since
 * this has unusual matching behaviour.
 */
public int hashCode() {
	return (host.hashCode() ^ other.hashCode());
}

public boolean equals(Object obj) {
	if (obj instanceof TripleKey) {
		TripleKey other = (TripleKey)obj;
		if (this.host.equals(other.host) &&
		    this.other == other.other &&
		    (this.port == other.port || this.port == 0 || other.port == 0)) {
			return (true);
		}
	}
	return (false);
}

}
