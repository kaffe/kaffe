package java.rmi.activation;

import java.rmi.server.RemoteServer;
import java.rmi.server.RMIClientSocketFactory;
import java.rmi.server.RMIServerSocketFactory;
import java.rmi.RemoteException;
import java.rmi.Remote;
import java.rmi.NoSuchObjectException;
import java.rmi.MarshalledObject;

public abstract class Activatable
	extends RemoteServer {

protected Activatable(String location, MarshalledObject data, boolean restart, int port) throws ActivationException, RemoteException {
	throw new kaffe.util.NotImplemented();
}

protected Activatable(String location, MarshalledObject data, boolean restart, int port, RMIClientSocketFactory csf, RMIServerSocketFactory ssf) throws ActivationException, RemoteException {
	throw new kaffe.util.NotImplemented();
}

protected Activatable(ActivationID id, int port) throws RemoteException {
	throw new kaffe.util.NotImplemented();
}

protected Activatable(ActivationID id, int port, RMIClientSocketFactory csf, RMIServerSocketFactory ssf) throws RemoteException {
	throw new kaffe.util.NotImplemented();
}

protected ActivationID getID() {
	throw new kaffe.util.NotImplemented();
}

public static Remote register(ActivationDesc desc) throws UnknownGroupException, ActivationException, RemoteException {
	throw new kaffe.util.NotImplemented();
}

public static boolean inactive(ActivationID id) throws UnknownObjectException, ActivationException, RemoteException {
	throw new kaffe.util.NotImplemented();
}

public static void unregister(ActivationID id) throws UnknownObjectException, ActivationException, RemoteException {
	throw new kaffe.util.NotImplemented();
}

public static ActivationID exportObject(Remote obj, String location, MarshalledObject data, boolean restart, int port) throws ActivationException, RemoteException {
	throw new kaffe.util.NotImplemented();
}

public static ActivationID exportObject(Remote obj, String location, MarshalledObject data, boolean restart, int port, RMIClientSocketFactory csf, RMIServerSocketFactory ssf) throws ActivationException, RemoteException {
	throw new kaffe.util.NotImplemented();
}

public static Remote exportObject(Remote obj, ActivationID id, int port) throws RemoteException {
	throw new kaffe.util.NotImplemented();
}

public static Remote exportObject(Remote obj, ActivationID id, int port, RMIClientSocketFactory csf, RMIServerSocketFactory ssf) throws RemoteException {
	throw new kaffe.util.NotImplemented();
}

public static boolean unexportObject(Remote obj, boolean force) throws NoSuchObjectException {
	throw new kaffe.util.NotImplemented();
}

}
