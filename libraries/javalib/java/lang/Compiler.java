/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.lang;

public final class Compiler {
  public static native boolean compileClass(Class aClass);
  public static native boolean compileClasses(String string);
  public static native Object command(Object any);
  public static native void enable();
  public static native void disable();
}
