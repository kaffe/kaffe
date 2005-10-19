/* Connection.java -- HTTPS URL connection.
   Copyright (C) 2003,2004  Casey Marshall <rsdio@metastatic.org>
   Parts Copyright (C) 1998,2002  Free Software Foundation, Inc.

This file is a part of Jessie.

Jessie is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

Jessie is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with Jessie; if not, write to the

   Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor,
   Boston, MA  02110-1301
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


package org.metastatic.jessie.https;

import java.io.BufferedOutputStream;
import java.io.BufferedReader;
import java.io.ByteArrayOutputStream;
import java.io.FilterInputStream;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.OutputStream;
import java.io.PrintStream;

import java.net.Socket;
import java.net.URL;

import java.security.Security;
import java.security.cert.X509Certificate;

import java.text.DateFormat;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;
import java.util.StringTokenizer;
import java.util.WeakHashMap;

import javax.net.ssl.HttpsURLConnection;
import javax.net.ssl.SSLPeerUnverifiedException;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;

import org.metastatic.jessie.provider.Jessie;

/**
 * A basic implementation of HttpsURLConnection. This class is based on
 * gnu.java.net.protocol.http.HttpURLConnection from GNU Classpath, written
 * by Aaron M. Renn.
 */
class Connection extends HttpsURLConnection
{

  // Constants and fields.
  // -------------------------------------------------------------------------

  public static final String USER_AGENT = "Jessie/" + Jessie.VERSION;
  public static final int HTTPS_PORT = 443;

  private ArrayList requestProps;
  private ArrayList headers;
  private HashSet redirections;
  private SSLSocket socket;
  private Socket proxySocket;
  private InputStream in;
  private ChunkedInputStream chunkedIn;
  private ConnectionInputStream connIn;
  private OutputStream out;
  private ByteArrayOutputStream bufferedOut;
  private boolean requestSent = false;
  private String host;
  private int port;
  private boolean keepAlive;

  private static final WeakHashMap keptAlive = new WeakHashMap();

  // Constructor.
  // -------------------------------------------------------------------------

  protected Connection(URL url) throws IOException
  {
    super(url);
    doOutput = false;
    headers = new ArrayList(10);
    redirections = new HashSet();
  }

  // HTTPS methods.
  // -------------------------------------------------------------------------

  public String getCipherSuite()
  {
    if (!connected)
      {
        throw new IllegalStateException("not yet connected");
      }
    return socket.getSession().getCipherSuite();
  }

  public java.security.cert.Certificate[] getLocalCertificates()
  {
    if (!connected)
      {
        throw new IllegalStateException("not yet connected");
      }
    return socket.getSession().getLocalCertificates();
  }

  public java.security.cert.Certificate[] getServerCertificates()
    throws SSLPeerUnverifiedException
  {
    if (!connected)
      {
        throw new IllegalStateException("not yet connected");
      }
    return socket.getSession().getPeerCertificates();
  }

  // HttpURLConnection methods.
  // -------------------------------------------------------------------------

  public void disconnect()
  {
    if (!connected)
      return;
    if (!"keep-alive".equalsIgnoreCase (getHeaderField ("Connection")))
      keepAlive = false;
    try
      {
        if (chunkedIn != null)
          {
            if (chunkedIn.eof())
              {
                while (chunkedIn.skip (1024) != -1);
                readHeaders();
              }
          }
        if (keepAlive)
          putKeepAliveSocket (host, port, socket);
        else
          socket.close();
        connected = false;
      }
    catch (IOException ioe)
      {
      }
    connIn = null;
    chunkedIn = null;
    bufferedOut = null;
  }

  public boolean usingProxy()
  {
    return false; // XXX
  }

  // URLConnection methods.
  // -------------------------------------------------------------------------

