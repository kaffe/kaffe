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

public interface BeanInfo {

  public static final int ICON_COLOR_16x16 = 1;
  public static final int ICON_COLOR_32x32 = 2;
  public static final int ICON_MONO_16x16 = 3;
  public static final int ICON_MONO_32x32 = 4;

  public abstract BeanDescriptor getBeanDescriptor();
  public abstract EventSetDescriptor[] getEventSetDescriptors();
  public abstract int getDefaultEventIndex();
  public abstract PropertyDescriptor[] getPropertyDescriptors();
  public abstract int getDefaultPropertyIndex();
  public abstract MethodDescriptor[] getMethodDescriptors();
  public abstract BeanInfo[] getAdditionalBeanInfo();
  public abstract Image getIcon(int iconKind);

}
