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

import java.util.EventObject;

public class PropertyChangeEvent extends EventObject {

  // NB: these are all part of the serial form
  private String propertyName;
  private Object oldValue;
  private Object newValue;
  private Object propagationId;

  public PropertyChangeEvent(Object source, String propertyName, Object oldValue, Object newValue)
  {
    super(source);
    this.propertyName = propertyName;
    this.oldValue = oldValue;
    this.newValue = newValue;
    propagationId = null;
  }

  public String getPropertyName()
  {
    return (propertyName);
  }

  public Object getNewValue()
  {
    return (newValue);
  }

  public Object getOldValue()
  {
    return (oldValue);
  }

  public void setPropagationId(Object propagationId)
  {
    this.propagationId = propagationId;
  }

  public Object getPropagationId()
  {
    return (propagationId);
  }

}