  public synchronized void connect() throws IOException
  {
    host = url.getHost();
    port = url.getPort();
    if (port == -1)
      {
        port = HTTPS_PORT;
      }

    socket = getKeepAliveSocket (host, port);
    keepAlive = true;

    if (socket == null)
      {
        keepAlive = false;
        SSLSocketFactory factory = getSSLSocketFactory();
        String proxyHost = System.getProperty ("https.proxyHost");
        int proxyPort = Integer.getInteger ("https.proxyPort", 80).intValue();
        if (proxyHost != null)
          socket = (SSLSocket) factory.createSocket
            (getProxySocket (host, port, proxyHost, proxyPort), host, port, true);
        else
          socket = (SSLSocket) factory.createSocket(host, port);

        String timeout = Security.getProperty("jessie.https.timeout");
        if (timeout != null)
          {
            try
              {
                socket.setSoTimeout(Integer.parseInt(timeout));
              }
            catch (Exception x)
              {
              }
          }
        socket.startHandshake();
        X509Certificate cert = (X509Certificate)
          socket.getSession().getPeerCertificates()[0];
        if (!checkHostname(host, cert) &&
            !hostnameVerifier.verify(host, socket.getSession()))
          {
            throw new SSLPeerUnverifiedException("hostname mismatch");
          }
      }

    out = socket.getOutputStream();
    connected = true;
  }

  public synchronized InputStream getInputStream() throws IOException
  {
    if (connIn != null)
      {
        return connIn;
      }
    if (!connected)
      {
        connect();
      }
    in = socket.getInputStream();
    sendRequest();
    receiveReply();
    if ("chunked".equals (getHeaderField ("Transfer-Encoding")))
      {
        chunkedIn = new ChunkedInputStream (in);
        connIn = new ConnectionInputStream (chunkedIn);
      }
    else
      connIn = new ConnectionInputStream (in);
    return connIn;
  }

  public synchronized OutputStream getOutputStream() throws IOException
  {
    if (connected)
      {
        throw new IOException("already connected");
      }
    if (!doOutput)
      {
        throw new IOException("not set up for output");
      }
    if (!method.equals("POST") && !method.equals("PUT"))
      {
        setRequestMethod("POST");
      }
    if (bufferedOut == null)
      {
        bufferedOut = new ByteArrayOutputStream(256);
      }
    return bufferedOut;
  }

  // The header situatuon is totally FUBAR, so we are doing this ourselves.

  public String getContentEncoding()
  {
    return getHeaderField("Content-Encoding");
  }

  public int getContentLength()
  {
    return getHeaderFieldInt("Content-Length", -1);
  }

  public String getContentType()
  {
    return getHeaderField("Content-Type");
  }

  public long getExpiration()
  {
    return getHeaderFieldDate("Expires", 0L);
  }

  public int getHeaderFieldInt(String header, int def)
  {
    try
      {
        return Integer.parseInt(getHeaderField(header));
      }
    catch (NumberFormatException nfe)
      {
        return def;
      }
  }

  public long getHeaderFieldDate(String header, long def)
  {
    try
      {
        DateFormat f = DateFormat.getDateInstance(DateFormat.LONG);
        f.setLenient(true);
        return f.parse(getHeaderField(header)).getTime();
      }
    catch (Exception e)
      {
        return def;
      }
  }

  public long getIfModifiedSince()
  {
    return getHeaderFieldDate("If-Modified-Since", 0L);
  }

  public Map getHeaderFields()
  {
    if (headers == null)
      {
        return null;
      }
    HashMap map = new HashMap(headers.size());
    for (int i = 0; i < headers.size(); i++)
      {
        Header h = (Header) headers.get(i);
        if (map.containsKey(h.getKey()))
          {
            ((List) map.get(h.getKey())).add(h.getValue());
          }
        else
          {
            LinkedList l = new LinkedList();
            l.add(h.getValue());
            map.put(h.getKey(), l);
          }
      }
    for (Iterator it = map.entrySet().iterator(); it.hasNext(); )
      {
        Map.Entry e = (Map.Entry) it.next();
        List l = (List) e.getValue();
        e.setValue(Collections.unmodifiableList(l));
      }
    return Collections.unmodifiableMap(map);
  }

  public String getHeaderField(int n)
  {
    if (!connected)
      {
        throw new IllegalStateException("not connected");
      }
    if (n < 0 || headers == null || n >= headers.size())
      {
        return null;
      }
    return (String) ((Header) headers.get(n)).getValue();
  }

  public String getHeaderField(String key)
  {
    if (!connected)
      {
        throw new IllegalStateException("not connected");
      }
    if (headers == null)
      {
        return null;
      }
    for (Iterator it = headers.iterator(); it.hasNext(); )
      {
        Header h = (Header) it.next();
        if (((String) h.getKey()).equalsIgnoreCase(key))
          {
            return (String) h.getValue();
          }
      }
    return null;
  }

