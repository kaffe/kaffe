package gnu.crypto.tool;

// ----------------------------------------------------------------------------
// $Id: HashSpeed.java,v 1.2 2005/07/04 00:05:10 robilad Exp $
//
// Copyright (C) 2001, 2002, 2003 Free Software Foundation, Inc.
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

import gnu.crypto.hash.HashFactory;
import gnu.crypto.hash.IMessageDigest;

import java.util.Iterator;

/**
 * A tool to exercise a hash in order to measure its performance in terms of
 * number of bytes per second.
 *
 * @version $Revision: 1.2 $
 */
public final class HashSpeed {

   // Constants and variables
   // -------------------------------------------------------------------------

   // Constructor(s)
   // -------------------------------------------------------------------------

   /** Trivial 0-arguments constructor to enforce Singleton pattern. */
   private HashSpeed() {
      super();
   }

   // Class methods
   // -------------------------------------------------------------------------

   /**
    * Accepts 0, 1 or 2 arguments. If no arguments are provided, this method
    * exercises every hash implementation. If one argument is provided, it's
    * assumed to be the name of the hash algorithm. An instance of that hash is
    * then exercised.
    */
   public static void main(String[] args) {
      if (args == null) {
         args = new String[0];
      }

      switch (args.length) {
      case 0: // exercise all hashes
         for (Iterator hit = HashFactory.getNames().iterator(); hit.hasNext(); ) {
            speed((String) hit.next());
         }
         break;
      default:
         speed(args[0]);
         break;
      }
   }

   private static void speed(String name) {
      try {
         IMessageDigest hash = HashFactory.getInstance(name);
         speed(hash);
      } catch (InternalError x) {
         System.out.println(name+": Failed self-test...");
      }
   }

   private static void speed(IMessageDigest hash) {
      try {
         int iterations = 100000;
         int blocksize = 500;
         byte[] data = new byte[blocksize];
         int i;
         for (i = 0; i < blocksize; i++) {
            data[i] = (byte) i;
         }

         System.out.print(hash.name()+": Hashing "+String.valueOf(iterations)
               +" blocks of "+String.valueOf(blocksize)+" bytes each: ");
         long elapsed = -System.currentTimeMillis();
         for (i = 0; i < iterations; i++) {
            hash.update(data, 0, blocksize);
         }
         elapsed += System.currentTimeMillis();
         float secs = (elapsed > 1) ? (float) elapsed / 1000 : 1;
         float speed = (float) iterations * blocksize / 1024 / secs;

         System.out.println("time = "+secs+", speed = "+speed+" KB/s");
      } catch (Exception x) {
         x.printStackTrace(System.err);
      }
   }

   // Instance methods
   // -------------------------------------------------------------------------
}
