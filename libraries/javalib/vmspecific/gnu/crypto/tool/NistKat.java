package gnu.crypto.tool;

// ----------------------------------------------------------------------------
// $Id: NistKat.java,v 1.1 2005/10/19 20:15:52 guilhem Exp $
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
import gnu.crypto.util.Util;
import java.security.InvalidKeyException;
import java.util.HashMap;
import java.util.Arrays;

/**
 * <p>For a designated symmetric block cipher algorithm, this command generates
 * and exercises Known Answer Tests data for either, or both, Variable Key and
 * Variable Text suites.</p>
 *
 * <p>NistKat's output file format is in conformance with the layout described
 * in Section 3 of NIST's document "Description of Known Answer Tests and Monte
 * Carlo Tests for Advanced Encryption Standard (AES) Candidate Algorithm
 * Submissions" dated January 7, 1998.</p>
 *
 * <p>References:</p>
 * <ol>
 *    <li><a href="http://csrc.nist.gov/encryption/aes/katmct/katmct.htm">Known
 *    Answer Test (KAT) and Monte Carlo Test (MCT) Information</a></li>
 * </ol>
 *
 * @version $Revision: 1.1 $
 */
public final class NistKat
{
   // Constants and variables
   // -------------------------------------------------------------------------

   private String cipherName;
   private int keySize; // in bits
   private IBlockCipher cipher;
   private long encBlocks; // total count of encrypted blocks
   private long decBlocks; // total count of decrypted blocks
   private long keyCount;  // total count of key creation requests

   // Constructor(s)
   // -------------------------------------------------------------------------

   /** Trivial 0-arguments constructor to enforce usage through main(). */
   private NistKat(String cipherName, IBlockCipher cipher, int keySize) {
      super();

      this.cipherName = cipherName;
      this.cipher = cipher;
      this.keySize = keySize;
   }

   // Class methods
   // -------------------------------------------------------------------------

   /**
    * <p>The main entry point for this tool.</p>
    *
    * <pre>
    * Usage:
    *    gnu.crypto.tool.NistKat (options) cipher [key-size]
    *
    * Where:
    *    cipher
    *       The canonical name of the cipher algorithm.
    *    key-size
    *       The key-size in bits to use for the algorithm.  If unspecified,
    *       then the three NIST key-sizes 128, 192 and 256 shall be used.
    *
    * Options:
    *    -k | -t | -a
    *       Generate Variable Key Known Answer Test (KAT) data only, Variable
    *       Text KAT data only, or both.  For backward compatibility, if this
    *       option is unspecified, then -a (both Variable Text and Variable
    *       Key data) is activated.
    *    -h
    *       Print this help page.
    * </pre>
    */
   public static void main(String[] args) {
      if (args == null || args.length == 0) {
         printUsage();
         return;
      }

      try {
         // set defaults
         boolean doVT = false; // set to true if -t or -a is specified
         boolean doVK = false; // set to true if -k or -a is specified
         String cipherName = null;
         String kSize = null;

         // parse arguments
         for (int i = 0; i < args.length; i++) {
            String arg = args[i];
            if (arg.startsWith("-")) { // an option
               String option = arg.substring(1);
               if (option.equals("k")) { // variable key ON
                  doVK = true;
                  continue;
               }
               if (option.equals("t")) { // variable text ON
                  doVT = true;
                  continue;
               }
               if (option.equals("a")) { // both ON
                  doVK = true;
                  doVT = true;
                  continue;
               }
               if (option.equals("h")) {
                  printUsage();
                  continue;
               }
            }
            if (cipherName == null) {
               cipherName = args[i].trim();
               continue;
            }
            if (kSize == null) {
               kSize = args[i].trim();
               break; // ignore anything after this
            }
         }

         // validity checks
         if (!doVT && !doVK) { // for backward compatibility turn'em ON
            doVT = true;
            doVK = true;
         }

         if (cipherName == null || cipherName.equals("")) {
            System.err.println("Missing cipher name...");
            printUsage();
            return;
         }

         // execute command
         IBlockCipher cipher = CipherFactory.getInstance(cipherName);
         long time = -System.currentTimeMillis();
         NistKat[] cmd;
         if (kSize == null || kSize.equals("")) {
            cmd = new NistKat[] {
               new NistKat(cipherName, cipher, 128),
               new NistKat(cipherName, cipher, 192),
               new NistKat(cipherName, cipher, 256)
            };
         } else {
            cmd = new NistKat[] {
               new NistKat(cipherName, cipher, Integer.parseInt(kSize))
            };
         }

         int i;
         if (doVK) {
            // print preamble
            System.out.println();
            System.out.println("=========================");
            System.out.println();
            System.out.println("Electronic Codebook (ECB) Mode");
            System.out.println("Variable Key Known Answer Tests");
            System.out.println();
            System.out.println("Algorithm Name: "+String.valueOf(cipherName));
            System.out.println();
            System.out.println("==========");

            for (i = 0; i < cmd.length; i++) {
               cmd[i].variableKeyKat();
            }
         }

         if (doVT) {
            // print preamble
            System.out.println();
            System.out.println("=========================");
            System.out.println();
            System.out.println("Electronic Codebook (ECB) Mode");
            System.out.println("Variable Text Known Answer Tests");
            System.out.println();
            System.out.println("Algorithm Name: "+String.valueOf(cipherName));

            for (i = 0; i < cmd.length; i++) {
               cmd[i].variableTextKat();
            }
         }

         time += System.currentTimeMillis();
         long encryptions = 0L;
         long decryptions = 0L;
         long keySetups = 0L;
         for (i = 0; i < cmd.length; i++) {
            encryptions += cmd[i].encBlocks;
            decryptions += cmd[i].decBlocks;
            keySetups += cmd[i].keyCount;
         }

         // print postamble
         System.out.println();
         System.out.println("Total execution time (ms): "+String.valueOf(time));
         System.out.println("During this time, "+String.valueOf(cipherName)+ ":");
         System.out.println("  Encrypted "+String.valueOf(encryptions)+" blocks");
         System.out.println("  Decrypted "+String.valueOf(decryptions)+" blocks");
         System.out.println("  Created "+String.valueOf(keySetups)+" session keys");

      } catch (Exception x) {
         x.printStackTrace(System.err);
      }
   }