  public String getHeaderFieldKey(int n)
  {
    if (!connected)
      {
        throw new IllegalStateException("not connected");
      }
    if (n < 0 || headers == null || n >= headers.size())
      {
        return null;
      }
    return (String) ((Header) headers.get(n)).getKey();
  }

  public void addRequestProperty(String name, String value)
  {
    if (requestSent)
      {
        throw new IllegalStateException("already connected");
      }
    if (name == null || value == null)
      {
        throw new NullPointerException();
      }
    if (name.trim().length() == 0 || value.trim().length() == 0)
      {
        throw new IllegalArgumentException();
      }
    if (requestProps == null)
      {
        requestProps = new ArrayList(10);
      }
    for (Iterator it = requestProps.iterator(); it.hasNext(); )
      {
        Header h = (Header) it.next();
        if (((String) h.getKey()).equalsIgnoreCase(name))
          {
            h.setValue(h.getValue() + ", " + value);
            return;
          }
      }
    requestProps.add(new Header(name, value));
  }

  public void setRequestProperty(String name, String value)
  {
    if (requestSent)
      {
        throw new IllegalStateException("already connected");
      }
    if (name == null || value == null)
      {
        throw new NullPointerException();
      }
    if (name.trim().length() == 0 || value.trim().length() == 0)
      {
        throw new IllegalArgumentException();
      }
    if (requestProps == null)
      {
        requestProps = new ArrayList(10);
      }
    for (Iterator it = requestProps.iterator(); it.hasNext(); )
      {
        Header h = (Header) it.next();
        if (((String) h.getKey()).equalsIgnoreCase(name))
          {
            it.remove();
            break;
          }
      }
    requestProps.add(new Header(name, value));
  }

  public String getRequestProperty(String name)
  {
    if (requestSent)
      {
        throw new IllegalStateException("already connected");
      }
    if (name == null || requestProps == null)
      {
        return null;
      }
    for (Iterator it = requestProps.iterator(); it.hasNext(); )
      {
        Header h = (Header) it.next();
        if (((String) h.getKey()).equalsIgnoreCase(name))
          {
            return (String) h.getValue();
          }
      }
    return null;
  }

  public Map getRequestProperties()
  {
    if (requestSent)
      {
        throw new IllegalStateException("already connected");
      }
    HashMap map = new HashMap(requestProps.size());
    for (Iterator it = requestProps.iterator(); it.hasNext(); )
      {
        Header h = (Header) it.next();
        map.put(h.getKey(), Collections.singletonList(h.getValue()));
      }
    return Collections.unmodifiableMap(map);
  }

  // Own methods.
  // -------------------------------------------------------------------------

  private boolean checkHostname(String host, X509Certificate cert)
  {
    String name = cert.getSubjectDN().getName();
    StringTokenizer tok = new StringTokenizer(name, ",+");
    String common_name = null;

    System.err.println("name=" + name);
    while (tok.hasMoreTokens())
      {
        String s = tok.nextToken().trim().toLowerCase();
        if (s.startsWith("cn="))
          {
            common_name = s.substring(3);
            break;
          }
      }
    if (common_name == null)
      {

        return false;
      }
    boolean match = false;
    if (host.startsWith("www."))
      match = common_name.indexOf(host.substring(4)) >= 0;
    return match || common_name.indexOf(host) >= 0;
  }

  private void sendRequest() throws IOException
  {
    BufferedOutputStream bout = new BufferedOutputStream(out);
    PrintStream httpout = new PrintStream(bout);
    String file = getURL().getFile();
    if (file == null || file.length() == 0)
      file = "/";
    httpout.print(getRequestMethod() + " " + file + " HTTP/1.1\r\n");

    if (getRequestProperty("Host") == null)
      {
        setRequestProperty("Host", getURL().getHost());
      }
    if (getRequestProperty("Connection") == null)
      {
        setRequestProperty("Connection", "Keep-Alive");
        keepAlive = true;
      }
    else if ("Keep-Alive".equals (getRequestProperty("Connection")))
      keepAlive = true;
    if (getRequestProperty("User-Agent") == null)
      {
        setRequestProperty("User-Agent", USER_AGENT);
      }
    if (getRequestProperty("Accept") == null)
      {
        setRequestProperty("Accept", "*/*");
      }
    if (getRequestProperty("Content-type") == null)
      {
        setRequestProperty("Content-type", "application/x-www-form-urlencoded");
      }
    if (getRequestProperty("Content-length") == null && bufferedOut != null)
      {
        setRequestProperty("Content-length", String.valueOf(bufferedOut.size()));
      }

    Iterator it = requestProps.iterator();
    while (it.hasNext())
      {
        Header h = (Header) it.next();
        String name = (String) h.getKey();
        String value = (String) h.getValue();
        if (name.length() + value.length() + 2 < 72)
          {
            httpout.print(name + ": " + value + "\r\n");
          }
        else
          {
            httpout.print(name + ": ");
            int idx = name.length() + 2;
            httpout.print(value.substring(0, idx));
            value = value.substring(idx + 1);
            while (value != null)
              {
                httpout.print("\t");
                idx = Math.min(72, value.length());
                httpout.print(value.substring(0, idx));
                if (idx == value.length())
                  {
                    value = null;
                  }
                else
                  {
                    value = value.substring(idx + 1);
                  }
              }
            httpout.print("\r\n");
          }
      }

    httpout.print("\r\n");
    httpout.flush();

    if (bufferedOut != null)
      {
        bufferedOut.writeTo(bout);
        bout.flush();
      }

    requestSent = true;
  }

