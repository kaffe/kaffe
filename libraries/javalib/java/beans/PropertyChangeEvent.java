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

  private String pname;
  private Object ovalue;
  private Object nvalue;
  private Object pid;

  public PropertyChangeEvent(Object source, String propertyName, Object oldValue, Object newValue)
  {
    super(source);
    pname = propertyName;
    ovalue = oldValue;
    nvalue = newValue;
    pid = null;
  }

  public String getPropertyName()
  {
    return (pname);
  }

  public Object getNewValue()
  {
    return (nvalue);
  }

  public Object getOldValue()
  {
    return (ovalue);
  }

  public void setPropagationId(Object propagationId)
  {
    pid = propagationId;
  }

  public Object getPropagationId()
  {
    return (pid);
  }

}
