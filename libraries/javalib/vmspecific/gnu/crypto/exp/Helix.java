package gnu.crypto.exp;  // -*- Mode: java; c-basic-offset: 3; -*-

// ---------------------------------------------------------------------------
// $Id: Helix.java,v 1.1 2005/10/19 20:15:33 guilhem Exp $
//
// Copyright (C) 2004 Free Software Foundation, Inc.
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
//
// ---------------------------------------------------------------------------

import gnu.crypto.cipher.IBlockCipher;
import gnu.crypto.mac.IMac;
import gnu.crypto.util.Sequence;
import gnu.crypto.util.Util;

import java.security.InvalidKeyException;
import java.util.Arrays;
import java.util.Collections;
import java.util.Iterator;
import java.util.Map;

/**
 * The Helix stream cipher and message authentication code. Helix combines a
 * word-aligned stream cipher with a message authentication code in a single
 * cryptographic primitive.
 *
 * <p>References:</p>
 * <ol>
 * <li>Niels Ferguson, Doug Whiting, Bruce Schneier, John Kelsey, Stefan Lucks,
 * and Tadayoshi Kohno. "Helix: Fast Encryption and Authentication in a Single
 * Cryptographic Primitive". <a
 * href="http://www.macfergus.com/helix/helix.pdf">http://www.macfergus.com/helix/helix.pdf</a></li>
 * </ol>
 */
public class Helix implements IBlockCipher, IMac {

   // Constants and fields.
   // ------------------------------------------------------------------------

   private static final boolean DEBUG = true;
   private void debug(String s) {
      System.err.print(">>> Helix: ");
      System.err.println(s);
   }

   /**
    * Property name for the <i>nonce</i>. This parameter is always required
    * and must be a sixteen-byte array.
    */
   public static final String NONCE = "gnu.crypto.helix.nonce";

   /**
    * Property name for the state. This is an {@link Integer} containing one
    * of the constants {@link #ENCRYPTION}, {@link #DECRYPTION}, or {@link
    * #MAC}.
    *
    * <p>If omitted, ENCRYPTION is assumed.
    */
   public static final String STATE = "gnu.crypto.helix.state";

   /**
    * The state constant for encryption and optional MAC.
    */
   public static final int ENCRYPTION = 0;

   /**
    * The state constant for decryption and optional MAC.
    */
   public static final int DECRYPTION = 1;

   /**
    * The state constant for MAC-only.
    */
   public static final int MAC = 2;

   private static final int TAG_MAGIC = 0x912d94f1;

   private boolean macOnly = false;
   private boolean haveKey = false, haveNonce = false;
   private long block, count;
   private int z0, z1, z2, z3, z4, lU;
   private int state;
   private final int[] K = new int[8];
   private final int[] N = new int[8];
   private int macBuf = 0, macOff = 0;

   // Constructor.
   // ------------------------------------------------------------------------

   public Helix() {
   }

   // Instance methods.
   // ------------------------------------------------------------------------

   public String name()
   {
      return "Helix";
   }

   public int defaultBlockSize()
   {
      return 4;
   }

   public int defaultKeySize()
   {
      return 32;
   }

   public Iterator blockSizes() {
      return Collections.singleton(new Integer(4)).iterator();
   }

   public Iterator keySizes() {
      return new Sequence(0, 32).iterator();
   }

   public int macSize() {
      return 16;
   }

   public int currentBlockSize() {
      return 4;
   }

   public Object clone() {
      Helix that = new Helix();
      that.block = block;
      that.z0 = z0;
      that.z1 = z1;
      that.z2 = z2;
      that.z3 = z3;
      that.z4 = z4;
      that.lU = lU;
      System.arraycopy(K, 0, that.K, 0, K.length);
      System.arraycopy(N, 0, that.N, 0, N.length);
      return that;
   }

   public void reset() {
      Arrays.fill(N, 0);
      count = block = 0;
      haveNonce = false;
      z0 = 0;
      z1 = 0;
      z2 = 0;
      z3 = 0;
      z4 = 0;
   }

