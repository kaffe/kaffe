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

import java.util.Hashtable;
import java.util.Enumeration;

public class FeatureDescriptor {

  private String name;
  private String dname;
  private boolean expert;
  private boolean hidden;
  private String sdesc;
  private Hashtable table;

  public FeatureDescriptor()
  {
    name = null;
    dname = null;
    expert = false;
    hidden = false;
    sdesc = null;
    table = new Hashtable();
  }

  FeatureDescriptor(String name)
  {
	this();
	this.name = name;
  }

  public String getName()
  {
    return (name);
  }

  public void setName(String name)
  {
    this.name = name;
  }

  public String getDisplayName()
  {
    return (dname);
  }

  public void setDisplayName(String displayName)
  {
    this.dname = displayName;
  }

  public boolean isExpert()
  {
    return (expert);
  }

  public void setExpert(boolean expert)
  {
    this.expert = expert;
  }

  public boolean isHidden()
  {
    return (hidden);
  }

  public void setHidden(boolean hidden)
  {
    this.hidden = hidden;
  }

  public String getShortDescription()
  {
    return (sdesc);
  }

  public void setShortDescription(String text)
  {
    sdesc = text;
  }

  public void setValue(String attributeName, Object value)
  {
    table.put(attributeName, value);
  }

  public Object getValue(String attributeName)
  {
    return (table.get(attributeName));
  }

  public Enumeration attributeNames()
  {
    return (table.elements());
  }

  static String capitalize(String name)
  {
    if (name.length() > 2 && Character.isUpperCase(name.charAt(0)) && Character.isUpperCase(name.charAt(1))) {
      return (name);
    }
    else {
      return (Character.toUpperCase(name.charAt(0)) + name.substring(1));
    }
  }

}
