package java.rmi.activation;

import java.rmi.RemoteException;

public class ActivateFailedException
	extends RemoteException {

public ActivateFailedException(String s) {
	super(s);
}

public ActivateFailedException(String s, Exception ex) {
	super(s, ex);
}

}
