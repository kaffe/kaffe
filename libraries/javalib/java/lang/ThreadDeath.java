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

public class ThreadDeath extends Error {
  public ThreadDeath() {};

  /**
   * this constructor is for internal VM use only (SignalError) 
   */
  ThreadDeath(String s) { super(s); };
}
