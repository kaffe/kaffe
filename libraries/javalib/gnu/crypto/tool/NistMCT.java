package gnu.crypto.tool;

// ----------------------------------------------------------------------------
// $Id: NistMCT.java,v 1.2 2005/07/04 00:05:11 robilad Exp $
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
import java.util.Arrays;
import java.util.HashMap;

/**
 * <p>For a designated symmetric block cipher algorithm, this command generates
 * and exercises Monte Carlo Tests data for both Encryption and Decryption in
 * Electronic Codebook (ECB) and Cipher Block Chaining (CBC) modes.</p>
 *
 * <p>NistMCT's output file format is in conformance with the layout described
 * in Section 4 of NIST's document "Description of Known Answer Tests and Monte
 * Carlo Tests for Advanced Encryption Standard (AES) Candidate Algorithm
 * Submissions" dated January 7, 1998.</p>
 *
 * <p>References:</p>
 * <ol>
 *    <li><a href="http://csrc.nist.gov/encryption/aes/katmct/katmct.htm">Known
 *    Answer Test (KAT) and Monte Carlo Test (MCT) Information</a></li>
 * </ol>
 *
 * @version $Revision: 1.2 $
 */
public class NistMCT
{
   // Constants and variables
   // -------------------------------------------------------------------------

   private String cipherName;
   private int keySize; // in bits
   private IBlockCipher cipher;

   // statistics fields
   private long encBlocks; // total count of encrypted blocks
   private long decBlocks; // total count of decrypted blocks
   private long keyCount; // total count of key creation requests

   // Constructor(s)
   // -------------------------------------------------------------------------

   /** Trivial 0-arguments constructor to enforce usage through main(). */
   private NistMCT(String cipherName, IBlockCipher cipher, int keySize) {
      super();

      this.cipherName = cipherName;
      this.cipher = cipher;
      this.keySize = keySize;
   }

   // Class methods
   // -------------------------------------------------------------------------

