package java.rmi.activation;

import java.io.Serializable;

public class ActivationGroupID
	implements Serializable {

private ActivationSystem system;

public ActivationGroupID(ActivationSystem system) {
	this.system = system;
}

public ActivationSystem getSystem() {
	return (system);
}

public int hashCode() {
	return (system.hashCode());
}

public boolean equals(Object obj) {
	if (obj instanceof ActivationGroupID) {
		ActivationGroupID that = (ActivationGroupID)obj;
		if (this.system.equals(that.system)) {
			return (true);
		}
	}
	return (false);
}

}
