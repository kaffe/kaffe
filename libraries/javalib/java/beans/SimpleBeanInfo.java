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

import java.awt.Image;

public class SimpleBeanInfo implements BeanInfo {

  protected BeanDescriptor bean = null;
  protected PropertyDescriptor[] properties = null;
  protected MethodDescriptor[] methods = null;
  protected EventSetDescriptor[] events = null;

  public BeanDescriptor getBeanDescriptor()
  {
    return (bean);
  }

  public EventSetDescriptor[] getEventSetDescriptors()
  {
    return (events);
  }

  public int getDefaultEventIndex()
  {
    return (-1);
  }

  public PropertyDescriptor[] getPropertyDescriptors()
  {
    return (properties);
  }

  public int getDefaultPropertyIndex()
  {
    return (-1);
  }

  public MethodDescriptor[] getMethodDescriptors()
  {
    return (methods);
  }

  public BeanInfo[] getAdditionalBeanInfo()
  {
    return (null);
  }

  public Image getIcon(int iconKind)
  {
    return (null);
  }

}