   /** Prints a simple help page to <code>System.err</code>. */
   private static final void printUsage() {
      System.err.println();
      System.err.println("Usage:");
      System.err.println("   gnu.crypto.tool.NistKat (options) cipher [key-size]");
      System.err.println();
      System.err.println("Where:");
      System.err.println("   cipher");
      System.err.println("      The canonical name of the cipher algorithm.");
      System.err.println("   key-size");
      System.err.println("      The key-size in bits to use for the algorithm.  If unspecified,");
      System.err.println("      then the three NIST key-sizes 128, 192 and 256 shall be used.");
      System.err.println();
      System.err.println("Options:");
      System.err.println("   -k | -t | -a");
      System.err.println("      Generate Variable Key Known Answer Test (KAT) data only, Variable");
      System.err.println("      Text KAT data only, or both.  For backward compatibility, if this");
      System.err.println("      option is unspecified, then -a (both Variable Text and Variable");
      System.err.println("      Key data) is activated.");
      System.err.println("   -h");
      System.err.println("      Print this help page.");
      System.err.println();
   }

   // Instance methods
   // -------------------------------------------------------------------------

   private void variableKeyKat() throws InvalidKeyException {
      int count = keySize / 8; // number of bytes in key material
      int size = cipher.defaultBlockSize();
      byte[] k = new byte[count];
      byte[] pt = new byte[size];  // plaintext
      byte[] ct = new byte[size];  // ciphertext
      byte[] cpt = new byte[size]; // computed plaintext
      int round = 0;               // current round ord. number

      System.out.println();
      System.out.println("KEYSIZE="+String.valueOf(keySize));
      System.out.println();
      System.out.println("PT="+Util.toString(pt));
      System.out.println();

      // The key bytes are organised and numbered as follows:
      //
      // |<- byte 0 ->|<- byte 1 ->|<- ... ->|<- byte n ->|
      // |<------------- bit_(n-1) to bit_0 ------------->|
      //
      HashMap map = new HashMap();
      for (int i = 0; i < count; i++) {
         for (int j = 0; j < 8; j++) {
            round++;
            System.out.println("I="+String.valueOf(round));

            k[i] = (byte)(1 << (7 - j));
            System.out.println("KEY="+Util.toString(k));
            map.put(IBlockCipher.KEY_MATERIAL, k);
            cipher.init(map);
            keyCount++;

            cipher.encryptBlock(pt, 0, ct, 0);
            encBlocks++;
            System.out.print("CT="+Util.toString(ct));

            cipher.decryptBlock(ct, 0, cpt, 0);
            decBlocks++;
            cipher.reset();

            if (!Arrays.equals(pt, cpt))  // check if results match
               System.out.print(" *** ERROR ***");

            System.out.println();
            System.out.println();
         }

         k[i] = 0x00;
      }

      System.out.println("==========");
   }

   private void variableTextKat() throws InvalidKeyException {
      byte[] k = new byte[keySize / 8];
      int count = cipher.defaultBlockSize(); // the cipher's block size
      byte[] pt = new byte[count];  // plaintext
      byte[] ct = new byte[count];  // ciphertext
      byte[] cpt = new byte[count]; // computed plaintext
      int round = 0;                // current round ord. number
      HashMap map = new HashMap();

      map.put(IBlockCipher.KEY_MATERIAL, k);
      cipher.init(map);
      keyCount++;

      System.out.println();
      System.out.println("KEYSIZE="+String.valueOf(keySize));
      System.out.println();
      System.out.println("KEY="+Util.toString(k));
      System.out.println();

      // The plaintext bytes are organised and numbered as follows:
      //
      // |<- byte 0 ->|<- byte 1 ->|<- ... ->|<- byte n ->|
      // |<------------- bit_(n-1) to bit_0 ------------->|
      //
      for (int i = 0; i < count; i++) {
         for (int j = 0; j < 8; j++) {
            round++;
            System.out.println("I="+String.valueOf(round));

            pt[i] = (byte)(1 << (7 - j));
            System.out.println("PT="+Util.toString(pt));

            cipher.encryptBlock(pt, 0, ct, 0);
            encBlocks++;
            System.out.print("CT="+Util.toString(ct));

            cipher.decryptBlock(ct, 0, cpt, 0);
            decBlocks++;

            if (!Arrays.equals(pt, cpt))
               System.out.print(" *** ERROR ***");

            System.out.println();
            System.out.println();
         }

         pt[i] = 0x00;
      }

      cipher.reset();
      System.out.println("==========");
   }
}
