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

public class PropertyVetoException extends Exception {

  private PropertyChangeEvent evt;

  public PropertyVetoException(String mess, PropertyChangeEvent evt)
  {
    super(mess);
    evt = evt;
  }

  public PropertyChangeEvent getPropertyChangeEvent()
  {
    return (evt);
  }

}