  private void receiveReply() throws IOException
  {
    while (true)
      {
        String line = readLine();

        int idx = line.indexOf(" ");
        if (idx < 0 || line.length() < idx + 6)
          {
            throw new IOException("malformed HTTP reply");
          }

        line = line.substring(idx + 1);
        String code = line.substring(0, 3);
        try
          {
            responseCode = Integer.parseInt(code);
          }
        catch (NumberFormatException nfe)
          {
            throw new IOException("malformed HTTP reply: " +
                                  "unparsable response code: " + code);
          }
        responseMessage = line.substring(4);
        readHeaders();
        if (!"keep-alive".equalsIgnoreCase (getHeaderField ("Connection")))
          keepAlive = false;

        if (getInstanceFollowRedirects() && responseCode / 100 == 3)
          {
            redirections.add (url);
            URL redirect = new URL (getHeaderField ("Location"));
            if (redirections.contains (redirect))
              throw new IOException ("looping redirections discovered");
            url = redirect;
            if (!url.getProtocol().equals ("https"))
              throw new IOException ("cannot be redirected to " + url);
            connect();
            headers.clear();
            sendRequest();
            continue;
          }

        break;
      }
  }

  private void readHeaders() throws IOException
  {
    int idx = 0;
    String line = null;
    String key = null;
    StringBuffer value = new StringBuffer();
    while (true)
      {
        line = readLine();
        if (line.trim().length() == 0)
          {
            break;
          }

        if (line.startsWith(" ") || line.startsWith("\t"))
          {
            line = line.trim();
            do
              {
                if (line.length() == 1)
                  {
                    throw new IOException("malformed header");
                  }
                line = line.substring(1);
              }
            while (line.startsWith(" ") || line.startsWith("\t"));
            value.append(' ');
            value.append(line);
          }
        else
          {
            if (key != null)
              {
                headers.add(new Header(key, value.toString()));
                key = null;
                value.setLength(0);
              }
            idx = line.indexOf(":");
            if (idx < 0 || line.length() < idx + 2)
              {
                throw new IOException("malformed header");
              }

            key = line.substring(0, idx);
            line = line.substring(idx + 1);
            while (line.startsWith(" ") || line.startsWith("\t"))
              {
                if (line.length() == 1)
                  {
                    throw new IOException("malformed header");
                  }
                line = line.substring(1);
              }
            value.append(line);
          }
      }
    if (key != null)
      {
        headers.add(new Header(key, value.toString()));
      }
  }

  private String readLine() throws IOException
  {
    return readLine (in);
  }

  private static String readLine (InputStream in) throws IOException
  {
    StringBuffer line = new StringBuffer();
    while (true)
      {
        int i = in.read();
        if (i == -1 || i == '\n')
          {
            break;
          }
        if (i != '\r')
          {
            line.append((char) i);
          }
      }
    return line.toString();
  }