   /**
    * <p>The main entry point for the tool.</p>
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
    *    -E | -C | -A
    *       Generate Monte Carlo Test (MCT) data for ECB mode only, CBC mode
    *       only, or both.  For backward compatibility, if this option is
    *       unspecified, then -A (both modes) is activated.
    *    -e | -d | -a
    *       Generate Monte Carlo Test (MCT) data for a designated cipher in
    *       the selected mode(s), for Encryption only, Decryption only, or
    *       both.  For backward compatibility, if this option is unspecified,
    *       then -a (both states) is activated.
    *    -h
    *       Print this help page.
    * </pre>
    */
   public static void main (String[] args) {
      if (args == null || args.length == 0) {
         printUsage();
         return;
      }

      try {
         // set defaults
         boolean doECB = false; // set to true if -E or -A is specified
         boolean doCBC = false; // set to true if -C or -A is specified
         boolean doEnc = false; // set to true if -e or -a is specified
         boolean doDec = false; // set to true if -d or -a is specified
         String cipherName = null;
         String kSize = null;

         // parse arguments
         for (int i = 0; i < args.length; i++) {
            String arg = args[i];
            if (arg.startsWith("-")) { // an option
               String option = arg.substring(1);
               if (option.equals("E")) { // ECB ON
                  doECB = true;
                  continue;
               }
               if (option.equals("C")) { // CBC ON
                  doCBC = true;
                  continue;
               }
               if (option.equals("A")) { // both ON
                  doECB = true;
                  doCBC = true;
                  continue;
               }
               if (option.equals("e")) { // encryption ON
                  doEnc = true;
                  continue;
               }
               if (option.equals("d")) { // decryption ON
                  doDec = true;
                  continue;
               }
               if (option.equals("a")) { // both ON
                  doEnc = true;
                  doDec = true;
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
         if (!doECB && !doCBC) { // for backward compatibility turn'em ON
            doECB = true;
            doCBC = true;
         }
         if (!doEnc && !doDec) { // for backward compatibility turn'em ON
            doEnc = true;
            doDec = true;
         }

         if (cipherName == null || cipherName.equals("")) {
            System.err.println("Missing cipher name...");
            printUsage();
            return;
         }

         IBlockCipher cipher = CipherFactory.getInstance(cipherName);
         long time = -System.currentTimeMillis();
         NistMCT[] cmd;
         if (kSize == null || kSize.equals("")) {
            cmd = new NistMCT[] {
               new NistMCT(cipherName, cipher, 128),
               new NistMCT(cipherName, cipher, 192),
               new NistMCT(cipherName, cipher, 256)
            };
         } else {
            cmd = new NistMCT[] {
               new NistMCT(cipherName, cipher, Integer.parseInt(kSize))
            };
         }

         int i;
         if (doECB) {
            if (doEnc) {
               // print preamble
               System.out.println();
               System.out.println("=========================");
               System.out.println();
               System.out.println("Electronic Codebook (ECB) Mode - ENCRYPTION");
               System.out.println("Monte Carlo Test");
               System.out.println();
               System.out.println("Algorithm Name: "+String.valueOf(cipherName));
               System.out.println();
               System.out.println("==========");

               for (i = 0; i < cmd.length; i++) {
                  cmd[i].ecbEncrypt();
               }
            }
            if (doDec) {
               // print preamble
               System.out.println();
               System.out.println("=========================");
               System.out.println();
               System.out.println("Electronic Codebook (ECB) Mode - DECRYPTION");
               System.out.println("Monte Carlo Test");
               System.out.println();
               System.out.println("Algorithm Name: "+String.valueOf(cipherName));
               System.out.println();
               System.out.println("==========");

               for (i = 0; i < cmd.length; i++) {
                  cmd[i].ecbDecrypt();
               }
            }
         }

         if (doCBC) {
            if (doEnc) {
               // print preamble
               System.out.println();
               System.out.println("=========================");
               System.out.println();
               System.out.println("Cipher Block Chaining (CBC) Mode - ENCRYPTION");
               System.out.println("Monte Carlo Test");
               System.out.println();
               System.out.println("Algorithm Name: "+String.valueOf(cipherName));
               System.out.println();
               System.out.println("==========");

               for (i = 0; i < cmd.length; i++) {
                  cmd[i].cbcEncrypt();
               }
            }
            if (doDec) {
               // print preamble
               System.out.println();
               System.out.println("=========================");
               System.out.println();
               System.out.println("Cipher Block Chaining (CBC) Mode - DECRYPTION");
               System.out.println("Monte Carlo Test");
               System.out.println();
               System.out.println("Algorithm Name: "+String.valueOf(cipherName));
               System.out.println();
               System.out.println("==========");

               for (i = 0; i < cmd.length; i++) {
                  cmd[i].cbcDecrypt();
               }
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
      System.err.println("   -E | -C | -A");
      System.err.println("      Generate Monte Carlo Test (MCT) data for ECB mode only, CBC mode");
      System.err.println("      only, or both.  For backward compatibility, if this option is ");
      System.err.println("      unspecified, then -A (both modes) is activated.");
      System.err.println("   -e | -d | -a");
      System.err.println("      Generate Monte Carlo Test (MCT) data for a designated cipher in");
      System.err.println("      the selected mode(s), for Encryption only, Decryption only, or");
      System.err.println("      both.  For backward compatibility, if this option is unspecified,");
      System.err.println("      then -a (both states) is activated.");
      System.err.println("   -h");
      System.err.println("      Print this help page.");
      System.err.println();
   }

   // Instance methods
   // -------------------------------------------------------------------------

//   private void ecb() throws InvalidKeyException {
//      ecbEncrypt();
//      ecbDecrypt();
//   }

   private void ecbEncrypt() throws InvalidKeyException {
      int keylen = keySize / 8;      // number of bytes in user key
      byte[] keyMaterial = new byte[keylen];
      int size = cipher.defaultBlockSize(); // cipher block size in bytes
      byte[] pt = new byte[size];    // plaintext
      byte[] cpt = new byte[size];   // computed plaintext
      byte[] ct = new byte[size];    // ciphertext @round 9999
      byte[] ct_1 = new byte[size];  // ciphertext @round 9998
      int j, k, count;               // temp vars

      System.out.println();
      System.out.println("KEYSIZE="+String.valueOf(keySize));
      System.out.println();

      // step 1. will use all zeroes.

      HashMap map = new HashMap();
      for (int i = 0; i < 400; i++) { // step 2
         // Encryption
         System.out.println("I="+String.valueOf(i)); // step 2.a
         System.out.println("KEY="+Util.toString(keyMaterial));
         System.out.println("PT="+Util.toString(pt));

         map.put(IBlockCipher.KEY_MATERIAL, keyMaterial);
         cipher.init(map);

         keyCount++;
         cipher.encryptBlock(pt, 0, ct_1, 0); // step 2.b
         encBlocks++;
         for (j = 1; j < 9999; j++) {
            cipher.encryptBlock(ct_1, 0, ct_1, 0);
            encBlocks++;
         }

         cipher.encryptBlock(ct_1, 0, ct, 0);
         encBlocks++;
         System.out.println("CT="+Util.toString(ct)); // step 2.c

         // Decryption
         cipher.decryptBlock(ct, 0, cpt, 0); // step 2.b
         decBlocks++;
         for (j = 1; j < 10000; j++) {
            cipher.decryptBlock(cpt, 0, cpt, 0);
            decBlocks++;
         }

         if (!Arrays.equals(pt, cpt)) { // check if results match
            System.out.println(" *** ERROR ***");
            throw new RuntimeException("ECB Encryption/Decryption mismatch");
         }

         System.out.println();
         cipher.reset();

         // may throw ArrayIndexOutOfBoundsException with non-AES ciphers; ie.
         // those for which: keylen < size || keylen > 2*size
         j = 0; // step 2.d
         if (keylen > size) {
            count = keylen - size;
            k = size - count;
            while (j < count) {
               keyMaterial[j++] ^= ct_1[k++];
            }
         }

         k = 0;
         while (j < keylen) {
            keyMaterial[j++] ^= ct[k++];
         }

         System.arraycopy(ct, 0, pt, 0, size); // step 2.e
      }

      System.out.println("==========");
   }

   void ecbDecrypt() throws InvalidKeyException {
      int keylen = keySize / 8;      // number of bytes in user key
      byte[] keyMaterial = new byte[keylen];
      int size = cipher.defaultBlockSize(); // cipher block size in bytes
      byte[] pt = new byte[size];    // plaintext
      byte[] cpt = new byte[size];   // computed plaintext
      byte[] ct = new byte[size];    // ciphertext @round 9999
      byte[] ct_1 = new byte[size];  // ciphertext @round 9998
      int j, k, count;                // temp vars

      System.out.println();
      System.out.println("KEYSIZE="+String.valueOf(keySize));
      System.out.println();

      // step 1. will use all zeroes.

      HashMap map = new HashMap();
      for (int i = 0; i < 400; i++) { // step 2
         map.put(IBlockCipher.KEY_MATERIAL, keyMaterial);
         cipher.init(map);
         keyCount++;

         // Encryption
         cipher.encryptBlock(pt, 0, ct_1, 0); // step 2.b
         encBlocks++;
         for (j = 1; j < 9999; j++) {
            cipher.encryptBlock(ct_1, 0, ct_1, 0);
            encBlocks++;
         }

         cipher.encryptBlock(ct_1, 0, ct, 0);
         encBlocks++;

         // Decryption
         System.out.println("I="+String.valueOf(i)); // step 2.a
         System.out.println("KEY="+Util.toString(keyMaterial));
         System.out.println("CT="+Util.toString(ct));

         cipher.decryptBlock(ct, 0, cpt, 0); // step 2.b
         decBlocks++;
         for (j = 1; j < 10000; j++) {
            cipher.decryptBlock(cpt, 0, cpt, 0);
            decBlocks++;
         }

         System.out.println("PT="+Util.toString(cpt)); // step 2.c

         if (!Arrays.equals(pt, cpt)) { // check if results match
            System.out.println(" *** ERROR ***");
            throw new RuntimeException("ECB Encryption/Decryption mismatch");
         }

         System.out.println();
         cipher.reset();

         // may throw ArrayIndexOutOfBoundsException with non-AES ciphers; ie.
         // those for which: keylen < size || keylen > 2*size
         j = 0; // step 2.d
         if (keylen > size) {
            count = keylen - size;
            k = size - count;
            while (j < count) {
               keyMaterial[j++] ^= ct_1[k++];
            }
         }

         k = 0;
         while (j < keylen) {
            keyMaterial[j++] ^= ct[k++];
         }

         System.arraycopy(ct, 0, pt, 0, size); // step 2.e (both)
      }

      System.out.println("==========");
   }

   void cbc() throws InvalidKeyException {
      cbcEncrypt();
      cbcDecrypt();
   }

   void cbcEncrypt() throws InvalidKeyException {
      int keylen = keySize / 8;      // number of bytes in user key material
      byte[] keyMaterial = new byte[keylen];
      int size = cipher.defaultBlockSize(); // cipher block size in bytes
      byte[] pt = new byte[size];    // plaintext
      byte[] ct = new byte[size];    // ciphertext
      byte[] iv = new byte[size];    // initialization vector
      int j, k, count;               // temp vars

      System.out.println();
      System.out.println("KEYSIZE="+String.valueOf(keySize));
      System.out.println();

      HashMap map = new HashMap();
      for (int i = 0; i < 400; i++) { // step 2
         // step 2.a is implicit since we're handling cv as iv
         System.out.println("I="+String.valueOf(i)); // step 2.b
         System.out.println("KEY="+Util.toString(keyMaterial));
         System.out.println("IV="+Util.toString(iv));
         System.out.println("PT="+Util.toString(pt));

         map.put(IBlockCipher.KEY_MATERIAL, keyMaterial);
         cipher.init(map);
         keyCount++;
         for (j = 0; j < 10000; j++) { // step 2.c
            for (k = 0; k < size; k++) {
               iv[k] ^= pt[k]; // step 2.c.i
            }

            System.arraycopy(ct, 0, pt, 0, size); // copy ct@(j-1) into pt
            cipher.encryptBlock(iv, 0, ct, 0); // step 2.c.ii
            encBlocks++;
            System.arraycopy(ct, 0, iv, 0, size); // set new iv/cv
         }

         System.out.println("CT="+Util.toString(ct)); // step 2.d
         System.out.println();
         cipher.reset();

         // may throw ArrayIndexOutOfBoundsException with non-AES ciphers; ie.
         // those for which: keylen < size || keylen > 2*size
         // remember: we keep ct@(j-1) values in pt...
         j = 0; // step 2.e
         if (keylen > size) {
            count = keylen - size;
            k = size - count;
            while (j < count) {
               keyMaterial[j++] ^= pt[k++];
            }
         }

         k = 0;
         while (j < keylen) {
            keyMaterial[j++] ^= ct[k++];
         }
      }

      System.out.println("==========");
   }

   void cbcDecrypt() throws InvalidKeyException {
      int keylen = keySize / 8;      // number of bytes in user key material
      byte[] keyMaterial = new byte[keylen];
      int size = cipher.defaultBlockSize(); // cipher block size in bytes
      byte[] pt = new byte[size];    // plaintext
      byte[] ct = new byte[size];    // ciphertext
      byte[] iv = new byte[size];    // initialization vector
      int j, k, count;               // temp vars

      System.out.println();
      System.out.println("KEYSIZE="+String.valueOf(keySize));
      System.out.println();

      // step 1. use all-zeroes values

      HashMap map = new HashMap();
      for (int i = 0; i < 400; i++) { // step 2
         // step 2.a is implicit since we're handling cv as iv
         System.out.println("I="+String.valueOf(i)); // step 2.b
         System.out.println("KEY="+Util.toString(keyMaterial));
         System.out.println("IV="+Util.toString(iv));
         System.out.println("CT="+Util.toString(ct));

         map.put(IBlockCipher.KEY_MATERIAL, keyMaterial);
         cipher.init(map);
         keyCount++;
         for (j = 0; j < 10000; j++) { // step 2.c
            cipher.decryptBlock(ct, 0, pt, 0); // steps 2.c.i + 2.c.ii
            decBlocks++;
            for (k = 0; k < size; k++)
               pt[k] ^= iv[k];    // step 2.c.iii

            System.arraycopy(ct, 0, iv, 0, size); // step 2.c.iv
            System.arraycopy(pt, 0, ct, 0, size);
         }

         System.out.println("PT="+Util.toString(pt)); // step 2.d
         System.out.println();
         cipher.reset();

         // may throw ArrayIndexOutOfBoundsException with non-AES ciphers; ie.
         // those for which: keylen < size || keylen > 2*size
         // remember: iv contains values of pt@(j-1)
         j = 0; // step 2.e
         if (keylen > size) {
            count = keylen - size;
            k = size - count;
            while (j < count) {
               keyMaterial[j++] ^= iv[k++];
            }
         }

         k = 0;
         while (j < keylen) {
            keyMaterial[j++] ^= pt[k++];
         }
      }

      System.out.println("==========");
   }
}
