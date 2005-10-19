package gnu.crypto.tool;

// ----------------------------------------------------------------------------
// $Id: SaslConnection.java,v 1.1 2005/10/19 20:15:52 guilhem Exp $
//
// Copyright (C) 2003, 2004 Free Software Foundation, Inc.
//
// This file is part of GNU Crypto.
//
// GNU Crypto is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// GNU Crypto is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to the
//
//    Free Software Foundation Inc.,
//    51 Franklin Street, Fifth Floor,
//    Boston, MA 02110-1301
//    USA
//
// Linking this library statically or dynamically with other modules is
// making a combined work based on this library.  Thus, the terms and
// conditions of the GNU General Public License cover the whole
// combination.
//
// As a special exception, the copyright holders of this library give
// you permission to link this library with independent modules to
// produce an executable, regardless of the license terms of these
// independent modules, and to copy and distribute the resulting
// executable under terms of your choice, provided that you also meet,
// for each linked independent module, the terms and conditions of the
// license of that module.  An independent module is a module which is
// not derived from or based on this library.  If you modify this
// library, you may extend this exception to your version of the
// library, but you are not obligated to do so.  If you do not wish to
// do so, delete this exception statement from your version.
// ----------------------------------------------------------------------------

import gnu.crypto.Registry;
import gnu.crypto.auth.Password;
import gnu.crypto.sasl.ClientMechanism;
import gnu.crypto.sasl.ClientFactory;
import gnu.crypto.sasl.SaslInputStream;
import gnu.crypto.sasl.SaslOutputStream;
import gnu.crypto.sasl.srp.SRPRegistry;

import java.io.EOFException;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.Socket;
import java.net.URL;
import java.util.HashMap;
import java.util.Map;

import javax.security.auth.callback.CallbackHandler;
import javax.security.sasl.SaslException;

/**
 * A sample client-side protocol driver.
 *
 * @version $Revision: 1.1 $
 */
public class SaslConnection
{
   // Constants and variables
   // --------------------------------------------------------------------------

   private boolean connected;
   private String id;
   private String protocol;
   private String serverName;
   private int port;
   private CallbackHandler cbh;
   private Map properties;
   private String mechanism;
   private transient Socket socket;

   private ClientMechanism sasl;

   private InputStream in;
   private OutputStream out;
   private InputStream secureIn;
   private OutputStream secureOut;

   // Constructor(s)
   // --------------------------------------------------------------------------

   public SaslConnection(String m, URL url) throws SaslException, IOException {
      super();

      mechanism = m;
      String userInfo = url.getUserInfo();
      properties = new HashMap();
      int ndx = userInfo.indexOf(':');
      if (ndx == -1) {
         id = userInfo;
      } else {
         id = userInfo.substring(0, ndx);
         properties.put(Registry.SASL_PASSWORD,
                        new Password(userInfo.substring(ndx+1).toCharArray()));
      }

      protocol = url.getProtocol();
      serverName = url.getHost();
      port = url.getPort();
      cbh = new SimpleCallbackHandler();

      properties.put(Registry.SASL_AUTHORISATION_ID, id);
      properties.put(Registry.SASL_PROTOCOL, protocol);
      properties.put(Registry.SASL_SERVER_NAME, serverName);
      properties.put(Registry.SASL_CALLBACK_HANDLER, cbh);

      properties.put(Registry.SASL_USERNAME, id);
      properties.put(SRPRegistry.SRP_REPLAY_DETECTION, "true");
      properties.put(SRPRegistry.SRP_INTEGRITY_PROTECTION, "true");
      properties.put(SRPRegistry.SRP_CONFIDENTIALITY, "true");

      disconnect();
   }

   // Class methods
   // -------------------------------------------------------------------------

