/*
 * Java core library component.
 *
 * Copyright (c) 1997, 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */

package java.net;

import java.lang.String;

public class ConnectException extends SocketException {

  public ConnectException()
  {
    super();
  }

  public ConnectException(String msg)
  {
    super(msg);
  }

}
