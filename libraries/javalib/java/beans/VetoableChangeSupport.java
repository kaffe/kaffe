/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.beans;

import java.io.Serializable;
import java.util.HashMap;
import java.util.Vector;

public class VetoableChangeSupport implements Serializable {

  private static final long serialVersionUID = -5090210921595982017L;
  private final HashMap PROPERTY_LISTENERS = new HashMap();
  private Object source;

  public VetoableChangeSupport(Object sourceBean)
  {
    source = sourceBean;
  }

  public synchronized void addVetoableChangeListener(String property, VetoableChangeListener listener)
  {
      if (!PROPERTY_LISTENERS.containsKey(property)) {
	  PROPERTY_LISTENERS.put(property, new Vector());
      }

      ((Vector) PROPERTY_LISTENERS.get(property)).addElement(listener);
  }

  public synchronized void addVetoableChangeListener(VetoableChangeListener listener)
  {
      addVetoableChangeListener(null, listener);
  }

  public synchronized void removeVetoableChangeListener(VetoableChangeListener listener)
  {
      removeVetoableChangeListener(null, listener);
  }

  public synchronized void removeVetoableChangeListener(String property, VetoableChangeListener listener)
  {
      if (PROPERTY_LISTENERS.containsKey(property)) {
	  ((Vector) PROPERTY_LISTENERS.get(property)).removeElement(listener);
      }
  }

  public void fireVetoableChange(String propertyName, Object oldValue, Object newValue) throws PropertyVetoException
  {
    // Check for equality.
    if (oldValue != null && oldValue.equals(newValue)) {
      return;
    }

    if (!PROPERTY_LISTENERS.containsKey(null)
	&& !PROPERTY_LISTENERS.containsKey(propertyName)) { 
	    return;
    }

    // Clone the set of listeners to avoid synchronization problems.
    Vector vec = (Vector) ((Vector) PROPERTY_LISTENERS.get(propertyName)).clone();
    vec.addAll((Vector) PROPERTY_LISTENERS.get(null));

    // Dispatch.  We catch the Veto exception so we can undo our changes
    int dispatch = 0;
    try {
      PropertyChangeEvent evt = new PropertyChangeEvent(source, propertyName, oldValue, newValue);
      for (; dispatch < vec.size(); dispatch++) {
        ((VetoableChangeListener)vec.elementAt(dispatch)).vetoableChange(evt);
      }
    }
    catch (PropertyVetoException _) {
      PropertyChangeEvent evt = new PropertyChangeEvent(source, propertyName, newValue, oldValue);
      for (int i = 0; i < dispatch; i++) {
        try {
          ((VetoableChangeListener)vec.elementAt(i)).vetoableChange(evt);
	}
        catch (PropertyVetoException ignore) {
	  // Just ignore these.
        }
      }
    }
  }

}