   public void init(Map attributes) throws InvalidKeyException {
      Integer st = (Integer) attributes.get(STATE);
      if (st == null) {
         state = ENCRYPTION;
      } else {
         switch (st.intValue()) {
            case ENCRYPTION:
               state = ENCRYPTION;
               break;
            case DECRYPTION:
               state = DECRYPTION;
               break;
            case MAC:
               state = MAC;
               break;
            default:
               throw new IllegalArgumentException("invalid state ("+st+")");
         }
      }

      byte[] key = (byte[]) attributes.get(KEY_MATERIAL);
      if (key == null && !haveKey) {
         key = (byte[]) attributes.get(MAC_KEY_MATERIAL);
         if (key == null && !haveKey) {
            throw new InvalidKeyException("null key");
         }
      }
      if (key != null) {
         lU = Math.min(key.length, 32);
         if (DEBUG) {
            debug("user key length: " + lU);
         }
         if (key.length < 32) {
            byte[] b = new byte[32];
            System.arraycopy(key, 0, b, 0, key.length);
            key = b;
         }
         int[] preK = new int[40];
         K[0] = (key[ 0] & 0xFF)       | (key[ 1] & 0xFF) <<  8
              | (key[ 2] & 0xFF) << 16 | (key[ 3] & 0xFF) << 24;
         K[1] = (key[ 4] & 0xFF)       | (key[ 5] & 0xFF) <<  8
              | (key[ 6] & 0xFF) << 16 | (key[ 7] & 0xFF) << 24;
         K[2] = (key[ 8] & 0xFF)       | (key[ 9] & 0xFF) <<  8
              | (key[10] & 0xFF) << 16 | (key[11] & 0xFF) << 24;
         K[3] = (key[12] & 0xFF)       | (key[13] & 0xFF) <<  8
              | (key[14] & 0xFF) << 16 | (key[15] & 0xFF) << 24;
         K[4] = (key[16] & 0xFF)       | (key[17] & 0xFF) <<  8
              | (key[18] & 0xFF) << 16 | (key[19] & 0xFF) << 24;
         K[5] = (key[20] & 0xFF)       | (key[21] & 0xFF) <<  8
              | (key[22] & 0xFF) << 16 | (key[23] & 0xFF) << 24;
         K[6] = (key[24] & 0xFF)       | (key[25] & 0xFF) <<  8
              | (key[26] & 0xFF) << 16 | (key[27] & 0xFF) << 24;
         K[7] = (key[28] & 0xFF)       | (key[29] & 0xFF) <<  8
              | (key[30] & 0xFF) << 16 | (key[31] & 0xFF) << 24;

         for (int i = 0; i < 8; i++) {
            int x0, x1, x2, x3;
            z0 = K[0];
            z1 = K[1];
            z2 = K[2];
            z3 = K[3];
            z4 = lU + 64;
            block(0, 0, 0);
            x0 = K[0];
            x1 = K[1];
            x2 = K[2];
            x3 = K[3];
            K[0] = z0 ^ K[4];
            K[1] = z1 ^ K[5];
            K[2] = z2 ^ K[6];
            K[3] = z3 ^ K[7];
            K[4] = x0;
            K[5] = x1;
            K[6] = x2;
            K[7] = x3;
         }
         haveKey = true;

         if (DEBUG) {
            debug("working key:");
            StringBuffer buf = new StringBuffer();
            for (int i = 0; i < K.length; i++) {
               String s = Integer.toHexString(K[i] & 0xFF);
               if (s.length() == 1) buf.append('0');
               buf.append(s).append(' ');
               s = Integer.toHexString(K[i] >>> 8 & 0xFF);
               if (s.length() == 1) buf.append('0');
               buf.append(s).append(' ');
               s = Integer.toHexString(K[i] >>> 16 & 0xFF);
               if (s.length() == 1) buf.append('0');
               buf.append(s).append(' ');
               s = Integer.toHexString(K[i] >>> 24 & 0xFF);
               if (s.length() == 1) buf.append('0');
               buf.append(s).append(' ');
               if (i == 3) {
                  debug(buf.toString());
                  buf.setLength(0);
               }
            }
            debug(buf.toString());
         }
      }

      byte[] nonce = (byte[]) attributes.get(NONCE);
      if (nonce.length < 16) {
         byte[] b = new byte[16];
         System.arraycopy(nonce, 0, b, 0, nonce.length);
         nonce = b;
      }
      N[0] = (nonce[ 0] & 0xFF)       | (nonce[ 1] & 0xFF) <<  8
           | (nonce[ 2] & 0xFF) << 16 | (nonce[ 3] & 0xFF) << 24;
      N[1] = (nonce[ 4] & 0xFF)       | (nonce[ 5] & 0xFF) <<  8
           | (nonce[ 6] & 0xFF) << 16 | (nonce[ 7] & 0xFF) << 24;
      N[2] = (nonce[ 8] & 0xFF)       | (nonce[ 9] & 0xFF) <<  8
           | (nonce[10] & 0xFF) << 16 | (nonce[11] & 0xFF) << 24;
      N[3] = (nonce[12] & 0xFF)       | (nonce[13] & 0xFF) <<  8
           | (nonce[14] & 0xFF) << 16 | (nonce[15] & 0xFF) << 24;
      N[4] =   - N[0];
      N[5] = 1 - N[1];
      N[6] = 2 - N[2];
      N[7] = 3 - N[3];
      haveNonce = true;

      z0 = K[3] ^ N[0];
      z1 = K[4] ^ N[1];
      z2 = K[5] ^ N[2];
      z3 = K[6] ^ N[3];
      z4 = K[7];
      block = -8;
      count = 0;

      block(0, X0(), X1()); block++;
      block(0, X0(), X1()); block++;
      block(0, X0(), X1()); block++;
      block(0, X0(), X1()); block++;
      block(0, X0(), X1()); block++;
      block(0, X0(), X1()); block++;
      block(0, X0(), X1()); block++;
      block(0, X0(), X1()); block++;
   }

