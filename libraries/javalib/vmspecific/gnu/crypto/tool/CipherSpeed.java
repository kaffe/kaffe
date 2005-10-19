package gnu.crypto.tool;

// ----------------------------------------------------------------------------
// $Id: CipherSpeed.java,v 1.1 2005/10/19 20:15:52 guilhem Exp $
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

import gnu.crypto.cipher.CipherFactory;
import gnu.crypto.cipher.IBlockCipher;
import java.util.Arrays;
import java.util.HashMap;
import java.util.Iterator;

/**
 * <p>A tool to exercise a block cipher in order to measure its performance in
 * terms of encrypted/decrypted bytes per second.</p>
 *
 * @version $Revision: 1.1 $
 */
public final class CipherSpeed {

   // Constants and variables
   // -------------------------------------------------------------------------

   // Constructor(s)
   // -------------------------------------------------------------------------

   /** Trivial 0-arguments constructor to enforce Singleton pattern. */
   private CipherSpeed() {
      super();
   }

   // Class methods
   // -------------------------------------------------------------------------

   /**
    * <p>Accepts 0, 1 or 2 arguments. If no arguments are provided, this method
    * exercises every block cipher implementation using the cipher's default
    * key size. If one argument is provided, it's assumed to be the name of a
    * block cipher to exercise. That cipher is then exercised using its default
    * key size. Finally if two arguments are provided, the first is assumed to
    * be the block cipher name and the second its key size.</p>
    */
   public static void main(String[] args) {
      if (args == null) {
         args = new String[0];
      }

      switch (args.length) {
      case 0: // exercise all ciphers
         for (Iterator cit = CipherFactory.getNames().iterator(); cit.hasNext(); ) {
            speed((String) cit.next());
         }
         break;
      case 1:
         speed(args[0]);
         break;
      default:
         speed(args[0], Integer.parseInt(args[1]) / 8);
         break;
      }
   }

   private static void speed(String name) {
      System.out.println("Exercising "+String.valueOf(name)+"...");
      try {
         IBlockCipher cipher = CipherFactory.getInstance(name);
         speed(cipher, cipher.defaultKeySize());
      } catch (InternalError x) {
         System.out.println("Failed self-test...");
      }
   }

   private static void speed(String name, int keysize) {
      System.out.println("Exercising "+String.valueOf(name)+"-"
         +String.valueOf(keysize)+"...");
      try {
         IBlockCipher cipher = CipherFactory.getInstance(name);
         speed(cipher, keysize);
      } catch (InternalError x) {
         System.out.println("Failed self-test...");
      }
   }

   private static void speed(IBlockCipher cipher, int keysize) {
      try {
         int iterations = 1000000;
         int blocksize = cipher.defaultBlockSize();
         int i;
         byte[] kb = new byte[keysize];
         for (i = 0; i < keysize; i++) {
            kb[i] = (byte) i;
         }

         byte[] pt = new byte[blocksize];
         for (i = 0; i < blocksize; i++) {
            pt[i] = (byte) i;
         }

         System.out.println("Running "+iterations+" iterations:");
         System.out.print("Encryption: ");

         HashMap map = new HashMap();
         map.put(IBlockCipher.KEY_MATERIAL, kb);
         cipher.init(map);

         byte[] ct = (byte[]) pt.clone();
         long elapsed = -System.currentTimeMillis();
         for (i = 0; i < iterations; i++) {
            cipher.encryptBlock(ct, 0, ct, 0);
         }

         elapsed += System.currentTimeMillis();
         float secs = (elapsed > 1) ? (float) elapsed / 1000 : 1;
         float speed = (float) iterations * blocksize / 1024 / secs;

         System.out.println("time = "+secs+", speed = "+speed+" KB/s");
         System.out.print("Decryption: ");

         byte[] cpt = (byte[]) ct.clone();
         elapsed = -System.currentTimeMillis();
         for (i = 0; i < iterations; i++) {
            cipher.decryptBlock(cpt, 0, cpt, 0);
         }

         elapsed += System.currentTimeMillis();
         secs = (elapsed > 1) ? (float) elapsed / 1000 : 1;
         speed = (float) iterations * blocksize / 1024 / secs;

         System.out.println("time = "+secs+", speed = "+speed+" KB/s");

         if (!Arrays.equals(pt, cpt)) {
            throw new RuntimeException("Symmetric operation failed");
         }
      } catch (Exception x) {
         x.printStackTrace(System.err);
      }
   }

   // Instance methods
   // -------------------------------------------------------------------------
}
