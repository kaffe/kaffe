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

public class GenericBeanInfo extends SimpleBeanInfo {

  public GenericBeanInfo(BeanDescriptor bean, PropertyDescriptor[] properties, MethodDescriptor[] methods, EventSetDescriptor[] events)
  {
    this.bean = bean;
    this.properties = properties;
    this.methods = methods;
    this.events = events;
  }

}
