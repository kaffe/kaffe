/*
 * SMTPConnection.java
 * Copyright (C) 2003 Chris Burdess <dog@gnu.org>
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

package gnu.inet.smtp;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.InetSocketAddress;
import java.net.ProtocolException;
import java.net.Socket;
import java.security.GeneralSecurityException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.HashMap;
import java.util.List;
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
import gnu.inet.util.MessageOutputStream;
import gnu.inet.util.SaslCallbackHandler;
import gnu.inet.util.SaslCramMD5;
import gnu.inet.util.SaslInputStream;
import gnu.inet.util.SaslLogin;
import gnu.inet.util.SaslOutputStream;
import gnu.inet.util.SaslPlain;
import gnu.inet.util.TraceLevel;

/**
 * An SMTP client.
 * This implements RFC 2821.
 *
 * @author <a href="mailto:dog@gnu.org">Chris Burdess</a>
 */
public class SMTPConnection
{

  /**
   * The logger used for SMTP protocol traces.
   */
  public static final Logger logger = Logger.getLogger("gnu.inet.smtp");

  /**
   * The network trace level.
   */
  public static final Level SMTP_TRACE = new TraceLevel("smtp");

  /**
   * The default SMTP port.
   */
  public static final int DEFAULT_PORT = 25;

  protected static final String MAIL_FROM = "MAIL FROM:";
  protected static final String RCPT_TO = "RCPT TO:";
  protected static final String SP = " ";
  protected static final String DATA = "DATA";
  protected static final String FINISH_DATA = "\n.";
  protected static final String RSET = "RSET";
  protected static final String VRFY = "VRFY";
  protected static final String EXPN = "EXPN";
  protected static final String HELP = "HELP";
  protected static final String NOOP = "NOOP";
  protected static final String QUIT = "QUIT";
  protected static final String HELO = "HELO";
  protected static final String EHLO = "EHLO";
  protected static final String AUTH = "AUTH";
  protected static final String STARTTLS = "STARTTLS";

  protected static final int INFO = 214;
  protected static final int READY = 220;
  protected static final int OK = 250;
  protected static final int OK_NOT_LOCAL = 251;
  protected static final int OK_UNVERIFIED = 252;
  protected static final int SEND_DATA = 354;
  protected static final int AMBIGUOUS = 553;

  /**
   * The underlying socket used for communicating with the server.
   */
  protected Socket socket;

  /**
   * The input stream used to read responses from the server.
   */
  protected LineInputStream in;

  /**
   * The output stream used to send commands to the server.
   */
  protected CRLFOutputStream out;

  /**
   * The last response message received from the server.
   */
  protected String response;

  /**
   * If true, there are more responses to read.
   */
  protected boolean continuation;

  /**
   * The greeting message given by the server.
   */
  protected final String greeting;

  /**
   * Creates a new connection to the specified host, using the default SMTP
   * port.
   * @param host the server hostname
   */
  public SMTPConnection(String host)
    throws IOException
  {
    this(host, DEFAULT_PORT, 0, 0, false, null);
  }

  /**
   * Creates a new connection to the specified host, using the specified
   * port.
   * @param host the server hostname
   * @param port the port to connect to
   */
  public SMTPConnection(String host, int port)
    throws IOException
  {
    this(host, port, 0, 0, false, null);
  }

  /**
   * Creates a new connection to the specified host, using the specified
   * port.
   * @param host the server hostname
   * @param port the port to connect to
   * @param connectionTimeout the connection timeout in milliseconds
   * @param timeout the I/O timeout in milliseconds
   */
  public SMTPConnection(String host, int port,
                        int connectionTimeout, int timeout)
    throws IOException
  {
    this(host, port, connectionTimeout, timeout, false, null);
  }
  