   public void encryptBlock(byte[] in, int inOff, byte[] out, int outOff) {
      if (!haveKey || !haveNonce) {
         throw new IllegalStateException("not initialized");
      }
      if (state != ENCRYPTION) {
         throw new IllegalStateException("not configured for encryption");
      }
      int p = (in[inOff++] & 0xFF)       | (in[inOff++] & 0xFF) <<  8
            | (in[inOff++] & 0xFF) << 16 | (in[inOff  ] & 0xFF) << 24;
      int c = p ^ z0;
      block(p, X0(), X1());
      block++;
      out[outOff++] = (byte)  c;
      out[outOff++] = (byte) (c >>>  8);
      out[outOff++] = (byte) (c >>> 16);
      out[outOff  ] = (byte) (c >>> 24);
      count += 4;
   }

   public void decryptBlock(byte[] in, int inOff, byte[] out, int outOff) {
      if (!haveKey || !haveNonce) {
         throw new IllegalStateException("not initialized");
      }
      if (state != DECRYPTION) {
         throw new IllegalStateException("not configured for decryption");
      }
      int c = (in[inOff++] & 0xFF)       | (in[inOff++] & 0xFF) <<  8
            | (in[inOff++] & 0xFF) << 16 | (in[inOff  ] & 0xFF) << 24;
      int p = c ^ z0;
      block(p, X0(), X1());
      out[outOff++] = (byte)  p;
      out[outOff++] = (byte) (p >>> 8);
      out[outOff++] = (byte) (p >>> 16);
      out[outOff  ] = (byte) (p >>> 24);
      count += 4;
   }

   public void update(byte b) {
      if (!haveKey || !haveNonce) {
         throw new IllegalStateException("not initialized");
      }
      if (state != MAC) {
         throw new IllegalStateException("not configured as a MAC");
      }
      macBuf |= (b & 0xFF) << ((count % 4) * 8);
      count++;
      if ((count % 4) == 0) {
         block(macBuf, X0(), X1());
         macBuf = 0;
      }
   }

   public void update(byte[] buf, int off, int len) {
      // XXX fixme: make this more efficient.
      for (int i = 0; i < len; i++) {
         update(buf[i+off]);
      }
   }