   private static final byte[] mungeSaslBuffer(InputStream in) throws IOException {
      byte[] header = new byte[4];
      int check = in.read(header);
      if (check == -1) {
         throw new EOFException();
      }
      if (check != 4) {
         throw new IOException();
      }
      int length = (header[0] & 0xFF) << 24 |
                   (header[1] & 0xFF) << 16 |
                   (header[2] & 0xFF) <<  8 |
                   (header[3] & 0xFF);
      System.out.println("[SaslConnection] Expecting "+String.valueOf(length)
            +" byte(s) from the stream...");
      byte[] result = new byte[length + 4];
      System.arraycopy(header, 0, result, 0, 4);
      check = in.read(result, 4, length);
      if (check == -1) {
         throw new EOFException();
      }
      if (check != length) {
         throw new IOException();
      }
      return result;
   }

   // Instance methods
   // -------------------------------------------------------------------------

   /**
    * Returns <code>true</code> if the communications link with the end-point
    * has been established; <code>false</code> otherwise.
    *
    * @return <code>true</code> or <code>false</code> depending on wether the
    * communications link with the end-point has been established or not.
    */
   public boolean isConnected() {
      return connected;
   }

   public void connect() throws IOException {
      if (connected) {
         return;
      }
      socket = new Socket(serverName, port);
      in = socket.getInputStream();
      out = socket.getOutputStream();

      // authentication layer ----------------------------------------------

      out.write(mechanism.getBytes("ASCII"));
      out.write(0x00);

      byte[] challenge, response;
      if (sasl.hasInitialResponse()) {
         response = sasl.evaluateChallenge(null);
         out.write(response);
      }
      while (!sasl.isComplete()) {
         challenge = mungeSaslBuffer(in);
         response = sasl.evaluateChallenge(challenge);
         if (response != null) {
            out.write(response);
         } else if (!sasl.isComplete()) {
            // is null. only valid if client authentication is complete
            throw new RuntimeException("Response null but client incomplete");
         } else {
            break;
         }
      }

      System.out.println("[SaslConnection] Connected!");

      secureIn =  new SaslInputStream( sasl, in );
      secureOut = new SaslOutputStream(sasl, out);

      connected = true;
   }

   /**
    * Returns an input stream that reads from this open connection.
    *
    * @return an input stream that reads from this open connection.
    * @exception IOException if an I/O error occurs while creating the input
    * stream.
    * @exception IllegalStateException if this method was invoked before the
    * connection was opened; ie. the authentication phase has not yet occured.
    */
   public InputStream getInputStream() throws IOException {
      if (!connected) {
         throw new IllegalStateException();
      }
      return secureIn;
   }

   /**
    * Returns an output stream that writes to this connection.
    *
    * @return an output stream that writes to this connection.
    * @exception IOException if an I/O error occurs while creating the output
    * stream.
    * @exception IllegalStateException if this method was invoked before the
    * connection was opened; ie. the authentication phase has not yet occured.
    */
   public OutputStream getOutputStream() throws IOException {
      if (!connected) {
         throw new IllegalStateException();
      }
      return secureOut;
   }

   public void send(byte[] message) throws IOException {
      System.out.println("[SaslConnection] Outgoing message (str): "+new String(message));
      secureOut.write(message);
   }

   public byte[] receive() throws IOException {
      int first = secureIn.read();
      int limit = secureIn.available();
      byte[] result = new byte[limit+1];
      result[0] = (byte) first;
      secureIn.read(result, 1, limit);

      System.out.println("[SaslConnection] Incoming response (str): "+new String(result));
      return result;
   }

   /**
    * Sets the <i>connected</i> field to false and instantiates a new
    * underlying mechanism client object.
    */
   public void disconnect() throws IOException {
      System.out.println("[SaslConnection] ==> disconnect()");
      connected = false;
      if (sasl != null) {
         sasl.reset();
      }
      sasl = ClientFactory.getInstance(mechanism);
      if (sasl == null) {
         throw new RuntimeException("Unable to create SASL client");
      }
//      mechanism = sasl.getMechanismName();
      System.out.println("[SaslConnection] Chosen mechanism: "+mechanism);

      sasl.init(properties);

      System.out.println("[SaslConnection] <== disconnect()");
   }

   public void reconnect() throws IOException {
      disconnect();
      connect();
   }
}
