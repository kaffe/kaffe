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

public interface SocketOptions {
  static final int SO_SNDBUF = 1;
  static final int SO_RCVBUF = 2;
  static final int SO_LINGER = 3;
  static final int SO_TIMEOUT = 4;
  static final int SO_BINDADDR = 5;
  static final int SO_REUSEADDR = 6;
  static final int TCP_NODELAY = 7;
  static final int IP_MULTICAST_IF = 8;

  public void setOption(int option, Object data) throws SocketException;
  public Object getOption(int option) throws SocketException;
};

