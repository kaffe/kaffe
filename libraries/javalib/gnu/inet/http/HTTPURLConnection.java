/*
 * $Id: HTTPURLConnection.java,v 1.4 2004/10/30 12:14:13 robilad Exp $
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
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this library with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this library, you may extend
 * this exception to your version of the library, but you are not
 * obliged to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 */

package gnu.inet.http;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.HttpURLConnection;
import java.net.ProtocolException;
import java.net.URL;
import java.security.AccessController;
import java.security.PrivilegedAction;
import java.util.Iterator;
import java.util.Map;

/**
 * A URLConnection that uses the HTTPConnection class.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class HTTPURLConnection
extends HttpURLConnection
{

  /*
   * The underlying connection.
   */
  private HTTPConnection connection;

  private String proxyHostname;
  private int proxyPort;

  private Request request;
  private Headers requestHeaders;
  private ByteArrayOutputStream requestSink;

  private Response response;
  private ByteArrayInputStream responseSink;

  /**
   * Constructor.
   * @param url the URL
   */
  public HTTPURLConnection (URL url)
  {
    super (url);
    requestHeaders = new Headers ();
    AccessController.doPrivileged(this.new GetProxyAction());
  }

  class GetProxyAction implements PrivilegedAction
  {

    public Object run()
    {
      proxyHostname = System.getProperty("http.proxyHost");
      if (proxyHostname != null)
        {
          String port = System.getProperty("http.proxyPort");
          proxyPort = (port != null) ? Integer.parseInt (port) : -1;
        }
      return null;
    }
    
  }

  public void connect ()
    throws IOException
  {
    if (connected)
      {
        return;
      }
    String protocol = url.getProtocol ();
    boolean secure = "https".equals (protocol);
    String host = url.getHost ();
    int port = url.getPort ();
    if (port < 0)
      {
        port = secure ? HTTPConnection.HTTPS_PORT :
          HTTPConnection.HTTP_PORT;
      }
    String file = url.getFile ();
    String username = url.getUserInfo ();
    String password = null;
    if (username != null)
      {
        int ci = username.indexOf (':');
        if (ci != -1)
          {
            password = username.substring (ci + 1);
            username = username.substring (0, ci);
          }
      }
    final Credentials creds = (username == null) ? null :
      new Credentials (username, password);
    
    boolean retry;
    do
      {
        retry = false;
        if (connection == null)
          {
            connection = new HTTPConnection (host, port, secure);
          }
        if (proxyHostname != null)
          {
            if (proxyPort < 0)
              {
                proxyPort = secure ? HTTPConnection.HTTPS_PORT :
                  HTTPConnection.HTTP_PORT;
              }
            connection.setProxy (proxyHostname, proxyPort);
          }
        request = connection.newRequest (method, file);
        request.getHeaders ().putAll (requestHeaders);
        if (requestSink != null)
          {
            byte[] content = requestSink.toByteArray ();
            RequestBodyWriter writer = new ByteArrayRequestBodyWriter (content);
            request.setRequestBodyWriter (writer);
          }
        ByteArrayResponseBodyReader reader = new ByteArrayResponseBodyReader ();
        request.setResponseBodyReader (reader);
        if (creds != null)
          {
            request.setAuthenticator(new Authenticator() {
              public Credentials getCredentials(String realm, int attempts)
              {
                return (attempts < 2) ? creds : null;
              }
            });
          }
        response = request.dispatch ();
        if (response.getCodeClass () == 3 && getInstanceFollowRedirects ())
          {
            // Follow redirect
            String location = response.getHeader ("Location");
            String connectionUri = connection.getURI ();
            int start = connectionUri.length ();
            if (location.startsWith (connectionUri) &&
                location.charAt (start) == '/')
              {
                file = location.substring (start);
                retry = true;
              }
            else if (location.startsWith ("http:"))
              {
                connection.close ();
                connection = null;
                secure = false;
                start = 7;
                int end = location.indexOf ('/', start);
                host = location.substring(start, end);
                int ci = host.lastIndexOf (':');
                if (ci != -1)
                  {
                    port = Integer.parseInt (host.substring (ci + 1));
                    host = host.substring (0, ci);
                  }
                else
                  {
                    port = HTTPConnection.HTTP_PORT;
                  }
                file = location.substring (end);
                retry = true;
              }
            else if (location.startsWith ("https:"))
              {
                connection.close ();
                connection = null;
                secure = true;
                start = 8;
                int end = location.indexOf ('/', start);
                host = location.substring(start, end);
                int ci = host.lastIndexOf (':');
                if (ci != -1)
                  {
                    port = Integer.parseInt (host.substring (ci + 1));
                    host = host.substring (0, ci);
                  }
                else
                  {
                    port = HTTPConnection.HTTPS_PORT;
                  }
                file = location.substring (end);
                retry = true;
              }
            // Otherwise this is not an HTTP redirect, can't follow
          }
        else
          {
            responseSink = new ByteArrayInputStream (reader.toByteArray ());
          }
      }
    while (retry);
    connected = true;
  }

  public void disconnect ()
  {
    if (connection != null)
      {
        try
          {
            connection.close ();
          }
        catch (IOException e)
          {
          }
      }
  }

  public boolean usingProxy ()
  {
    return (proxyHostname != null);
  }

  /**
   * Overrides the corresponding method in HttpURLConnection to permit
   * arbitrary methods, as long as they're valid ASCII alphabetic
   * characters. This is to permit WebDAV and other HTTP extensions to
   * function.
   * @param method the method
   */
  public void setRequestMethod (String method)
    throws ProtocolException
  {
    if (connected)
      {
        throw new ProtocolException ("Already connected");
      }
    // Validate
    method = method.toUpperCase ();
    int len = method.length ();
    if (len == 0)
      {
        throw new ProtocolException ("Empty method name");
      }
    for (int i = 0; i < len; i++)
      {
        char c = method.charAt (i);
        if (c < 0x41 || c > 0x5a)
          {
            throw new ProtocolException ("Illegal character '" + c +
                                         "' at index " + i);
          }
      }
    // OK
    this.method = method;
  }

  public String getRequestProperty (String key)
  {
    return requestHeaders.getValue (key);
  }

  public Map getRequestProperties ()
  {
    return requestHeaders;
  }

  public void setRequestProperty (String key, String value)
  {
    requestHeaders.put (key, value);
  }

  public void addRequestProperty (String key, String value)
  {
    String old = requestHeaders.getValue (key);
    if (old == null)
      {
        requestHeaders.put (key, value);
      }
    else
      {
        requestHeaders.put (key, old + "," + value);
      }
  }

  public OutputStream getOutputStream ()
    throws IOException
  {
    if (connected)
      {
        throw new ProtocolException ("Already connected");
      }
    if (!doOutput)
      {
        throw new ProtocolException ("doOutput is false");
      }
    if (requestSink == null)
      {
        requestSink = new ByteArrayOutputStream ();
      }
    return requestSink;
  }
  
  // -- Response --
  
  public InputStream getInputStream ()
    throws IOException
  {
    if (!connected)
      {
        connect ();
      }
    if (!doInput)
      {
        throw new ProtocolException ("doInput is false");
      }
    return responseSink;
  }

  public Map getHeaderFields ()
  {
    if (!connected)
      {
        return null;
      }
    return response.getHeaders ();
  }

  public String getHeaderField (int index)
  {
    if (!connected)
      {
        return null;
      }
    Iterator i = response.getHeaders ().entrySet ().iterator ();
    Map.Entry entry;
    int count = 0;
    do
      {
        entry = (Map.Entry) i.next ();
        count++;
      }
    while (count < index);
    return (String) entry.getValue ();
  }

  public String getHeaderFieldKey (int index)
  {
    if (!connected)
      {
        return null;
      }
    Iterator i = response.getHeaders ().entrySet ().iterator ();
    Map.Entry entry;
    int count = 0;
    do
      {
        entry = (Map.Entry) i.next ();
        count++;
      }
    while (count < index);
    return (String) entry.getKey ();
  }

}
