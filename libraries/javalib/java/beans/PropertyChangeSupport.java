package java.beans;

import java.io.Serializable;
import java.util.HashMap;
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
	private final HashMap PROPERTY_LISTENERS = new HashMap();
	private Object source;

public PropertyChangeSupport(Object sourceBean) {
	source = sourceBean;
}

public synchronized void addPropertyChangeListener(String property, PropertyChangeListener listener) {
	if (!PROPERTY_LISTENERS.containsKey(property)) {
			PROPERTY_LISTENERS.put(property, new Vector());
	}
	
	((Vector) PROPERTY_LISTENERS.get(property)).addElement(listener);
}

public synchronized void addPropertyChangeListener(PropertyChangeListener listener) {
	addPropertyChangeListener(null, listener);
}

public void firePropertyChange(String propertyName, Object oldValue, Object newValue) {
	// Check for equality.
	if (oldValue != null && oldValue.equals(newValue)) {
		return;
	}

	// Is anyone interested ?
	Vector vec = (Vector) PROPERTY_LISTENERS.get (propertyName);
        if (vec == null) {
        	vec = (Vector) PROPERTY_LISTENERS.get (null);
        } else {
		vec = ((Vector)vec.clone());
                vec.addAll((Vector) PROPERTY_LISTENERS.get(null));
	}

        if (vec==null) {
		return;
	}

	int size;
	synchronized (this) {
		size = vec.size();
		if (size == 0) {
			return;
		}
	}
	PropertyChangeEvent evt = new PropertyChangeEvent(source, propertyName, oldValue, newValue);
	for (int dis = 0; dis < size; dis++) {
		((PropertyChangeListener)vec.elementAt(dis)).propertyChange(evt);
	}
}

public synchronized void removePropertyChangeListener(PropertyChangeListener listener) {
	removePropertyChangeListener(null, listener);
}

public synchronized void removePropertyChangeListener(String property, PropertyChangeListener listener) {
	if (PROPERTY_LISTENERS.containsKey(property)) {
		((Vector) PROPERTY_LISTENERS.get(property)).removeElement(listener);
	}
}
}
