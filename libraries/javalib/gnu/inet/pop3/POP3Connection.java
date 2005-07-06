/*
 * POP3Connection.java
 * Copyright (C) 2003 The Free Software Foundation
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

package gnu.inet.pop3;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.ProtocolException;
import java.net.Socket;
import java.net.UnknownHostException;
import java.security.GeneralSecurityException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.logging.Level;
import java.util.logging.Logger;

import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLSocket;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManager;

import javax.security.auth.callback.CallbackHandler;
import javax.security.sasl.Sasl;
import javax.security.sasl.SaslClient;
import javax.security.sasl.SaslException;

import gnu.inet.util.BASE64;
import gnu.inet.util.CRLFInputStream;
import gnu.inet.util.CRLFOutputStream;
import gnu.inet.util.EmptyX509TrustManager;
import gnu.inet.util.LineInputStream;
import gnu.inet.util.MessageInputStream;
import gnu.inet.util.SaslCallbackHandler;
import gnu.inet.util.SaslCramMD5;
import gnu.inet.util.SaslInputStream;
import gnu.inet.util.SaslLogin;
import gnu.inet.util.SaslOutputStream;
import gnu.inet.util.SaslPlain;
import gnu.inet.util.TraceLevel;

/**
 * A POP3 client connection.
 * This implements the entire POP3 specification as detailed in RFC 1939,
 * with the exception of the no-arg LIST and UIDL commands (use STAT
 * followed by multiple LIST and/or UIDL instead) and TOP with a specified
 * number of content lines. It also implements the POP3 extension mechanism
 * CAPA, documented in RFC 2449, as well as the STLS command to initiate TLS
 * over POP3 documented in RFC 2595 and the AUTH command in RFC 1734.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class POP3Connection
{

  /**
   * The logger used for POP3 protocol traces.
   */
  public static final Logger logger = Logger.getLogger("gnu.inet.pop3");

  /**
   * The network trace level.
   */
  public static final Level POP3_TRACE = new TraceLevel("pop3");

  /**
   * The default POP3 port.
   */
  public static final int DEFAULT_PORT = 110;

  // -- POP3 vocabulary --
  private static final String _OK = "+OK";
  private static final String _ERR = "-ERR";
  private static final String _READY = "+ ";

  protected static final String STAT = "STAT";
  protected static final String LIST = "LIST";
  protected static final String RETR = "RETR";
  protected static final String DELE = "DELE";
  protected static final String NOOP = "NOOP";
  protected static final String RSET = "RSET";
  protected static final String QUIT = "QUIT";
  protected static final String TOP = "TOP";
  protected static final String UIDL = "UIDL";
  protected static final String USER = "USER";
  protected static final String PASS = "PASS";
  protected static final String APOP = "APOP";
  protected static final String CAPA = "CAPA";
  protected static final String STLS = "STLS";
  protected static final String AUTH = "AUTH";

  protected static final int OK = 0;
  protected static final int ERR = 1;
  protected static final int READY = 2;

  /**
   * The socket used to communicate with the server.
   */
  protected Socket socket;

  /**
   * The socket input stream.
   */
  protected LineInputStream in;

  /**
   * The socket output stream.
   */
  protected CRLFOutputStream out;

  /**
   * The last response received from the server.
   * The status code (+OK or -ERR) is stripped from the line.
   */
  protected String response;

  /**
   * The APOP timestamp, if sent by the server on connection.
   * Otherwise null.
   */
  protected byte[] timestamp;

  /**
   * Creates a new connection to the server.
   * @param hostname the hostname of the server to connect to
   */
  public POP3Connection(String hostname)
    throws UnknownHostException, IOException
  {
    this(hostname, -1, 0, 0, false, null);
  }

  /**
   * Creates a new connection to the server.
   * @param hostname the hostname of the server to connect to
   * @param port the port to connect to(if &lt;=0, use default POP3 port)
   */
  public POP3Connection(String hostname, int port)
    throws UnknownHostException, IOException
  {
    this(hostname, port, 0, 0, false, null);
  }

  /**
   * Creates a new connection to the server.
   * @param hostname the hostname of the server to connect to
   * @param port the port to connect to(if &lt;=0, use default POP3 port)
   * @param connectionTimeout the connection timeout, in milliseconds
   * @param timeout the I/O timeout, in milliseconds
   */
  public POP3Connection(String hostname, int port,
                        int connectionTimeout, int timeout)
    throws UnknownHostException, IOException
  {
    this(hostname, port, connectionTimeout, timeout, false, null);
  }

  /**
   * Creates a new connection to the server.
   * @param hostname the hostname of the server to connect to
   * @param port the port to connect to(if &lt;=0, use default POP3 port)
   * @param connectionTimeout the connection timeout, in milliseconds
   * @param timeout the I/O timeout, in milliseconds
   * @param secure if true, create a POP3S connection
   * @param tm a trust manager used to check SSL certificates, or null to
   * use the default
   */
  public POP3Connection(String hostname, int port,
                        int connectionTimeout, int timeout,
                        boolean secure, TrustManager tm)
    throws UnknownHostException, IOException
  {
    if (port <= 0)
      {
        port = DEFAULT_PORT;
      }
    
    // Set up socket
    try
      {
        socket = new Socket();
        InetSocketAddress address = new InetSocketAddress(hostname, port);
        if (connectionTimeout > 0)
          {
            socket.connect(address, connectionTimeout);
          }
        else
          {
            socket.connect(address);
          }
        if (timeout > 0)
          {
            socket.setSoTimeout(timeout);
          }
        if (secure)
          {
            SSLSocketFactory factory = getSSLSocketFactory(tm);
            SSLSocket ss =
              (SSLSocket) factory.createSocket(socket, hostname, port, true);
            String[] protocols = { "TLSv1", "SSLv3" };
            ss.setEnabledProtocols(protocols);
            ss.setUseClientMode(true);
            ss.startHandshake();
            socket = ss;
          }
      }
    catch (GeneralSecurityException e)
      {
        IOException e2 = new IOException();
        e2.initCause(e);
        throw e2;
      }
    
    InputStream in = socket.getInputStream();
    in = new BufferedInputStream(in);
    in = new CRLFInputStream(in);
    this.in = new LineInputStream(in);
    OutputStream out = socket.getOutputStream();
    out = new BufferedOutputStream(out);
    this.out = new CRLFOutputStream(out);

    if (getResponse() != OK)
      {
        throw new ProtocolException("Connect failed: " + response);
      }
    // APOP timestamp
    timestamp = parseTimestamp(response);
  }

  /**
   * Authenticates the connection using the specified SASL mechanism,
   * username, and password.
   * @param mechanism a SASL authentication mechanism, e.g. LOGIN, PLAIN,
   * CRAM-MD5, GSSAPI
   * @param username the authentication principal
   * @param password the authentication credentials
   * @return true if authentication was successful, false otherwise
   */
  public boolean auth(String mechanism, String username, String password)
    throws IOException
  {
    try
      {
        String[] m = new String[] { mechanism };
        CallbackHandler ch = new SaslCallbackHandler(username, password);
        // Avoid lengthy callback procedure for GNU Crypto
        HashMap p = new HashMap();
        p.put("gnu.crypto.sasl.username", username);
        p.put("gnu.crypto.sasl.password", password);
        SaslClient sasl =
          Sasl.createSaslClient(m, null, "pop3",
                                socket.getInetAddress().getHostName(),
                                p, ch);
        if (sasl == null)
          {
            // Fall back to home-grown SASL clients
            if ("LOGIN".equalsIgnoreCase(mechanism))
              {
                sasl = new SaslLogin(username, password);
              }
            else if ("PLAIN".equalsIgnoreCase(mechanism))
              {
                sasl = new SaslPlain(username, password);
              }
            else if ("CRAM-MD5".equalsIgnoreCase(mechanism))
              {
                sasl = new SaslCramMD5(username, password);
              }
            else
              {
                return false;
              }
          }
        
        StringBuffer cmd = new StringBuffer(AUTH);
        cmd.append(' ');
        cmd.append(mechanism);
        send(cmd.toString());
        while (true)
          {
            switch (getResponse())
              {
              case OK:
                String qop = (String) sasl.getNegotiatedProperty(Sasl.QOP);
                if ("auth-int".equalsIgnoreCase(qop)
                    || "auth-conf".equalsIgnoreCase(qop))
                  {
                    InputStream in = socket.getInputStream();
                    in = new BufferedInputStream(in);
                    in = new SaslInputStream(sasl, in);
                    in = new CRLFInputStream(in);
                    this.in = new LineInputStream(in);
                    OutputStream out = socket.getOutputStream();
                    out = new BufferedOutputStream(out);
                    out = new SaslOutputStream(sasl, out);
                    this.out = new CRLFOutputStream(out);
                  }
                return true;
              case READY:
                try
                  {
                    byte[] c0 = response.getBytes("US-ASCII");
                    byte[] c1 = BASE64.decode(c0);       // challenge
                    byte[] r0 = sasl.evaluateChallenge(c1);
                    byte[] r1 = BASE64.encode(r0);       // response
                    out.write(r1);
                    out.write(0x0d);
                    out.flush();
                    logger.log(POP3_TRACE, "> " +
                               new String(r1, "US-ASCII"));
                  }
                catch (SaslException e)
                  {
                    // Error in SASL challenge evaluation - cancel exchange
                    out.write(0x2a);
                    out.write(0x0d);
                    out.flush();
                    logger.log(POP3_TRACE, "> *");
                  }
              default:
                return false;
              }
          }
      }
    catch (SaslException e)
      {
        logger.log(POP3_TRACE, e.getMessage(), e);
        return false;             // No provider for mechanism
      }
    catch (RuntimeException e)
      {
        logger.log(POP3_TRACE, e.getMessage(), e);
        return false;             // No javax.security.sasl classes
      }
  }
  
  /**
   * Authenticate the specified user using the APOP MD5-based method.
   * This does not transmit the password in the clear, but doesn't provide
   * any transport-level privacy features either.
   * @param username the user to authenticate
   * @param password the user's password
   */
  public boolean apop(String username, String password)
    throws IOException
  {
    if (username == null || password == null || timestamp == null)
      {
        return false;
      }
    // APOP <username> <digest>
    try
      {
        byte[] secret = password.getBytes("US-ASCII");
        // compute digest
        byte[] target = new byte[timestamp.length + secret.length];
        System.arraycopy(timestamp, 0, target, 0, timestamp.length);
        System.arraycopy(secret, 0, target, timestamp.length, secret.length);
        MessageDigest md5 = MessageDigest.getInstance("MD5");
        byte[] db = md5.digest(target);
        // create hexadecimal representation
        StringBuffer digest = new StringBuffer();
        for (int i = 0; i < db.length; i++)
          {
            int c = (int) db[i];
            if (c < 0)
              {
                c += 256;
              }
            digest.append(Integer.toHexString((c & 0xf0) >> 4));
            digest.append(Integer.toHexString(c & 0x0f));
          }
        // send command
        String cmd = new StringBuffer(APOP)
          .append(' ')
          .append(username)
          .append(' ')
          .append(digest.toString())
          .toString();
        send(cmd);
        return getResponse() == OK;
      }
    catch (NoSuchAlgorithmException e)
      {
        logger.log(POP3_TRACE, "MD5 algorithm not found");
        return false;
      }
  }
  
  /**
   * Authenticate the user using the basic USER and PASS handshake.
   * It is recommended to use a more secure authentication method such as
   * the <code>auth</code> or <code>apop</code> method if the server
   * understands them.
   * @param username the user to authenticate
   * @param password the user's password
   */
  public boolean login(String username, String password)
    throws IOException
  {
    if (username == null || password == null)
      {
        return false;
      }
    // USER <username>
    String cmd = USER + ' ' + username;
    send(cmd);
    if (getResponse() != OK)
      {
        return false;
      }
    // PASS <password>
    cmd = PASS + ' ' + password;
    send(cmd);
    return getResponse() == OK;
  }

  /**
   * Returns a configured SSLSocketFactory to use in creating new SSL
   * sockets.
   * @param tm an optional trust manager to use
   */
  protected SSLSocketFactory getSSLSocketFactory(TrustManager tm)
    throws GeneralSecurityException
  {
    if (tm == null)
      {
        tm = new EmptyX509TrustManager();
      }
    SSLContext context = SSLContext.getInstance("TLS");
    TrustManager[] trust = new TrustManager[] { tm };
    context.init(null, trust, null);
    return context.getSocketFactory();
  }
  
  /**
   * Attempts to start TLS on the specified connection.
   * See RFC 2595 for details
   * @return true if successful, false otherwise
   */
  public boolean stls()
    throws IOException
  {
    return stls(new EmptyX509TrustManager());
  }
  
  /**
   * Attempts to start TLS on the specified connection.
   * See RFC 2595 for details
   * @param tm the custom trust manager to use
   * @return true if successful, false otherwise
   */
  public boolean stls(TrustManager tm)
    throws IOException
  {
    try
      {
        SSLSocketFactory factory = getSSLSocketFactory(tm);
        
        send(STLS);
        if (getResponse() != OK)
          {
            return false;
          }
        
        String hostname = socket.getInetAddress().getHostName();
        int port = socket.getPort();
        SSLSocket ss =
          (SSLSocket) factory.createSocket(socket, hostname, port, true);
        String[] protocols = { "TLSv1", "SSLv3" };
        ss.setEnabledProtocols(protocols);
        ss.setUseClientMode(true);
        ss.startHandshake();
        
        // set up streams
        InputStream in = ss.getInputStream();
        in = new BufferedInputStream(in);
        in = new CRLFInputStream(in);
        this.in = new LineInputStream(in);
        OutputStream out = ss.getOutputStream();
        out = new BufferedOutputStream(out);
        this.out = new CRLFOutputStream(out);
        
        return true;
      }
    catch (GeneralSecurityException e)
      {
        return false;
      }
  }
  
  /**
   * Returns the number of messages in the maildrop.
   */
  public int stat()
    throws IOException
  {
    send(STAT);
    if (getResponse() != OK)
      {
        throw new ProtocolException("STAT failed: " + response);
      }
    try
      {
        return
          Integer.parseInt(response.substring(0, response.indexOf(' ')));
      }
    catch (NumberFormatException e)
      {
        throw new ProtocolException("Not a number: " + response);
      }
    catch (ArrayIndexOutOfBoundsException e)
      {
        throw new ProtocolException("Not a STAT response: " + response);
      }
  }
  
  /**
   * Returns the size of the specified message.
   * @param msgnum the message number
   */
  public int list(int msgnum)
    throws IOException
  {
    String cmd = LIST + ' ' + msgnum;
    send(cmd);
    if (getResponse() != OK)
      {
        throw new ProtocolException("LIST failed: " + response);
      }
    try
      {
        return
          Integer.parseInt(response.substring(response.indexOf(' ') + 1));
      }
    catch (NumberFormatException e)
      {
        throw new ProtocolException("Not a number: " + response);
      }
  }
  
  /**
   * Returns an input stream containing the entire message.
   * This input stream must be read in its entirety before further commands
   * can be issued on this connection.
   * @param msgnum the message number
   */
  public InputStream retr(int msgnum)
    throws IOException
  {
    String cmd = RETR + ' ' + msgnum;
    send(cmd);
    if (getResponse() != OK)
      {
        throw new ProtocolException("RETR failed: " + response);
      }
    return new MessageInputStream(in);
  }
  
  /**
   * Marks the specified message as deleted.
   * @param msgnum the message number
   */
  public void dele(int msgnum)
    throws IOException
  {
    String cmd = DELE + ' ' + msgnum;
    send(cmd);
    if (getResponse() != OK)
      {
        throw new ProtocolException("DELE failed: " + response);
      }
  }
  
  /**
   * Does nothing.
   * This can be used to keep the connection alive.
   */
  public void noop()
    throws IOException
  {
    send(NOOP);
    if (getResponse() != OK)
      {
        throw new ProtocolException("NOOP failed: " + response);
      }
  }

  /**
   * If any messages have been marked as deleted, they are unmarked.
   */
  public void rset()
    throws IOException
  {
    send(RSET);
    if (getResponse() != OK)
      {
        throw new ProtocolException("RSET failed: " + response);
      }
  }
  
  /**
   * Closes the connection.
   * No further commands may be issued on this connection after this method
   * has been called.
   * @return true if all deleted messages were successfully removed, false
   * otherwise
   */
  public boolean quit()
    throws IOException
  {
    send(QUIT);
    int ret = getResponse();
    socket.close();
    return ret == OK;
  }

  /**
   * Returns just the headers of the specified message as an input stream.
   * The stream must be read in its entirety before further commands can be
   * issued.
   * @param msgnum the message number
   */
  public InputStream top(int msgnum)
    throws IOException
  {
    String cmd = TOP + ' ' + msgnum + ' ' + '0';
    send(cmd);
    if (getResponse() != OK)
      {
        throw new ProtocolException("TOP failed: " + response);
      }
    return new MessageInputStream(in);
  }
  
  /**
   * Returns a unique identifier for the specified message.
   * @param msgnum the message number
   */
  public String uidl(int msgnum)
    throws IOException
  {
    String cmd = UIDL + ' ' + msgnum;
    send(cmd);
    if (getResponse() != OK)
      {
        throw new ProtocolException("UIDL failed: " + response);
      }
    return response.substring(response.indexOf(' ') + 1);
  }

  /**
   * Returns a map of message number to UID pairs.
   * Message numbers are Integers, UIDs are Strings.
   */
  public Map uidl()
    throws IOException
  {
    send(UIDL);
    if (getResponse() != OK)
      {
        throw new ProtocolException("UIDL failed: " + response);
      }
    Map uids = new LinkedHashMap();
    String line = in.readLine();
    while (line != null && !(".".equals(line)))
      {
        int si = line.indexOf(' ');
        if (si < 1)
          {
            throw new ProtocolException("Invalid UIDL response: " + line);
          }
        try
          {
            uids.put(new Integer(line.substring(0, si)),
                      line.substring(si + 1));
          }
        catch (NumberFormatException e)
          {
            throw new ProtocolException("Invalid message number: " + line);
          }
      }
    return Collections.unmodifiableMap(uids);
  }

  /**
   * Returns a list of capabilities supported by the POP3 server.
   * If the server does not support POP3 extensions, returns
   * <code>null</code>.
   */
  public List capa()
    throws IOException
  {
    send(CAPA);
    if (getResponse() == OK)
      {
        final String DOT = ".";
        List list = new ArrayList();
        for (String line = in.readLine();
             !DOT.equals(line);
             line = in.readLine())
          {
            list.add(line);
          }
        return Collections.unmodifiableList(list);
      }
    return null;
  }
  
  /** 
   * Send the command to the server.
   */
  protected void send(String command)
    throws IOException
  {
    logger.log(POP3_TRACE, "> " + command);
    out.write(command);
    out.writeln();
    out.flush();
  }
  
  /**
   * Parse the response from the server.
   */
  protected int getResponse()
    throws IOException
  {
    response = in.readLine();
    logger.log(POP3_TRACE, "< " + response);
    if (response.indexOf(_OK) == 0)
      {
        response = response.substring(3).trim();
        return OK;
      }
    else if (response.indexOf(_ERR) == 0)
      {
        response = response.substring(4).trim();
        return ERR;
      }
    else if (response.indexOf(_READY) == 0)
      {
        response = response.substring(2).trim();
        return READY;
      }
    else
      {
        throw new ProtocolException("Unexpected response: " + response);
      }
  }
  
  /*
   * Parse the APOP timestamp from the server's banner greeting.
   */
  byte[] parseTimestamp(String greeting)
    throws IOException
  {
    int bra = greeting.indexOf('<');
    if (bra != -1)
      {
        int ket = greeting.indexOf('>', bra);
        if (ket != -1)
          {
            String mid = greeting.substring(bra, ket + 1);
            int at = mid.indexOf('@');
            if (at != -1)           // This is a valid RFC822 msg-id
              {
                return mid.getBytes("US-ASCII");
              }
          }
      }
    return null;
  }
  
}

