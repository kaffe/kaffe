package gnu.crypto.tool;

// ----------------------------------------------------------------------------
// $Id: SaslS.java,v 1.1 2005/10/19 20:15:52 guilhem Exp $
//
// Copyright (C) 2003 Free Software Foundation, Inc.
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

import gnu.crypto.sasl.SaslInputStream;
import gnu.crypto.sasl.SaslOutputStream;
import gnu.crypto.sasl.ServerFactory;
import gnu.crypto.sasl.ServerMechanism;
import gnu.crypto.sasl.srp.PasswordFile;
import gnu.crypto.sasl.srp.SRPRegistry;

import java.io.EOFException;
import java.io.File;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.ServerSocket;
import java.net.Socket;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Random;

import javax.security.sasl.SaslException;

/**
 * <p>A basic server-side test class to exercise SASL mechanisms that exchange
 * their response/challenge in SASL Buffers. At the time of testing this code,
 * only two such mechanisms are included in the library: SRP and SM2!</p>
 *
 * <p>The <code>main()</code> method accepts two arguments: the first is the
 * number in milliseconds for the read timeout operation, and the second is the
 * port number on which to listen for incoming connections. Use <code>Ctrl-C</code>
 * to kill it. It should activate a shutdown thread that will orderly close all
 * live worker threads before exiting.</p>
 *
 * @version $Revision: 1.1 $
 */
public class SaslS implements Runnable {

   // Constants and variables
   // -------------------------------------------------------------------------

   static final HashMap pool = new HashMap();

   private static int port;
   private static int timeout;
   private static long counter = 0L;
   private static String user = "test";
   private static String password = "test";

   private ServerSocket server;

   // Constructor(s)
   // -------------------------------------------------------------------------

   SaslS(ServerSocket ss) {
      super();

      server = ss;
      listen();
   }

   // Class methods
   // -------------------------------------------------------------------------

   /**
    * A simple test server that handles SASL connections.
    *
    * @param args [0] timeout, in millis, for client connections;
    * [1] port number where to listen for incoming connections.
    */
   public static final void main(String[] args) throws IOException {
      System.out.println("SASL Test Server");
      if (args.length != 2) {
        System.out.println("Usage: "+SaslS.class.getName()+" <timeout in millis> <port>");
        System.exit(0);
      }

      System.out.println("1. Updating 'test' user password info");
      System.out.println("Checking existence of password file. Creating it if new...");
      String pFile = System.getProperty(SRPRegistry.PASSWORD_FILE, "./test");
      System.out.println("SRP password file: \""+pFile+"\"");
      File f = new File(pFile);
      if (!f.exists()) {
         System.out.println("Creating new file \""+f.getCanonicalPath()+"\"");
         if (f.createNewFile()) {
            f.deleteOnExit();
         }
      } else if (!f.isFile()) {
         throw new RuntimeException("File object ("+pFile+") exists but is not a file");
      } else if (!f.canRead() || !f.canWrite()) {
         throw new RuntimeException("File ("+pFile+") exists but is not accessible");
      }
      f = null;

      PasswordFile tpasswd = new PasswordFile(pFile);
      if (!tpasswd.contains(user)) {
         System.out.println("Adding test user...");
         byte[] testSalt = new byte[10];
         new Random().nextBytes(testSalt);
         tpasswd.add(user, password, testSalt, "1");
         System.out.println("Added test user...");
      } else {
         System.out.println("Updating test user...");
         tpasswd.changePasswd(user, password);
         System.out.println("Updated test user...");
      }

      tpasswd.savePasswd();
      tpasswd = null;

      timeout = Integer.valueOf(args[0]).intValue();
      port = Integer.valueOf(args[1]).intValue();
      final ServerSocket ss = new ServerSocket(port);
      new SaslS(ss);

      System.out.println("Server ready...");

      Runtime.getRuntime().addShutdownHook(
         new Thread() {
            public void run() {
               System.out.println("Started shutdown...");
               try {
                  System.out.println("Closing server socket...");
                  ss.close();
                  System.out.println("Closed server socket...");
               } catch (IOException x) {
                  x.printStackTrace(System.err);
                  System.out.println("Exception ("+String.valueOf(x)
                        +") while shutting down. Ignored...");
               }

               System.out.println("Interrupting "+String.valueOf(pool.size())
                     +" waiting connection thread(s)...");
               Iterator it = pool.entrySet().iterator();
               while (it.hasNext()) {
                  Thread t = (Thread)((Map.Entry) it.next()).getValue();
                  String name = t.getName();
                  System.out.println("Interrupting connection "+name+"...");
                  try {
                     t.interrupt();
                     t.join();
                  } catch (Exception x) {
                     x.printStackTrace(System.err);
                     System.out.println("Exception ("+String.valueOf(x)
                           +") while interrupting/joining "+name+". Ignored...");
                  }

                  System.out.println("Interrupted connection "+name+"...");
               }
               System.out.println("Completed shutdown. Exiting...");
            }
         }
      );
   }

