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

// Not documented !!!!

import java.io.IOException;

class PlainDatagramSocketImpl extends DatagramSocketImpl {

  private int timeout;

  protected void create() throws SocketException
  {
    timeout = 0;
    datagramSocketCreate();
  }

  protected void close()
  {
    datagramSocketClose();
  }

  protected native void bind(int lport, InetAddress laddr) throws SocketException;
  protected native void send(DatagramPacket p) throws IOException;
  protected native int peek(InetAddress i) throws IOException;
  protected native void receive(DatagramPacket p) throws IOException;
  protected native void setTTL(byte ttl) throws IOException;
  protected native byte getTTL() throws IOException;
  protected native void join(InetAddress inetaddr) throws IOException;
  protected native void leave(InetAddress inetaddr) throws IOException;
  private native void datagramSocketCreate();
  private native void datagramSocketClose();
  public native void socketSetOption(int option, Object data);
  public native int socketGetOption(int option);

}
