/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang.reflect;

public class Modifier {

  public static final int PUBLIC = 0x0001;
  public static final int PRIVATE = 0x0002;
  public static final int PROTECTED = 0x0004;
  public static final int STATIC = 0x0008;
  public static final int FINAL = 0x0010;
  public static final int SYNCHRONIZED = 0x0020;
  public static final int VOLATILE = 0x0040;
  public static final int TRANSIENT = 0x0080;
  public static final int NATIVE = 0x0100;
  public static final int INTERFACE = 0x0200;
  public static final int ABSTRACT = 0x0400;

  public Modifier()
  {
	// ???
  }

  public static boolean isPublic(int mod)
  {
    return ((mod & PUBLIC) == 0 ? false : true);
  }

  public static boolean isPrivate(int mod)
  {
    return ((mod & PRIVATE) == 0 ? false : true);
  }

  public static boolean isProtected(int mod)
  {
    return ((mod & PROTECTED) == 0 ? false : true);
  }

  public static boolean isStatic(int mod)
  {
    return ((mod & STATIC) == 0 ? false : true);
  }

  public static boolean isFinal(int mod)
  {
    return ((mod & FINAL) == 0 ? false : true);
  }

  public static boolean isSynchronized(int mod)
  {
    return ((mod & SYNCHRONIZED) == 0 ? false : true);
  }

  public static boolean isVolatile(int mod)
  {
    return ((mod & VOLATILE) == 0 ? false : true);
  }

  public static boolean isTransient(int mod)
  {
    return ((mod & TRANSIENT) == 0 ? false : true);
  }

  public static boolean isNative(int mod)
  {
    return ((mod & NATIVE) == 0 ? false : true);
  }

  public static boolean isInterface(int mod)
  {
    return ((mod & INTERFACE) == 0 ? false : true);
  }

  public static boolean isAbstract(int mod)
  {
    return ((mod & ABSTRACT) == 0 ? false : true);
  }

  public static String toString(int mod)
  {
    StringBuffer str = new StringBuffer();

    if ((mod & PUBLIC) != 0) {
      str.append("public ");
    }
    if ((mod & PRIVATE) != 0) {
      str.append("private ");
    }
    if ((mod & PROTECTED) != 0) {
      str.append("protected ");
    }
    if ((mod & STATIC) != 0) {
      str.append("static ");
    }
    if ((mod & FINAL) != 0) {
      str.append("final ");
    }
    if ((mod & SYNCHRONIZED) != 0) {
      str.append("synchronized ");
    }
    if ((mod & VOLATILE) != 0) {
      str.append("volatile ");
    }
    if ((mod & TRANSIENT) != 0) {
      str.append("transient ");
    }
    if ((mod & NATIVE) != 0) {
      str.append("native ");
    }
    if ((mod & INTERFACE) != 0) {
      str.append("interface ");
    }
    if ((mod & ABSTRACT) != 0) {
      str.append("abstract ");
    }
    return (new String(str));
  }
};
