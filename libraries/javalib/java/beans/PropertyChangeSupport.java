package java.beans;

import java.io.Serializable;
import java.util.Vector;

/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class PropertyChangeSupport
  implements Serializable
{
	private static final long serialVersionUID = 6401253773779951803L;
	private Vector listeners;
	private Object source;

public PropertyChangeSupport(Object sourceBean) {
	listeners = new Vector();
	source = sourceBean;
}

public synchronized void addPropertyChangeListener(PropertyChangeListener listener) {
	listeners.addElement(listener);
}

public void firePropertyChange(String propertyName, Object oldValue, Object newValue) {
	// Check for equality.
	if (oldValue != null && oldValue.equals(newValue)) {
		return;
	}

	// Is anyone interested ?
	Vector vec;
	int size;
	synchronized (this) {
		size = listeners.size();
		if (size == 0) {
			return;
		}
		// Clone the set of listeners to avoid synchronization problems.
		vec = (Vector)listeners.clone();
	}
	PropertyChangeEvent evt = new PropertyChangeEvent(source, propertyName, oldValue, newValue);
	for (int dis = 0; dis < size; dis++) {
		((PropertyChangeListener)vec.elementAt(dis)).propertyChange(evt);
	}
}

public synchronized void removePropertyChangeListener(PropertyChangeListener listener) {
	listeners.removeElement(listener);
}
}