  /**
   * Creates a new connection to the specified host, using the specified
   * port.
   * @param host the server hostname
   * @param port the port to connect to
   * @param connectionTimeout the connection timeout in milliseconds
   * @param timeout the I/O timeout in milliseconds
   * @param secure true to create an SMTPS connection
   * @param tm a trust manager used to check SSL certificates, or null to
   * use the default
   */
  public SMTPConnection(String host, int port,
                        int connectionTimeout, int timeout,
                        boolean secure, TrustManager tm)
    throws IOException
  {
    if (port <= 0)
      {
        port = DEFAULT_PORT;
      }
    response = null;
    continuation = false;
    
    // Initialise socket
    try
      {
        socket = new Socket();
        InetSocketAddress address = new InetSocketAddress(host, port);
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
              (SSLSocket) factory.createSocket(socket, host, port, true);
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
    
    // Initialise streams
    InputStream in = socket.getInputStream();
    in = new BufferedInputStream(in);
    in = new CRLFInputStream(in);
    this.in = new LineInputStream(in);
    OutputStream out = socket.getOutputStream();
    out = new BufferedOutputStream(out);
    this.out = new CRLFOutputStream(out);
    
    // Greeting
    StringBuffer greetingBuffer = new StringBuffer();
    boolean notFirst = false;
    do
      {
        if (getResponse() != READY)
          {
            throw new ProtocolException(response);
          }
        if (notFirst)
          {
            greetingBuffer.append('\n');
          }
        else
          {
            notFirst = true;
          }
        greetingBuffer.append(response);
        
      }
    while (continuation);
    greeting = greetingBuffer.toString();
  }
  
  /**
   * Returns the server greeting message.
   */
  public String getGreeting()
  {
    return greeting;
  }

  /**
   * Returns the text of the last response received from the server.
   */
  public String getLastResponse()
  {
    return response;
  }

  // -- 3.3 Mail transactions --

  /**
   * Execute a MAIL command.
   * @param reversePath the source mailbox(from address)
   * @param parameters optional ESMTP parameters
   * @return true if accepted, false otherwise
   */
  public boolean mailFrom(String reversePath, ParameterList parameters)
    throws IOException
  {
    StringBuffer command = new StringBuffer(MAIL_FROM);
    command.append('<');
    command.append(reversePath);
    command.append('>');
    if (parameters != null)
      {
        command.append(SP);
        command.append(parameters);
      }
    send(command.toString());
    switch (getAllResponses())
      {
      case OK:
      case OK_NOT_LOCAL:
      case OK_UNVERIFIED:
        return true;
      default:
        return false;
      }
  }

  /**
   * Execute a RCPT command.
   * @param forwardPath the forward-path(recipient address)
   * @param parameters optional ESMTP parameters
   * @return true if successful, false otherwise
   */
  public boolean rcptTo(String forwardPath, ParameterList parameters)
    throws IOException
  {
    StringBuffer command = new StringBuffer(RCPT_TO);
    command.append('<');
    command.append(forwardPath);
    command.append('>');
    if (parameters != null)
      {
        command.append(SP);
        command.append(parameters);
      }
    send(command.toString());
    switch (getAllResponses())
      {
      case OK:
      case OK_NOT_LOCAL:
      case OK_UNVERIFIED:
        return true;
      default:
        return false;
      }
  }

  /**
   * Requests an output stream to write message data to.
   * When the entire message has been written to the stream, the
   * <code>flush</code> method must be called on the stream. Until then no
   * further methods should be called on the connection.
   * Immediately after this procedure is complete, <code>finishData</code>
   * must be called to complete the transfer and determine its success.
   * @return a stream for writing messages to
   */
  public OutputStream data()
    throws IOException
  {
    send(DATA);
    switch (getAllResponses())
      {
      case SEND_DATA:
        return new MessageOutputStream(out);
      default:
        throw new ProtocolException(response);
      }
  }

  /**
   * Completes the DATA procedure.
   * @see #data
   * @return true id transfer was successful, false otherwise
   */
  public boolean finishData()
    throws IOException
  {
    send(FINISH_DATA);
    switch (getAllResponses())
      {
      case OK:
        return true;
      default:
        return false;
      }
  }

  /**
   * Aborts the current mail transaction.
   */
  public void rset()
    throws IOException
  {
    send(RSET);
    if (getAllResponses() != OK)
      {
        throw new ProtocolException(response);
      }
  }

  // -- 3.5 Commands for Debugging Addresses --

  /**
   * Returns a list of valid possibilities for the specified address, or
   * null on failure.
   * @param address a mailbox, or real name and mailbox
   */
  public List vrfy(String address)
    throws IOException
  {
    String command = VRFY + ' ' + address;
    send(command);
    List list = new ArrayList();
    do
      {
        switch (getResponse())
          {
          case OK:
          case AMBIGUOUS:
            response = response.trim();
            if (response.indexOf('@') != -1)
              {
                list.add(response);
              }
            else if (response.indexOf('<') != -1)
              {
                list.add(response);
              }
            else if (response.indexOf(' ') == -1)
              {
                list.add(response);
              }
            break;
          default:
            return null;
          }
      }
    while (continuation);
    return Collections.unmodifiableList(list);
  }

  /**
   * Returns a list of valid possibilities for the specified mailing list,
   * or null on failure.
   * @param address a mailing list name
   */
  public List expn(String address)
    throws IOException
  {
    String command = EXPN + ' ' + address;
    send(command);
    List list = new ArrayList();
    do
      {
        switch (getResponse())
          {
          case OK:
            response = response.trim();
            list.add(response);
            break;
          default:
            return null;
          }
      }
    while (continuation);
    return Collections.unmodifiableList(list);
  }

  /**
   * Returns some useful information about the specified parameter.
   * Typically this is a command.
   * @param arg the context of the query, or null for general information
   * @return a list of possibly useful information, or null if the command
   * failed.
   */
  public List help(String arg)
    throws IOException
  {
    String command = (arg == null) ? HELP :
      HELP + ' ' + arg;
    send(command);
    List list = new ArrayList();
    do
      {
        switch (getResponse())
          {
          case INFO:
            list.add(response);
            break;
          default:
            return null;
          }
      }
    while (continuation);
    return Collections.unmodifiableList(list);
  }

  /**
   * Issues a NOOP command.
   * This does nothing, but can be used to keep the connection alive.
   */
  public void noop()
    throws IOException
  {
    send(NOOP);
    getAllResponses();
  }

  /**
   * Close the connection to the server.
   */
  public void quit()
    throws IOException
  {
    try
      {
        send(QUIT);
        getAllResponses();
        /* RFC 2821 states that the server MUST send an OK reply here, but
         * many don't: postfix, for instance, sends 221.
         * In any case we have done our best. */
      }
    catch (IOException e)
      {
      }
    finally
      {
        // Close the socket anyway.
        socket.close();
      }
  }

  /**
   * Issues a HELO command.
   * @param hostname the local host name
   */
  public boolean helo(String hostname)
    throws IOException
  {
    String command = HELO + ' ' + hostname;
    send(command);
    return (getAllResponses() == OK);
  }

  /**
   * Issues an EHLO command.
   * If successful, returns a list of the SMTP extensions supported by the
   * server.
   * Otherwise returns null, and HELO should be called.
   * @param hostname the local host name
   */
  public List ehlo(String hostname)
    throws IOException
  {
    String command = EHLO + ' ' + hostname;
    send(command);
    List extensions = new ArrayList();
    do
      {
        switch (getResponse())
          {
          case OK:
            extensions.add(response);
            break;
          default:
            return null;
          }
      }
    while (continuation);
    return Collections.unmodifiableList(extensions);
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
   * Negotiate TLS over the current connection.
   * This depends on many features, such as the JSSE classes being in the
   * classpath. Returns true if successful, false otherwise.
   */
  public boolean starttls()
    throws IOException
  {
    return starttls(new EmptyX509TrustManager());
  }
  
  /**
   * Negotiate TLS over the current connection.
   * This depends on many features, such as the JSSE classes being in the
   * classpath. Returns true if successful, false otherwise.
   * @param tm the custom trust manager to use
   */
  public boolean starttls(TrustManager tm)
    throws IOException
  {
    try
      {
        SSLSocketFactory factory = getSSLSocketFactory(tm);
        
        send(STARTTLS);
        if (getAllResponses() != READY)
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
        
        // Set up streams
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

  // -- Authentication --

  /**
   * Authenticates the connection using the specified SASL mechanism,
   * username, and password.
   * @param mechanism a SASL authentication mechanism, e.g. LOGIN, PLAIN,
   * CRAM-MD5, GSSAPI
   * @param username the authentication principal
   * @param password the authentication credentials
   * @return true if authentication was successful, false otherwise
   */
  public boolean authenticate(String mechanism, String username,
                              String password) throws IOException
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
          Sasl.createSaslClient(m, null, "smtp",
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
        if (sasl.hasInitialResponse())
          {
            cmd.append(' ');
            byte[] init = sasl.evaluateChallenge(new byte[0]);
            if (init.length == 0)
              {
                cmd.append('=');
              }
            else
              {
                cmd.append(new String(BASE64.encode(init), "US-ASCII"));
              }
          }
        send(cmd.toString());
        while (true)
          {
            switch (getAllResponses())
              {
              case 334:
                try
                  {
                    byte[] c0 = response.getBytes("US-ASCII");
                    byte[] c1 = BASE64.decode(c0);       // challenge
                    byte[] r0 = sasl.evaluateChallenge(c1);
                    byte[] r1 = BASE64.encode(r0);       // response
                    out.write(r1);
                    out.write(0x0d);
                    out.flush();
                    logger.log(SMTP_TRACE, "> " +
                                    new String(r1, "US-ASCII"));
                  }
                catch (SaslException e)
                  {
                    // Error in SASL challenge evaluation - cancel exchange
                    out.write(0x2a);
                    out.write(0x0d);
                    out.flush();
                    logger.log(SMTP_TRACE, "> *");
                  }
                break;
              case 235:
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
              default:
                return false;
              }
          }
      }
    catch (SaslException e)
      {
        logger.log(SMTP_TRACE, e.getMessage(), e);
        return false;             // No provider for mechanism
      }
    catch (RuntimeException e)
      {
        logger.log(SMTP_TRACE, e.getMessage(), e);
        return false;             // No javax.security.sasl classes
      }
  }

  // -- Utility methods --

  /**
   * Send the specified command string to the server.
   * @param command the command to send
   */
  protected void send(String command)
    throws IOException
  {
    logger.log(SMTP_TRACE, "> " + command);
    out.write(command.getBytes("US-ASCII"));
    out.write(0x0d);
    out.flush();
  }
  
  /**
   * Returns the next response from the server.
   */
  protected int getResponse()
    throws IOException
  {
    String line = null;
    try
      {
        line = in.readLine();
        // Handle special case eg 334 where CRLF occurs after code.
        if (line.length() < 4)
          {
            line = line + '\n' + in.readLine();
          }
        logger.log(SMTP_TRACE, "< " + line);
        int code = Integer.parseInt(line.substring(0, 3));
        continuation = (line.charAt(3) == '-');
        response = line.substring(4);
        return code;
      }
    catch (NumberFormatException e)
      {
        throw new ProtocolException("Unexpected response: " + line);
      }
  }

  /**
   * Returns the next response from the server.
   * If the response is a continuation, continues to read responses until
   * continuation ceases. If a different response code from the first is
   * encountered, this causes a protocol exception.
   */
  protected int getAllResponses()
    throws IOException
  {
    int code1, code;
    boolean err = false;
    code1 = code = getResponse();
    while (continuation)
      {
        code = getResponse();
        if (code != code1)
          {
            err = true;
          }
      }
    if (err)
      {
        throw new ProtocolException("Conflicting response codes");
      }
    return code;
  }

}

