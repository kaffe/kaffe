/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package kaffe.util;


/**
 * This class consists of static methods that perform assertion checks.
 * If a check fails, we throw an error with a descriptive message.
 */
public class Assert {
  private static final String MSG = "assertion failure";

  /**
   * Throw an error if not <code>true</code>.
   *
   * @param	x	The value to test
   */
  public static void that(boolean x) {
    if (!x) {
      fail();
    }
  }

  /**
   * Throw an error if not <code>true</code>.
   *
   * @param	x	The value to test
   * @param	msg	Description of the failure
   */
  public static void that(boolean x, String msg) {
    if (!x) {
      fail(msg);
    }
  }

  /**
   * Throw an error if equal to zero.
   *
   * @param	x	The value to test
   */
  public static void that(long x) {
    if (x == 0) {
      fail();
    }
  }

  /**
   * Throw an error if equal to zero.
   *
   * @param	x	The value to test
   * @param	msg	Description of the failure
   */
  public static void that(long x, String msg) {
    if (x == 0) {
      fail(msg);
    }
  }

  /**
   * Throw an error if equal to zero.
   *
   * @param	x	The value to test
   */
  public static void that(double x) {
    if (x == 0.0) {
      fail();
    }
  }

  /**
   * Throw an error if equal to zero.
   *
   * @param	x	The value to test
   * @param	msg	Description of the failure
   */
  public static void that(double x, String msg) {
    if (x == 0.0) {
      fail(msg);
    }
  }

  /**
   * Throw an error if equal to <code>null</code>.
   *
   * @param	x	The value to test
   */
  public static void that(Object x) {
    if (x == null) {
      fail();
    }
  }

  /**
   * Throw an error if equal to <code>null</code>.
   *
   * @param	x	The value to test
   * @param	msg	Description of the failure
   */
  public static void that(Object x, String msg) {
    if (x == null) {
      fail(msg);
    }
  }

  /**
   * Abort execution due to some failure.
   */
  public static void fail() {
    throw new Error(MSG);
  }

  /**
   * Abort execution due to some failure.
   *
   * @param	msg	Description of the failure
   */
  public static void fail(String msg) {
    throw new Error(msg == null ? MSG : MSG + ": " + msg);
  }
}

