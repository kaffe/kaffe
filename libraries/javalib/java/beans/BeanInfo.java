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

  int ICON_COLOR_16x16 = 1;
  int ICON_COLOR_32x32 = 2;
  int ICON_MONO_16x16 = 3;
  int ICON_MONO_32x32 = 4;

  BeanDescriptor getBeanDescriptor();
  EventSetDescriptor[] getEventSetDescriptors();
  int getDefaultEventIndex();
  PropertyDescriptor[] getPropertyDescriptors();
  int getDefaultPropertyIndex();
  MethodDescriptor[] getMethodDescriptors();
  BeanInfo[] getAdditionalBeanInfo();
  Image getIcon(int iconKind);

}
