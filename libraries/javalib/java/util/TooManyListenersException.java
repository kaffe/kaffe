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

import java.lang.String;

public class TooManyListenersException extends Exception {

  public TooManyListenersException()
  {
    super();
  }

  public TooManyListenersException(String s)
  {
    super(s);
  }

}
