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
import java.util.Vector;

public class VetoableChangeSupport implements Serializable {

  private Vector listeners;
  private Object source;

  public VetoableChangeSupport(Object sourceBean)
  {
    listeners = new Vector();
    source = sourceBean;
  }

  public synchronized void addVetoableChangeListener(VetoableChangeListener listener)
  {
    listeners.addElement(listener);
  }

  public synchronized void removeVetoableChangeListener(VetoableChangeListener listener)
  {
    listeners.removeElement(listener);
  }

  public void fireVetoableChange(String propertyName, Object oldValue, Object newValue) throws PropertyVetoException
  {
    // Check for equality.
    if (oldValue != null && oldValue.equals(newValue)) {
      return;
    }

    // Clone the set of listeners to avoid synchrnization problems.
    Vector vec = (Vector)listeners.clone();

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
