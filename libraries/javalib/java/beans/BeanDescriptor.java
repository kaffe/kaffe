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

public class BeanDescriptor extends FeatureDescriptor {

  private Class bclass;
  private Class cclass;

  public BeanDescriptor(Class beanClass)
  {
    this(beanClass, null);
  }

  public BeanDescriptor(Class beanClass, Class customizerClass)
  {
    bclass = beanClass;
    cclass = customizerClass;
    /* The default name is the class name without the package name.
     * It is also the default value for display name and short
     * description.
     */
    String name = beanClass.getName();
    name = name.substring(name.lastIndexOf('.') + 1);
    setName(name);
    setDisplayName(name);
    setShortDescription(name);
  }

  public Class getBeanClass()
  {
    return (bclass);
  }

  public Class getCustomizerClass()
  {
    return (cclass);
  }

}
