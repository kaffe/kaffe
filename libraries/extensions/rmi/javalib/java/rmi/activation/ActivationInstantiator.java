package java.rmi.activation;

import java.rmi.Remote;
import java.rmi.RemoteException;
import java.rmi.MarshalledObject;

public interface ActivationInstantiator
	extends Remote {

public MarshalledObject newInstance(ActivationID id, ActivationDesc desc) throws ActivationException, RemoteException;

}
