/*
 * $Id: HTTPConnection.java,v 1.1 2004/07/25 22:46:19 dalibor Exp $
 * Copyright (C) 2004 The Free Software Foundation
 * 
 * This file is part of GNU inetlib, a library.
 * 
 * GNU inetlib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * GNU inetlib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License.
 */

package gnu.inet.http;

import java.io.IOException;
import java.net.Socket;
import java.security.GeneralSecurityException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import javax.net.SocketFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManager;

import gnu.inet.http.event.ConnectionEvent;
import gnu.inet.http.event.ConnectionListener;
import gnu.inet.http.event.RequestEvent;
import gnu.inet.http.event.RequestListener;
import gnu.inet.util.EmptyX509TrustManager;

/**
 * A connection to an HTTP server.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class HTTPConnection
{

  /**
   * The default HTTP port.
   */
  public static final int HTTP_PORT = 80;

  /**
   * The default HTTPS port.
   */
  public static final int HTTPS_PORT = 443;

  private static final String userAgent;
  static
  {
    String newUserAgent;

    try
      {
        StringBuffer buf = new StringBuffer("inetlib/1.1 (");
        buf.append (System.getProperty ("os.name"));
        buf.append ("; ");
        buf.append (System.getProperty ("os.arch"));
        buf.append ("; ");
        buf.append (System.getProperty ("user.language"));
        buf.append (")");
        newUserAgent = buf.toString ();
      }
    catch (SecurityException e)
      {
        newUserAgent = "inetlib/1.1";
      }

    userAgent = newUserAgent;
  }

  /**
   * The host name of the server to connect to.
   */
  protected final String hostname;

  /**
   * The port to connect to.
   */
  protected final int port;

  /**
   * Whether the connection should use transport level security (HTTPS).
   */
  protected final boolean secure;

  /**
   * The host name of the proxy to connect to.
   */
  protected String proxyHostname;

  /**
   * The port on the proxy to connect to.
   */
  protected int proxyPort;

  /**
   * The major version of HTTP supported by this client.
   */
  protected int majorVersion;

  /**
   * The minor version of HTTP supported by this client.
   */
  protected int minorVersion;

  private final List connectionListeners;
  private final List requestListeners;

  /**
   * The socket this connection communicates on.
   */
  protected Socket socket;

  /**
   * Creates a new HTTP connection.
   * @param hostname the name of the host to connect to
   */
  public HTTPConnection (String hostname)
  {
    this (hostname, HTTP_PORT, false);
  }

  /**
   * Creates a new HTTP or HTTPS connection.
   * @param hostname the name of the host to connect to
   * @param secure whether to use a secure connection
   */
  public HTTPConnection (String hostname, boolean secure)
  {
    this (hostname, secure ? HTTP_PORT : HTTPS_PORT, secure);
  }

  /**
   * Creates a new HTTP connection on the specified port.
   * @param hostname the name of the host to connect to
   * @param port the port on the host to connect to
   */
  public HTTPConnection (String hostname, int port)
  {
    this (hostname, port, false);
  }

  /**
   * Creates a new HTTP or HTTPS connection on the specified port.
   * @param hostname the name of the host to connect to
   * @param port the port on the host to connect to
   * @param secure whether to use a secure connection
   */
  public HTTPConnection (String hostname, int port, boolean secure)
  {
    this.hostname = hostname;
    this.port = port;
    this.secure = secure;
    majorVersion = minorVersion = 1;
    connectionListeners = Collections.synchronizedList (new ArrayList (4));
    requestListeners = Collections.synchronizedList (new ArrayList (4));
  }

  /**
   * Returns the name of the host to connect to.
   */
  public String getHostName ()
  {
    return hostname;
  }

  /**
   * Returns the port on the host to connect to.
   */
  public int getPort ()
  {
    return port;
  }

  /**
   * Indicates whether to use a secure connection or not.
   */
  public boolean isSecure ()
  {
    return secure;
  }

  /**
   * Returns the HTTP version string supported by this connection.
   * @see #version
   */
  public String getVersion ()
  {
    return "HTTP/" + majorVersion + '.' + minorVersion;
  }

  /**
   * Sets the HTTP version supported by this connection.
   * @param majorVersion the major version
   * @param minorVersion the minor version
   */
  public void setVersion (int majorVersion, int minorVersion)
  {
    if (majorVersion != 1)
      {
        throw new IllegalArgumentException ("major version not supported: " +
                                            majorVersion);
      }
    if (minorVersion < 0 || minorVersion > 1)
      {
        throw new IllegalArgumentException ("minor version not supported: " +
                                            minorVersion);
      }
    this.majorVersion = majorVersion;
    this.minorVersion = minorVersion;
  }

  /**
   * Directs this connection to use the specified proxy.
   * @param hostname the proxy host name
   * @param port the port on the proxy to connect to
   */
  public void setProxy (String hostname, int port)
  {
    proxyHostname = hostname;
    proxyPort = port;
  }

  /**
   * Indicates whether this connection is using an HTTP proxy.
   */
  public boolean isUsingProxy ()
  {
    return (proxyHostname != null && proxyPort > 0);
  }

  /**
   * Creates a new request using this connection.
   * @param method the HTTP method to invoke
   * @param path the URI-escaped RFC2396 <code>abs_path</code> with
   * optional query part
   */
  public Request newRequest (String method, String path)
  {
    Request ret = new Request (this, method, path);
    ret.setHeader ("Host", hostname);
    ret.setHeader ("User-Agent", userAgent);
    fireRequestEvent (RequestEvent.REQUEST_CREATED, ret);
    return ret;
  }

  /**
   * Closes this connection.
   */
  public void close ()
    throws IOException
  {
    closeConnection ();
    fireConnectionEvent (ConnectionEvent.CONNECTION_CLOSED);
  }

  /**
   * Retrieves the socket associated with this connection.
   * This creates the socket if necessary.
   */
  protected Socket getSocket ()
    throws IOException
  {
    if (socket == null)
      {
        String connectHostname = hostname;
        int connectPort = port;
        if (isUsingProxy ())
          {
            connectHostname = proxyHostname;
            connectPort = proxyPort;
          }
        socket = new Socket (connectHostname, connectPort);
        if (secure)
          {
            try
              {
                TrustManager tm = new EmptyX509TrustManager ();
                SSLContext context = SSLContext.getInstance ("SSL");
                TrustManager[] trust = new TrustManager[] { tm };
                context.init (null, trust, null);
                SSLSocketFactory factory =  context.getSocketFactory ();
                SSLSocket ss =
                  (SSLSocket) factory.createSocket (socket, connectHostname,
                                                    connectPort, true);
                String[] protocols = { "TLSv1", "SSLv3" };
                ss.setEnabledProtocols (protocols);
                ss.setUseClientMode (true);
                ss.startHandshake ();
                socket = ss;
              }
            catch (GeneralSecurityException e)
              {
                throw new IOException (e.getMessage ());
              }
          }
      }
    return socket;
  }

  /**
   * Closes the underlying socket, if any.
   */
  protected void closeConnection ()
    throws IOException
  {
    if (socket != null)
      {
        socket.close ();
        socket = null;
      }
  }

  /**
   * Returns a URI representing the connection.
   * This does not include any request path component.
   */
  protected String getURI ()
  {
    StringBuffer buf = new StringBuffer ();
    buf.append (secure ? "https://" : "http://");
    buf.append (hostname);
    if (secure)
      {
        if (port != HTTPConnection.HTTPS_PORT)
          {
            buf.append (':');
            buf.append (port);
          }
      }
    else
      {
        if (port != HTTPConnection.HTTP_PORT)
          {
            buf.append (':');
            buf.append (port);
          }
      }
    return buf.toString ();
  }

  // -- Events --
  
  public void addConnectionListener (ConnectionListener l)
  {
    synchronized (connectionListeners)
      {
        connectionListeners.add (l);
      }
  }

  public void removeConnectionListener (ConnectionListener l)
  {
    synchronized (connectionListeners)
      {
        connectionListeners.remove (l);
      }
  }

  protected void fireConnectionEvent (int type)
  {
    ConnectionEvent event = new ConnectionEvent (this, type);
    ConnectionListener[] l = null;
    synchronized (connectionListeners)
      {
        l = new ConnectionListener[connectionListeners.size ()];
        connectionListeners.toArray (l);
      }
    for (int i = 0; i < l.length; i++)
      {
        switch (type)
          {
          case ConnectionEvent.CONNECTION_CLOSED:
            l[i].connectionClosed (event);
            break;
          }
      }
  }

  public void addRequestListener (RequestListener l)
  {
    synchronized (requestListeners)
      {
        requestListeners.add (l);
      }
  }

  public void removeRequestListener (RequestListener l)
  {
    synchronized (requestListeners)
      {
        requestListeners.remove (l);
      }
  }

  protected void fireRequestEvent (int type, Request request)
  {
    RequestEvent event = new RequestEvent (this, type, request);
    RequestListener[] l = null;
    synchronized (requestListeners)
      {
        l = new RequestListener[requestListeners.size ()];
        requestListeners.toArray (l);
      }
    for (int i = 0; i < l.length; i++)
      {
        switch (type)
          {
          case RequestEvent.REQUEST_CREATED:
            l[i].requestCreated (event);
            break;
          case RequestEvent.REQUEST_SENDING:
            l[i].requestSent (event);
            break;
          case RequestEvent.REQUEST_SENT:
            l[i].requestSent (event);
            break;
          }
      }
  }

}
