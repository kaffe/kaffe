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

public class ExceptionInInitializerError extends LinkageError {

  private Throwable thrown;

  public ExceptionInInitializerError () {
    super();
    thrown = null;
  }

  public ExceptionInInitializerError (Throwable t) {
    super("[exception was " 
	+ t.getClass().getName() + ": " + t.getMessage() + "]");
    thrown = t;
  }

  public ExceptionInInitializerError (String s) {
    super(s);
    thrown = null;
  }

  public Throwable getException() {
    return (thrown);
  }
}
