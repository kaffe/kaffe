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

  private String clsnm;
  private String key;

  public MissingResourceException(String s, String className, String ky)
  {
    super(s);
    clsnm = className;
    key = ky;
  }

  public String getClassName()
  {
    return (clsnm);
  }

  public String getKey()
  {
    return (key);
  }

}
