package gnu.crypto.tool;

// ----------------------------------------------------------------------------
// $Id: SMTPClient.java,v 1.1 2005/10/19 20:15:52 guilhem Exp $
//
// Copyright (C) 2003, Free Software Foundation, Inc.
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
import gnu.crypto.jce.GnuCrypto;
import gnu.crypto.sasl.srp.SRPRegistry;
import gnu.crypto.util.Base64;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.net.Socket;
import java.security.Security;
import java.util.HashMap;

import javax.security.sasl.Sasl;
import javax.security.sasl.SaslClient;
import javax.security.sasl.SaslException;
import javax.security.auth.callback.CallbackHandler;

/**
 * @version $Revision: 1.1 $
 */
public class SMTPClient {

   // Constants and variables
   // -------------------------------------------------------------------------

   private String mechanism;
   private String host;
   private int port;

   // Constructor(s)
   // -------------------------------------------------------------------------

   public SMTPClient(String mechanism, String host, int port) {
      super();

      this.mechanism = mechanism;
      this.host = host;
      this.port = port;
   }

   // Class methods
   // -------------------------------------------------------------------------

   public static void main(String[] args) throws Exception {
      if (args.length != 3) {
         System.out.println("Usage: "+SMTPClient.class.getName()+" <mechanism> <host> <port>");
      } else {
         SMTPClient smtp = new SMTPClient(args[0], args[1], Integer.parseInt(args[2]));
         smtp.work();
      }
   }

   // Instance methods
   // -------------------------------------------------------------------------

   public void work() throws SaslException, IOException, InterruptedException {

      Security.addProvider(new GnuCrypto());

      boolean useInitialResponse = true;
      String username = "test";
      String id = "test";
      String password = "test";
      String service = "SMTP";
      HashMap properties = new HashMap(6);
      CallbackHandler cbh = new SimpleCallbackHandler();

      properties.put(Registry.SASL_USERNAME, username);
      properties.put(Registry.SASL_PASSWORD, password);
      properties.put(SRPRegistry.SRP_REPLAY_DETECTION, "true");
      properties.put(SRPRegistry.SRP_INTEGRITY_PROTECTION, "true");
      properties.put(SRPRegistry.SRP_CONFIDENTIALITY, "true");

      SaslClient client = Sasl.createSaslClient(
            new String[] { mechanism }, id, service, host, properties, cbh);
      if (client == null) {
         throw new RuntimeException("Unable to create SASL client");
      }

      Socket socket = new Socket(host, port);
      InputStreamReader in = new InputStreamReader(socket.getInputStream());
      BufferedReader brin = new BufferedReader(in, 1024);
      PrintWriter out = new PrintWriter(socket.getOutputStream(), true);

      byte[] challenge;
      byte[] response;

      String greeting = brin.readLine();
      System.out.println("S: "+greeting);

      System.out.println("C: EHLO ook");
      out.println("EHLO ook");

      greeting = brin.readLine();
      System.out.println("S: "+greeting);

      System.out.print("C: AUTH "+mechanism);
      out.print("AUTH "+mechanism);
      if (useInitialResponse && client.hasInitialResponse()) {
         response = client.evaluateChallenge(null);
         System.out.print(" "+Base64.encode(response));
         out.print(" "+Base64.encode(response));
      }
      System.out.println();
      out.println();

      String input, code;
      while (!client.isComplete()) {
         try {
            input = brin.readLine();
            System.out.println("S: "+input);
            if (input != null) {
               code = input.substring(0,3);
               if (code.startsWith("5")) {
                  System.exit(1);
               }
               if (input.startsWith("3")) {
                  input = input.substring(4);
                  challenge = null;
                  if (input.length() > 0) {
                     challenge = Base64.decode(input);
                  }
                  response = client.evaluateChallenge(challenge);
                  if (response != null) {
                     String output = Base64.encode(response);
                     System.out.println("C: "+output);
                     out.println(output);
                  } else {
                     System.out.println("C: ");
                     out.println();
                  }
               }
            }
         } catch (IOException x) {
            x.printStackTrace(System.err);
            System.exit(1);
         }
      }

      input = brin.readLine();
      System.out.println("S: "+input);
      if (!input.startsWith("235")) {
         System.out.println("C: ");
         out.println();
         input = brin.readLine();
         System.out.println("S: "+input);
      }

      System.out.println("complete");
      for (int i = 0; i < 3; i++) {
         System.out.println("C: NOOP");
         out.println("NOOP");
         input = brin.readLine();
         System.out.println("S: "+input);

         Thread.sleep(2000);
      }

      System.out.println("C: QUIT");
      out.println("QUIT");
      input = brin.readLine();
      System.out.println("S: "+input);

      Thread.sleep(2000);

      out.flush();
      brin.close();
      out.close();
      socket.close();
   }
}
