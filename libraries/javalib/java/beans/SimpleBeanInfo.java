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
import java.awt.Toolkit;
import java.net.URL;

public class SimpleBeanInfo implements BeanInfo {

  public BeanDescriptor getBeanDescriptor()
  {
    return (null);
  }

  public EventSetDescriptor[] getEventSetDescriptors()
  {
    return (null);
  }

  public int getDefaultEventIndex()
  {
    return (-1);
  }

  public PropertyDescriptor[] getPropertyDescriptors()
  {
    return (null);
  }

  public int getDefaultPropertyIndex()
  {
    return (-1);
  }

  public MethodDescriptor[] getMethodDescriptors()
  {
    return (null);
  }

  public BeanInfo[] getAdditionalBeanInfo()
  {
    return (null);
  }

  public Image getIcon(int iconKind)
  {
    return (null);
  }

  public Image loadImage(String resourceName)
  {
    URL nm = this.getClass().getResource(resourceName);
    if (nm == null) {
      return (null);
    }
    Image img = Toolkit.getDefaultToolkit().getImage(nm);
    return (img);
  }

}
