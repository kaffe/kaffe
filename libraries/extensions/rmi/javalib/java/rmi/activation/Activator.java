package java.rmi.activation;

import java.rmi.Remote;
import java.rmi.RemoteException;
import java.rmi.MarshalledObject;

public interface Activator
	extends Remote {

public MarshalledObject activate(ActivationID id, boolean force) throws ActivationException, UnknownObjectException, RemoteException;

}
