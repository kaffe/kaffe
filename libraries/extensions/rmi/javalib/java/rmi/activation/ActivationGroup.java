package java.rmi.activation;

import java.rmi.server.UnicastRemoteObject;
import java.rmi.RemoteException;
import java.rmi.Remote;
import java.rmi.MarshalledObject;

public abstract class ActivationGroup
	extends UnicastRemoteObject
	implements ActivationInstantiator {

protected ActivationGroup(ActivationGroupID groupID) throws RemoteException {
	throw new kaffe.util.NotImplemented();
}

public boolean inactiveObject(ActivationID id) throws ActivationException, UnknownObjectException, RemoteException {
	throw new kaffe.util.NotImplemented();
}

public abstract void activeObject(ActivationID id, Remote obj) throws ActivationException, UnknownObjectException, RemoteException;

public static ActivationGroup createGroup(ActivationGroupID id, ActivationGroupDesc desc, long incarnation) throws ActivationException {
	throw new kaffe.util.NotImplemented();
}

public static ActivationGroupID currentGroupID() {
	throw new kaffe.util.NotImplemented();
}

public static void setSystem(ActivationSystem system) throws ActivationException {
	throw new kaffe.util.NotImplemented();
}

public static ActivationSystem getSystem() throws ActivationException {
	throw new kaffe.util.NotImplemented();
}

protected void activeObject(ActivationID id, MarshalledObject mobj) throws ActivationException, UnknownObjectException, RemoteException {
	throw new kaffe.util.NotImplemented();
}

protected void inactiveGroup() throws UnknownGroupException, RemoteException {
	throw new kaffe.util.NotImplemented();
}

}
