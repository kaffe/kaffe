/*
 * Copyright (c) 1996, 1997, 1998, 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license-lesser.terms" for information on usage and 
 * redistribution of this file.
 */

package java.rmi.activation;

import java.io.Serializable;
import java.rmi.Remote;
import java.rmi.RemoteException;

public class ActivationID
	implements Serializable {

private Activator activator;

public ActivationID(Activator activator) {
	this.activator = activator;
}

public Remote activate(boolean force) throws ActivationException, UnknownObjectException, RemoteException {
	throw new kaffe.util.NotImplemented();
}

public int hashCode() {
	return (activator.hashCode());
}

public boolean equals(Object obj) {
	if (obj instanceof ActivationID) {
		ActivationID that = (ActivationID)obj;
		if (this.activator.equals(that.activator)) {
			return (true);
		}
	}
	return (false);
}

}
