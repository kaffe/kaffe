/* SHA1.java -- The SHA-1 message digest.
   Copyright (C) 2003  Casey Marshall <csm@metastatic.org>

This file is part of GNU Peking, a PKI library.

GNU Peking is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU Peking is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Peking; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */


package org.metastatic.jessie.pki.provider;

import java.util.Arrays;
import java.security.DigestException;
import java.security.MessageDigestSpi;

public final class SHA1 extends MessageDigestSpi implements Cloneable
{

  // Fields.
  // -------------------------------------------------------------------------

  private long count;
  private int a, b, c, d, e;
  private int w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10, w11, w12, w13, w14,
    w15, w16, w17, w18, w19, w20, w21, w22, w23, w24, w25, w26, w27, w28, w29,
    w30, w31, w32, w33, w34, w35, w36, w37, w38, w39, w40, w41, w42, w43, w44,
    w45, w46, w47, w48, w49, w50, w51, w52, w53, w54, w55, w56, w57, w58, w59,
    w60, w61, w62, w63, w64, w65, w66, w67, w68, w69, w70, w71, w72, w73, w74,
    w75, w76, w77, w78, w79;
  private final byte[] one = new byte[1];

  // Constructor.
  // -------------------------------------------------------------------------

  public SHA1()
  {
    super();
    engineReset();
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  protected int engineGetDigestLength()
  {
    return 20;
  }

  protected void engineUpdate(byte b)
  {
    one[0] = b;
    engineUpdate(one, 0, 1);
  }

  protected void engineUpdate(byte[] in, int off, int len)
  {
    final int l = off + len;
    while (off < l)
      {
        int k = (int) (count & 63);
        count += Math.min(64 - k, l - off);
        switch (k)
          {
          case  0: w0   = (in[off++]       ) << 24; if (off == l) break;
          case  1: w0  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case  2: w0  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case  3: w0  |=  in[off++] & 0xFF;        if (off == l) break;
          case  4: w1   = (in[off++]       ) << 24; if (off == l) break;
          case  5: w1  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case  6: w1  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case  7: w1  |=  in[off++] & 0xFF;        if (off == l) break;
          case  8: w2   = (in[off++]       ) << 24; if (off == l) break;
          case  9: w2  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 10: w2  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 11: w2  |=  in[off++] & 0xFF;        if (off == l) break;
          case 12: w3   = (in[off++]       ) << 24; if (off == l) break;
          case 13: w3  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 14: w3  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 15: w3  |=  in[off++] & 0xFF;        if (off == l) break;
          case 16: w4   = (in[off++]       ) << 24; if (off == l) break;
          case 17: w4  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 18: w4  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 19: w4  |=  in[off++] & 0xFF;        if (off == l) break;
          case 20: w5   = (in[off++]       ) << 24; if (off == l) break;
          case 21: w5  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 22: w5  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 23: w5  |=  in[off++] & 0xFF;        if (off == l) break;
          case 24: w6   = (in[off++]       ) << 24; if (off == l) break;
          case 25: w6  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 26: w6  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 27: w6  |=  in[off++] & 0xFF;        if (off == l) break;
          case 28: w7   = (in[off++]       ) << 24; if (off == l) break;
          case 29: w7  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 30: w7  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 31: w7  |=  in[off++] & 0xFF;        if (off == l) break;
          case 32: w8   = (in[off++]       ) << 24; if (off == l) break;
          case 33: w8  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 34: w8  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 35: w8  |=  in[off++] & 0xFF;        if (off == l) break;
          case 36: w9   = (in[off++]       ) << 24; if (off == l) break;
          case 37: w9  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 38: w9  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 39: w9  |=  in[off++] & 0xFF;        if (off == l) break;
          case 40: w10  = (in[off++]       ) << 24; if (off == l) break;
          case 41: w10 |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 42: w10 |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 43: w10 |=  in[off++] & 0xFF;        if (off == l) break;
          case 44: w11  = (in[off++]       ) << 24; if (off == l) break;
          case 45: w11 |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 46: w11 |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 47: w11 |=  in[off++] & 0xFF;        if (off == l) break;
          case 48: w12  = (in[off++]       ) << 24; if (off == l) break;
          case 49: w12 |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 50: w12 |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 51: w12 |=  in[off++] & 0xFF;        if (off == l) break;
          case 52: w13  = (in[off++]       ) << 24; if (off == l) break;
          case 53: w13 |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 54: w13 |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 55: w13 |=  in[off++] & 0xFF;        if (off == l) break;
          case 56: w14  = (in[off++]       ) << 24; if (off == l) break;
          case 57: w14 |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 58: w14 |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 59: w14 |=  in[off++] & 0xFF;        if (off == l) break;
          case 60: w15  = (in[off++]       ) << 24; if (off == l) break;
          case 61: w15 |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 62: w15 |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 63: w15 |=  in[off++] & 0xFF; break;
          default: throw new Error("impossible value: " + k);
          }
        if ((count & 63) == 0)
          transform();
      }
  }

  protected byte[] engineDigest()
  {
    byte[] digest = new byte[20];
    engineDigest(digest, 0, 20);
    return digest;
  }

  protected int engineDigest(byte[] out, int off, int len)
  {
    pad();
    if (off < 0 || len < 0 || 20 + off > out.length)
      throw new ArrayIndexOutOfBoundsException();
    final int l = Math.min(len, 20);
    final int k = off + l;
    do
      {
        out[off++] = (byte) (a >>> 24); if (off == k) break;
        out[off++] = (byte) (a >>> 16); if (off == k) break;
        out[off++] = (byte) (a >>>  8); if (off == k) break;
        out[off++] = (byte)  a;         if (off == k) break;
        out[off++] = (byte) (b >>> 24); if (off == k) break;
        out[off++] = (byte) (b >>> 16); if (off == k) break;
        out[off++] = (byte) (b >>>  8); if (off == k) break;
        out[off++] = (byte)  b;         if (off == k) break;
        out[off++] = (byte) (c >>> 24); if (off == k) break;
        out[off++] = (byte) (c >>> 16); if (off == k) break;
        out[off++] = (byte) (c >>>  8); if (off == k) break;
        out[off++] = (byte)  c;         if (off == k) break;
        out[off++] = (byte) (d >>> 24); if (off == k) break;
        out[off++] = (byte) (d >>> 16); if (off == k) break;
        out[off++] = (byte) (d >>>  8); if (off == k) break;
        out[off++] = (byte)  d;         if (off == k) break;
        out[off++] = (byte) (e >>> 24); if (off == k) break;
        out[off++] = (byte) (e >>> 16); if (off == k) break;
        out[off++] = (byte) (e >>>  8); if (off == k) break;
        out[off  ] = (byte)  e;
      }
    while (false);
    engineReset();
    return l;
  }

  protected void engineReset()
  {
    count = 0L;
    a = 0x67452301;
    b = 0xEFCDAB89;
    c = 0x98BADCFE;
    d = 0x10325476;
    e = 0xC3D2E1F0;
  }

  public Object clone() throws CloneNotSupportedException
  {
    return super.clone();
  }

  // Own methods.
  // -------------------------------------------------------------------------

  private void pad()
  {
    final int n = (int) (count % 64);
    int len = (n < 56) ? (56 - n) : (120 - n);
    final byte[] pad = new byte[len + 8];

    pad[0] = (byte) 0x80;
    final long bits = count << 3;
    pad[len++] = (byte) (bits >>> 56);
    pad[len++] = (byte) (bits >>> 48);
    pad[len++] = (byte) (bits >>> 40);
    pad[len++] = (byte) (bits >>> 32);
    pad[len++] = (byte) (bits >>> 24);
    pad[len++] = (byte) (bits >>> 16);
    pad[len++] = (byte) (bits >>>  8);
    pad[len  ] = (byte)  bits;
    engineUpdate(pad, 0, pad.length);
  }

  private void transform()
  {
    int A = a;
    int B = b;
    int C = c;
    int D = d;
    int E = e;
    int T;

    T = w13 ^ w8 ^ w2 ^ w0;
    w16 = T << 1 | T >>> 31;
    T = w14 ^ w9 ^ w3 ^ w1;
    w17 = T << 1 | T >>> 31;
    T = w15 ^ w10 ^ w4 ^ w2;
    w18 = T << 1 | T >>> 31;
    T = w16 ^ w11 ^ w5 ^ w3;
    w19 = T << 1 | T >>> 31;
    T = w17 ^ w12 ^ w6 ^ w4;
    w20 = T << 1 | T >>> 31;
    T = w18 ^ w13 ^ w7 ^ w5;
    w21 = T << 1 | T >>> 31;
    T = w19 ^ w14 ^ w8 ^ w6;
    w22 = T << 1 | T >>> 31;
    T = w20 ^ w15 ^ w9 ^ w7;
    w23 = T << 1 | T >>> 31;
    T = w21 ^ w16 ^ w10 ^ w8;
    w24 = T << 1 | T >>> 31;
    T = w22 ^ w17 ^ w11 ^ w9;
    w25 = T << 1 | T >>> 31;
    T = w23 ^ w18 ^ w12 ^ w10;
    w26 = T << 1 | T >>> 31;
    T = w24 ^ w19 ^ w13 ^ w11;
    w27 = T << 1 | T >>> 31;
    T = w25 ^ w20 ^ w14 ^ w12;
    w28 = T << 1 | T >>> 31;
    T = w26 ^ w21 ^ w15 ^ w13;
    w29 = T << 1 | T >>> 31;
    T = w27 ^ w22 ^ w16 ^ w14;
    w30 = T << 1 | T >>> 31;
    T = w28 ^ w23 ^ w17 ^ w15;
    w31 = T << 1 | T >>> 31;
    T = w29 ^ w24 ^ w18 ^ w16;
    w32 = T << 1 | T >>> 31;
    T = w30 ^ w25 ^ w19 ^ w17;
    w33 = T << 1 | T >>> 31;
    T = w31 ^ w26 ^ w20 ^ w18;
    w34 = T << 1 | T >>> 31;
    T = w32 ^ w27 ^ w21 ^ w19;
    w35 = T << 1 | T >>> 31;
    T = w33 ^ w28 ^ w22 ^ w20;
    w36 = T << 1 | T >>> 31;
    T = w34 ^ w29 ^ w23 ^ w21;
    w37 = T << 1 | T >>> 31;
    T = w35 ^ w30 ^ w24 ^ w22;
    w38 = T << 1 | T >>> 31;
    T = w36 ^ w31 ^ w25 ^ w23;
    w39 = T << 1 | T >>> 31;
    T = w37 ^ w32 ^ w26 ^ w24;
    w40 = T << 1 | T >>> 31;
    T = w38 ^ w33 ^ w27 ^ w25;
    w41 = T << 1 | T >>> 31;
    T = w39 ^ w34 ^ w28 ^ w26;
    w42 = T << 1 | T >>> 31;
    T = w40 ^ w35 ^ w29 ^ w27;
    w43 = T << 1 | T >>> 31;
    T = w41 ^ w36 ^ w30 ^ w28;
    w44 = T << 1 | T >>> 31;
    T = w42 ^ w37 ^ w31 ^ w29;
    w45 = T << 1 | T >>> 31;
    T = w43 ^ w38 ^ w32 ^ w30;
    w46 = T << 1 | T >>> 31;
    T = w44 ^ w39 ^ w33 ^ w31;
    w47 = T << 1 | T >>> 31;
    T = w45 ^ w40 ^ w34 ^ w32;
    w48 = T << 1 | T >>> 31;
    T = w46 ^ w41 ^ w35 ^ w33;
    w49 = T << 1 | T >>> 31;
    T = w47 ^ w42 ^ w36 ^ w34;
    w50 = T << 1 | T >>> 31;
    T = w48 ^ w43 ^ w37 ^ w35;
    w51 = T << 1 | T >>> 31;
    T = w49 ^ w44 ^ w38 ^ w36;
    w52 = T << 1 | T >>> 31;
    T = w50 ^ w45 ^ w39 ^ w37;
    w53 = T << 1 | T >>> 31;
    T = w51 ^ w46 ^ w40 ^ w38;
    w54 = T << 1 | T >>> 31;
    T = w52 ^ w47 ^ w41 ^ w39;
    w55 = T << 1 | T >>> 31;
    T = w53 ^ w48 ^ w42 ^ w40;
    w56 = T << 1 | T >>> 31;
    T = w54 ^ w49 ^ w43 ^ w41;
    w57 = T << 1 | T >>> 31;
    T = w55 ^ w50 ^ w44 ^ w42;
    w58 = T << 1 | T >>> 31;
    T = w56 ^ w51 ^ w45 ^ w43;
    w59 = T << 1 | T >>> 31;
    T = w57 ^ w52 ^ w46 ^ w44;
    w60 = T << 1 | T >>> 31;
    T = w58 ^ w53 ^ w47 ^ w45;
    w61 = T << 1 | T >>> 31;
    T = w59 ^ w54 ^ w48 ^ w46;
    w62 = T << 1 | T >>> 31;
    T = w60 ^ w55 ^ w49 ^ w47;
    w63 = T << 1 | T >>> 31;
    T = w61 ^ w56 ^ w50 ^ w48;
    w64 = T << 1 | T >>> 31;
    T = w62 ^ w57 ^ w51 ^ w49;
    w65 = T << 1 | T >>> 31;
    T = w63 ^ w58 ^ w52 ^ w50;
    w66 = T << 1 | T >>> 31;
    T = w64 ^ w59 ^ w53 ^ w51;
    w67 = T << 1 | T >>> 31;
    T = w65 ^ w60 ^ w54 ^ w52;
    w68 = T << 1 | T >>> 31;
    T = w66 ^ w61 ^ w55 ^ w53;
    w69 = T << 1 | T >>> 31;
    T = w67 ^ w62 ^ w56 ^ w54;
    w70 = T << 1 | T >>> 31;
    T = w68 ^ w63 ^ w57 ^ w55;
    w71 = T << 1 | T >>> 31;
    T = w69 ^ w64 ^ w58 ^ w56;
    w72 = T << 1 | T >>> 31;
    T = w70 ^ w65 ^ w59 ^ w57;
    w73 = T << 1 | T >>> 31;
    T = w71 ^ w66 ^ w60 ^ w58;
    w74 = T << 1 | T >>> 31;
    T = w72 ^ w67 ^ w61 ^ w59;
    w75 = T << 1 | T >>> 31;
    T = w73 ^ w68 ^ w62 ^ w60;
    w76 = T << 1 | T >>> 31;
    T = w74 ^ w69 ^ w63 ^ w61;
    w77 = T << 1 | T >>> 31;
    T = w75 ^ w70 ^ w64 ^ w62;
    w78 = T << 1 | T >>> 31;
    T = w76 ^ w71 ^ w65 ^ w63;
    w79 = T << 1 | T >>> 31;

    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w0 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w1 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w2 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w3 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w4 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w5 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w6 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w7 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w8 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w9 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w10 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w11 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w12 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w13 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w14 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w15 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w16 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w17 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w18 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + ((B & C) | (~B & D)) + E + w19 + 0x5A827999;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;

    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w20 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w21 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w22 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w23 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w24 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w25 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w26 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w27 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w28 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w29 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w30 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w31 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w32 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w33 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w34 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w35 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w36 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w37 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w38 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w39 + 0x6ED9EBA1;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;

    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w40 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w41 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w42 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w43 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w44 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w45 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w46 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w47 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w48 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w49 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w50 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w51 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w52 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w53 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w54 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w55 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w56 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w57 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w58 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B & C | B & D | C & D) + E + w59 + 0x8F1BBCDC;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;

    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w60 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w61 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w62 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w63 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w64 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w65 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w66 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w67 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w68 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w69 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w70 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w71 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w72 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w73 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w74 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w75 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w76 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w77 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w78 + 0xCA62C1D6;
    E = D;
    D = C;
    C = B << 30 | B >>> 2;
    B = A;
    A = T;
    T = (A << 5 | A >>> 27) + (B ^ C ^ D) + E + w79 + 0xCA62C1D6;
    E = D;
    D = C;

    a += T;
    b += A;
    c += B << 30 | B >>> 2;
    d += D;
    e += E;
  }
}
