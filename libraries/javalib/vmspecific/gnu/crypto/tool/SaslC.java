package gnu.crypto.tool;

// ----------------------------------------------------------------------------
// $Id: SaslC.java,v 1.1 2005/10/19 20:15:52 guilhem Exp $
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

import java.net.URL;
import java.util.ArrayList;
import java.util.ListIterator;

/**
 * <p>A basic client-side test class to exercise SASL mechanisms.</p>
 *
 * <p>The <code>main()</code> method accepts two arguments: the first is the
 * number of threads, and the second is the port number on which the (local)
 * server is listening.</p>
 *
 * @version $Revision: 1.1 $
 */
public class SaslC implements Runnable {

   // Constants and variables
   // -------------------------------------------------------------------------

   String port;
   String mechanism;

   // Constructor(s)
   // -------------------------------------------------------------------------

   private SaslC(String mechanism, String port) {
      super();

      this.mechanism = mechanism;
      this.port = port;
   }

   // Class methods
   // -------------------------------------------------------------------------

   /**
    * Simple test client that connects to the test server over SASL connections.
    *
    * @param args [0] the name of the SASL mechanism to exercise;
    * [1] the number of working threads to exercise simultaneously; and
    * [2] port number where to connect to the server.
    */
   public static void main(String[] args) throws Exception {
      System.out.println("SASL Test Client");
      if (args.length != 3)
      {
         System.out.print("Usage: "+SaslC.class.getName()+" ");
         System.out.print("<SASL mechanism> ");
         System.out.print("<number of simultaneous threads> ");
         System.out.println("<server port>");
         System.exit(0);
      }

      String mechanism = args[0];
      int limit = Integer.parseInt(args[1]);
      String port = args[2];

      System.out.println("Starting...");
      ArrayList threads = new ArrayList();
      ListIterator it = threads.listIterator();
      while (limit-- > 0) {
         it.add(new Thread(new SaslC(mechanism, port), "C"+String.valueOf(limit)));
      }

      it = threads.listIterator();
      while (it.hasNext()) {
         ((Thread) it.next()).start();
      }

      boolean loop = true;
      do {
         try {
            Thread.sleep(3000);
         } catch (InterruptedException ignored) {
         }

         it = threads.listIterator();
         while (it.hasNext()) {
            loop &= ((Thread) it.next()).isAlive();
         }
      } while (loop);

      System.out.println("Exiting...");
   }

   // Instance methods
   // -------------------------------------------------------------------------

   public void run() {
      String name = Thread.currentThread().getName();
      System.out.println("Client "+name+" starting...");
      try {
         URL url = new URL("http://test:test@localhost:"+port+"/");
         SaslConnection channel = new SaslConnection(mechanism, url);

         channel.connect();
         String message = "There is more than one way to do it";
         channel.send(message.getBytes("ASCII"));
         channel.receive();

         // write another string re-using the same session!
         channel.reconnect();
         message = "1 if by land. 2 if by sea...";
         channel.send(message.getBytes("ASCII"));
         channel.receive();
      } catch (Exception x) {
         x.printStackTrace(System.err);
      }
      System.out.println("Client "+name+" shutdown...");
   }
}
