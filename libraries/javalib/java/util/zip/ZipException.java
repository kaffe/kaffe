/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.util.zip;

import java.io.IOException;

public class ZipException extends IOException {

  public ZipException()
  {
    super();
  }

  public ZipException(String s)
  {
    super(s);
  }

}
