/* SSLSocket.java -- an SSL client socket.
   Copyright (C) 2003  Casey Marshall <rsdio@metastatic.org>

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the

   Free Software Foundation, Inc.,
   59 Temple Place, Suite 330,
   Boston, MA  02111-1307
   USA

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under terms
of your choice, provided that you also meet, for each linked independent
module, the terms and conditions of the license of that module.  An
independent module is a module which is not derived from or based on
this library.  If you modify this library, you may extend this exception
to your version of the library, but you are not obligated to do so.  If
you do not wish to do so, delete this exception statement from your
version.  */


package javax.net.ssl;

import java.io.IOException;
import java.net.InetAddress;
import java.net.Socket;
import java.net.UnknownHostException;

public abstract class SSLSocket extends Socket
{

  // Constructors.
  // --------------------------------------------------------------------

  protected SSLSocket() throws IOException
  {
    super();
  }

  protected SSLSocket(String host, int port)
    throws IOException, UnknownHostException
  {
    super(host, port);
  }

  protected SSLSocket(InetAddress address, int port) throws IOException
  {
    super(address, port);
  }

  protected SSLSocket(String host, int port,
                      InetAddress localAddr, int localPort)
    throws IOException, UnknownHostException
  {
    super(host, port, localAddr, localPort);
  }

  protected SSLSocket(InetAddress address, int port,
                      InetAddress localAddr, int localPort)
    throws IOException
  {
    super(address, port, localAddr, localPort);
  }

  // Abstract methods.
  // -------------------------------------------------------------------

  public abstract void
  addHandshakeCompletedListener(HandshakeCompletedListener listener);

  public abstract void
  removeHandshakeCompletedListener(HandshakeCompletedListener listener);

  public abstract String[] getEnabledCipherSuites();

  public abstract void setEnabledCipherSuites(String[] suites);

  public abstract String[] getEnabledProtocols();

  public abstract void setEnabledProtocols(String[] protocols);

  public abstract boolean getEnableSessionCreation();

  public abstract void setEnableSessionCreation(boolean enable);

  public abstract boolean getNeedClientAuth();

  public abstract void setNeedClientAuth(boolean needAuth);

  public abstract SSLSession getSession();

  public abstract String[] getSupportedCipherSuites();

  public abstract String[] getSupportedProtocols();

  public abstract boolean getUseClientMode();

  public abstract void setUseClientMode(boolean clientMode);

  public abstract boolean getWantClientAuth();

  public abstract void setWantClientAuth(boolean wantAuth);

  public abstract void startHandshake() throws IOException;
}