  private static Socket getProxySocket (String host, int port,
                                        String proxyHost, int proxyPort)
    throws IOException
  {
    Socket socket = new Socket (proxyHost, proxyPort);
    System.out.println (socket);
    PrintStream out = new PrintStream (socket.getOutputStream());
    out.print ("CONNECT " + host + ":" + port + " HTTP/1.0\r\n");
//     out.print ("User-Agent: " + USER_AGENT + "\r\n");
//     out.print ("Host: " + host + "\r\n");
    out.print ("\r\n");
    out.flush();

    InputStream in = socket.getInputStream();
    String line = readLine (in);
    if (line == null)
      throw new IOException ("proxy " + proxyHost + " did not reply");

    int idx = line.indexOf(" ");
    if (idx < 0 || line.length() < idx + 6)
      {
        throw new IOException("proxy " + proxyHost + " sent malformed HTTP reply");
      }

    line = line.substring(idx + 1);
    String code = line.substring(0, 3);
    int responseCode;
    String responseMessage = null;
    try
      {
        responseCode = Integer.parseInt(code);
      }
    catch (NumberFormatException nfe)
      {
        throw new IOException("proxy " + proxyHost +
                              " sent malformed HTTP reply: " +
                              "unparsable response code: " + code);
      }
    responseMessage = line.substring(4);

    if (responseCode / 100 != 2)
      {
        throw new IOException ("proxy " + proxyHost + " connect failed: " +
                               responseCode + " " + responseMessage);
      }

    while ((line = readLine (in)) != null && line.length() > 0);
    return socket;
  }

  private static SSLSocket getKeepAliveSocket (String host, int port)
  {
    synchronized (keptAlive)
      {
        String key = host + ":" + port;
        SSLSocket s = (SSLSocket) keptAlive.remove (key);
        if (s == null || s.isClosed())
          return null;
        return s;
      }
  }

  private static void putKeepAliveSocket (String host, int port, SSLSocket socket)
  {
    synchronized (keptAlive)
      {
        String key = host + ":" + port;
        SSLSocket s = (SSLSocket) keptAlive.get (key);
        if (s == null || s.isClosed())
          keptAlive.put (key, socket);
      }
  }

  // Inner class.
  // -------------------------------------------------------------------------

  private class Header implements Map.Entry
  {

    // Instance methods.
    // -----------------------------------------------------------------------

    private final String name;
    private String value;

    // Constructor.
    // -----------------------------------------------------------------------

    public Header(String name, String value)
    {
      this.name = name;
      this.value = value;
    }

    // Instance methods.
    // -----------------------------------------------------------------------

    public Object getKey()
    {
      return name;
    }

    public Object getValue()
    {
      return value;
    }

    public boolean equals(Object o)
    {
      return ((Header) o).name.equalsIgnoreCase(name) &&
             ((Header) o).value.equalsIgnoreCase(value);
    }

    public int hashCode()
    {
      return name.hashCode() ^ value.hashCode();
    }

    public Object setValue(Object value)
    {
      String oldval = this.value;
      this.value = (String) value;
      return oldval;
    }
  }

  /**
   * An input stream that will throw an exception if the underlying URL
   * connection is disconnected (but, possibly, the socket was not closed).
   */
  private class ConnectionInputStream extends FilterInputStream
  {

    // Costructor.
    // -----------------------------------------------------------------------

    ConnectionInputStream (InputStream in)
    {
      super (in);
    }

    // Methods.
    // -----------------------------------------------------------------------

    public int available() throws IOException
    {
      if (!Connection.this.connected)
        throw new IOException ("not connected");
      return super.available();
    }

    public void close() throws IOException
    {
      if (!Connection.this.connected)
        throw new IOException ("not connected");
      super.close();
    }

    public void mark (int readLimit)
    {
      if (!Connection.this.connected)
        throw new IllegalStateException ("not connected");
      super.mark (readLimit);
    }

    public boolean markSupported()
    {
      if (!Connection.this.connected)
        return false;
      return super.markSupported();
    }

    public int read() throws IOException
    {
      if (!Connection.this.connected)
        throw new IOException ("not connected");
      return super.read();
    }

    public int read (byte[] buf) throws IOException
    {
      if (!Connection.this.connected)
        throw new IOException ("not connected");
      return super.read (buf);
    }

    public int read (byte[] buf, int off, int len) throws IOException
    {
      if (!Connection.this.connected)
        throw new IOException ("not connected");
      return super.read (buf, off, len);
    }

    public void reset() throws IOException
    {
      if (!Connection.this.connected)
        throw new IOException ("not connected");
      super.reset();
    }

    public long skip (long bytes) throws IOException
    {
      if (!Connection.this.connected)
        throw new IOException ("not connected");
      return super.skip (bytes);
    }
  }
}