   private static final byte[] mungeSaslBuffer(InputStream in) throws IOException {
      byte[] h = new byte[4];
      int check = in.read(h);
      if (check == -1) {
         throw new EOFException();
      }
      if (check != 4) {
         throw new IOException();
      }
      int length = h[0] << 24 | (h[1] & 0xFF) << 16 | (h[2] & 0xFF) << 8 | (h[3] & 0xFF);
      byte[] result = new byte[length + 4];
      System.arraycopy(h, 0, result, 0, 4);
      check = in.read(result, 4, length);
      if (check == -1) {
         throw new EOFException();
      } else if (check != length) {
         throw new IOException();
      }
      return result;
   }

   // Instance methods
   // -------------------------------------------------------------------------

   // Runnable interface methods implementation -------------------------------

   public void run() {
      Socket socket;
      try {
         socket = server.accept();
      } catch (IOException x) {
         x.printStackTrace(System.err);
         System.out.println("run(). Server shutdown...");
         return;
      }

      listen();

      InputStream in, secureIn;
      OutputStream out, secureOut;
      ServerMechanism sasl = null;
      try {
         socket.setSoTimeout(timeout);
         socket.setTcpNoDelay(true);

         in = socket.getInputStream();
         out = socket.getOutputStream();
         int c;
         StringBuffer sb = new StringBuffer();
         while ((c = in.read()) != 0x00) {
            sb.append((char) c);
         }
         String mechanism = sb.toString();
         System.out.println("Requested authentication mechanism: "+mechanism);

         HashMap properties = new HashMap();
         properties.put(SRPRegistry.SRP_REPLAY_DETECTION, "true");
         properties.put(SRPRegistry.SRP_INTEGRITY_PROTECTION, "true");
         properties.put(SRPRegistry.SRP_CONFIDENTIALITY, "true");
         properties.put(SRPRegistry.PASSWORD_FILE,
               System.getProperty(SRPRegistry.PASSWORD_FILE, "./test"));

         sasl = ServerFactory.getInstance(mechanism);
         if (sasl == null) {
            throw new RuntimeException("Unable to find "+mechanism+" SASL mechanism");
         }

         sasl.init(properties);
         byte[] response, challenge, rpcCall, retValue;
         do {
            response = mungeSaslBuffer(in);
            challenge = sasl.evaluateResponse(response);
            if (challenge != null) {
               out.write(challenge);
            } else if (!sasl.isComplete()) {
               // is null. only valid if server authentication is complete
               throw new RuntimeException("Challenge null but server incomplete");
            } else {
               break;
            }
         } while (!sasl.isComplete());

         System.out.println("************** Connection authenticated....");
         secureIn =  new SaslInputStream( sasl, in );
         secureOut = new SaslOutputStream(sasl, out);

         rpcCall = receive(secureIn);
         retValue = call(rpcCall);
         send(retValue, secureOut);

      } catch (Exception x) {
         x.printStackTrace(System.err);
         System.out.println("Exception while processing connection");
      } finally {
         System.out.println("Connection shutdown...");
         try {
            if (sasl != null) {
               try {
                  sasl.reset();
               } catch (SaslException ignored) {
                  System.out.println("Exception ("+ignored.getMessage()
                        +") while resetting the SASL mechanism. Ignored...");
               }
            }
            socket.close();
         } catch (IOException ignored) {
            System.out.println("Exception ("+ignored.getMessage()
                  +") while shutting down the connection. Ignored...");
         }

         pool.remove(Thread.currentThread().getName());
      }
   }

   // other Instance methods --------------------------------------------------

   void listen() {
      String name = "CONN-"+String.valueOf(++counter);
      Thread t = new Thread(this, name);
      pool.put(name, t);
      t.start();
   }

   private void send(byte[] message, OutputStream out) throws IOException {
      System.out.println("Outgoing message (str): "+new String(message));
      out.write(message);
   }

   private byte[] receive(InputStream in) throws IOException {
      int first = in.read();
      int limit = in.available();
      byte[] result = new byte[limit+1];
      result[0] = (byte) first;
      in.read(result, 1, limit);

      return result;
   }

   private byte[] call(byte[] request) {
      System.out.println("Incoming request (str): "+new String(request));
      return "ok".getBytes();
   }
}
