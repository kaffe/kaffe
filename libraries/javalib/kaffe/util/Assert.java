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

public class Assert {

  static public void true(boolean test, String report)
  {
    if (test == false) {
      throw new Error("Failed assertion: " + report);
    }
  }

  static public void false(boolean test, String report) {
	Assert.true(!test, report);
  }
}