   public byte[] digest() {
      byte[] digest = new byte[16];
      if (state == MAC && count % 4 != 0) {
         block(macBuf, X0(), X1());
         block++;
      }

      // Post-mixing.
      z0 ^= TAG_MAGIC;
      int l = (int) (count % 4);
      block(l, X0(), X1()); block++;
      block(l, X0(), X1()); block++;
      block(l, X0(), X1()); block++;
      block(l, X0(), X1()); block++;
      block(l, X0(), X1()); block++;
      block(l, X0(), X1()); block++;
      block(l, X0(), X1()); block++;
      block(l, X0(), X1()); block++;

      // Generate the tag.
      block(l, X0(), X1()); block++;
      digest[ 0] = (byte)  z0;
      digest[ 1] = (byte) (z0 >>>  8);
      digest[ 2] = (byte) (z0 >>> 16);
      digest[ 3] = (byte) (z0 >>> 24);
      block(l, X0(), X1()); block++;
      digest[ 4] = (byte)  z0;
      digest[ 5] = (byte) (z0 >>>  8);
      digest[ 6] = (byte) (z0 >>> 16);
      digest[ 7] = (byte) (z0 >>> 24);
      block(l, X0(), X1()); block++;
      digest[ 8] = (byte)  z0;
      digest[ 9] = (byte) (z0 >>>  8);
      digest[10] = (byte) (z0 >>> 16);
      digest[11] = (byte) (z0 >>> 24);
      block(l, X0(), X1()); block++;
      digest[12] = (byte)  z0;
      digest[13] = (byte) (z0 >>>  8);
      digest[14] = (byte) (z0 >>> 16);
      digest[15] = (byte) (z0 >>> 24);

      reset();
      return digest;
   }

   public boolean selfTest() {
      return true; // XXX
   }

   // Own methods.
   // ------------------------------------------------------------------------

   private void block(final int p, final int x0, final int x1) {
      if (DEBUG) {
         debug("block ("+block+") x0="+x0+" x1="+x1+" p="+p);
      }
      z0 += z3;
      z3 = z3 << 15 | z3 >>> -15;
      z1 += z4;
      z4 = z4 << 25 | z4 >>> -25;
      z2 ^= z0;
      z0 = z0 <<  9 | z0 >>>  -9;
      z3 ^= z1;
      z1 = z1 << 10 | z1 >>> -10;
      z4 += z2;
      z2 = z2 << 17 | z2 >>> -17;

      z0 ^= z3 + x0;
      z3 = z3 << 30 | z3 >>> -30;
      z1 ^= z4;
      z4 = z4 << 13 | z4 >>> -13;
      z2 += z0;
      z0 = z0 << 20 | z0 >>> -20;
      z3 += z1;
      z1 = z1 << 11 | z1 >>> -11;
      z4 ^= z2;
      z2 = z2 <<  5 | z2 >>>  -5;

      z0 += z3 ^ p;
      z3 = z3 << 15 | z3 >>> -15;
      z1 += z4;
      z4 = z4 << 25 | z4 >>> -25;
      z2 ^= z0;
      z0 = z0 <<  9 | z0 >>>  -9;
      z3 ^= z1;
      z1 = z1 << 10 | z1 >>> -10;
      z4 += z2;
      z2 = z2 << 17 | z2 >>> -17;

      z0 ^= z3 + x1;
      z3 = z3 << 30 | z3 >>> -30;
      z1 ^= z4;
      z4 = z4 << 13 | z4 >>> -13;
      z2 += z0;
      z0 = z0 << 20 | z0 >>> -20;
      z3 += z1;
      z1 = z1 << 11 | z1 >>> -11;
      z4 ^= z2;
      z2 = z2 <<  5 | z2 >>>  -5;
   }

   private int X0() {
      return K[(int) (block & 7)];
   }

   private int X1() {
      int t = K[(int) ((block + 4) & 7)] + N[(int) (block & 7)] + (int) (block + 8);
      if ((block & 3) == 3)
         t += (int) ((block + 8) >>> 31);
      else if ((block & 3) == 1)
         t += lU << 2;
      return t;
   }
}
