/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util;


public class MissingResourceException extends RuntimeException {

  // NB: both are part of serialized form
  private String className;
  private String key;

  public MissingResourceException(String s, String className, String ky)
  {
    super(s + "\t" + className + "\t" + ky);
    this.className = className;
    key = ky;
  }

  public String getClassName()
  {
    return (className);
  }

  public String getKey()
  {
    return (key);
  }

}
