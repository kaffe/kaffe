/* MD5.java -- The MD5 message digest.
   Copyright (C) 2003  Casey Marshall <csm@metastatic.org>

This file is part of GNU PKI, a PKI library.

GNU PKI is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU PKI is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GNU PKI; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
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

import java.security.MessageDigestSpi;

public final class MD5 extends MessageDigestSpi implements Cloneable
{

  // Fields.
  // -------------------------------------------------------------------------

  private long count;
  private int a, b, c, d;
  private int w0, w1, w2, w3, w4, w5, w6, w7, w8, w9, w10, w11, w12, w13, w14, w15;
  private final byte[] one = new byte[1];

  // Constructor.
  // -------------------------------------------------------------------------

  public MD5()
  {
    super();
    engineReset();
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  protected int engineGetDigestLength()
  {
    return 16;
  }

  protected void engineUpdate(final byte b)
  {
    one[0] = b;
    engineUpdate(one, 0, 1);
  }

  protected void engineUpdate(final byte[] in, int off, final int len)
  {
    final int l = off + len;
    while (off < l)
      {
        int k = (int) (count & 63);
        count += Math.min(64 - k, l - off);
        switch (k)
          {
          case  0: w0   =  in[off++] & 0xFF;        if (off == l) break;
          case  1: w0  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case  2: w0  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case  3: w0  |=  in[off++]         << 24; if (off == l) break;
          case  4: w1   =  in[off++] & 0xFF;        if (off == l) break;
          case  5: w1  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case  6: w1  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case  7: w1  |=  in[off++]         << 24; if (off == l) break;
          case  8: w2   =  in[off++] & 0xFF;        if (off == l) break;
          case  9: w2  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 10: w2  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 11: w2  |=  in[off++]         << 24; if (off == l) break;
          case 12: w3   =  in[off++] & 0xFF;        if (off == l) break;
          case 13: w3  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 14: w3  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 15: w3  |=  in[off++]         << 24; if (off == l) break;
          case 16: w4   =  in[off++] & 0xFF;        if (off == l) break;
          case 17: w4  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 18: w4  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 19: w4  |=  in[off++]         << 24; if (off == l) break;
          case 20: w5   =  in[off++] & 0xFF;        if (off == l) break;
          case 21: w5  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 22: w5  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 23: w5  |=  in[off++]         << 24; if (off == l) break;
          case 24: w6   =  in[off++] & 0xFF;        if (off == l) break;
          case 25: w6  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 26: w6  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 27: w6  |=  in[off++]         << 24; if (off == l) break;
          case 28: w7   =  in[off++] & 0xFF;        if (off == l) break;
          case 29: w7  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 30: w7  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 31: w7  |=  in[off++]         << 24; if (off == l) break;
          case 32: w8   =  in[off++] & 0xFF;        if (off == l) break;
          case 33: w8  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 34: w8  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 35: w8  |=  in[off++]         << 24; if (off == l) break;
          case 36: w9   =  in[off++] & 0xFF;        if (off == l) break;
          case 37: w9  |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 38: w9  |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 39: w9  |=  in[off++]         << 24; if (off == l) break;
          case 40: w10  =  in[off++] & 0xFF;        if (off == l) break;
          case 41: w10 |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 42: w10 |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 43: w10 |=  in[off++]         << 24; if (off == l) break;
          case 44: w11  =  in[off++] & 0xFF;        if (off == l) break;
          case 45: w11 |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 46: w11 |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 47: w11 |=  in[off++]         << 24; if (off == l) break;
          case 48: w12  =  in[off++] & 0xFF;        if (off == l) break;
          case 49: w12 |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 50: w12 |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 51: w12 |=  in[off++]         << 24; if (off == l) break;
          case 52: w13  =  in[off++] & 0xFF;        if (off == l) break;
          case 53: w13 |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 54: w13 |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 55: w13 |=  in[off++]         << 24; if (off == l) break;
          case 56: w14  =  in[off++] & 0xFF;        if (off == l) break;
          case 57: w14 |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 58: w14 |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 59: w14 |=  in[off++]         << 24; if (off == l) break;
          case 60: w15  =  in[off++] & 0xFF;        if (off == l) break;
          case 61: w15 |= (in[off++] & 0xFF) <<  8; if (off == l) break;
          case 62: w15 |= (in[off++] & 0xFF) << 16; if (off == l) break;
          case 63: w15 |=  in[off++] << 24; break;
          default: throw new Error("impossible value: " + k);
          }
        if ((count & 63) == 0)
          transform();
      }
  }

  protected byte[] engineDigest()
  {
    final byte[] digest = new byte[16];
    engineDigest(digest, 0, 16);
    return digest;
  }

  protected int engineDigest(final byte[] out, int off, final int len)
  {
    pad();
    if (off < 0 || len < 0 || 16 + off > out.length)
      throw new ArrayIndexOutOfBoundsException();
    final int l = Math.min(len, 16);
    final int k = off + l;
    do
      {
        out[off++] = (byte)  a;         if (off == k) break;
        out[off++] = (byte) (a >>>  8); if (off == k) break;
        out[off++] = (byte) (a >>> 16); if (off == k) break;
        out[off++] = (byte) (a >>> 24); if (off == k) break;
        out[off++] = (byte)  b;         if (off == k) break;
        out[off++] = (byte) (b >>>  8); if (off == k) break;
        out[off++] = (byte) (b >>> 16); if (off == k) break;
        out[off++] = (byte) (b >>> 24); if (off == k) break;
        out[off++] = (byte)  c;         if (off == k) break;
        out[off++] = (byte) (c >>>  8); if (off == k) break;
        out[off++] = (byte) (c >>> 16); if (off == k) break;
        out[off++] = (byte) (c >>> 24); if (off == k) break;
        out[off++] = (byte)  d;         if (off == k) break;
        out[off++] = (byte) (d >>>  8); if (off == k) break;
        out[off++] = (byte) (d >>> 16); if (off == k) break;
        out[off  ] = (byte) (d >>> 24);
      }
    while (false);
    engineReset();
    return l;
  }

  protected void engineReset()
  {
    a = 0x67452301;
    b = 0xEFCDAB89;
    c = 0x98BADCFE;
    d = 0x10325476;
    count = 0;
  }

  public Object clone() throws CloneNotSupportedException
  {
    return super.clone();
  }

  // Own methods.
  // -------------------------------------------------------------------------

  private void pad()
  {
    final int n = (int) (count & 63);
    int len = (n < 56) ? (56 - n) : (120 - n);
    final byte[] pad = new byte[len + 8];

    pad[0] = (byte) 0x80;
    final long bits = count << 3;
    pad[len++] = (byte)  bits;
    pad[len++] = (byte) (bits >>>  8);
    pad[len++] = (byte) (bits >>> 16);
    pad[len++] = (byte) (bits >>> 24);
    pad[len++] = (byte) (bits >>> 32);
    pad[len++] = (byte) (bits >>> 40);
    pad[len++] = (byte) (bits >>> 48);
    pad[len  ] = (byte) (bits >>> 56);
    engineUpdate(pad, 0, pad.length);
  }

  private void transform()
  {
    int A = a;
    int B = b;
    int C = c;
    int D = d;

    // round 1
    A += ((B & C) | (~B & D)) + w0 + 0xD76AA478;  A = B + (A << 7 | A >>> (32 - 7));
    D += ((A & B) | (~A & C)) + w1 + 0xE8C7B756;  D = A + (D << 12 | D >>> (32 - 12));
    C += ((D & A) | (~D & B)) + w2 + 0x242070DB;  C = D + (C << 17 | C >>> (32 - 17));
    B += ((C & D) | (~C & A)) + w3 + 0xC1BDCEEE;  B = C + (B << 22 | B >>> (32 - 22));

    A += ((B & C) | (~B & D)) + w4 + 0xF57C0FAF;  A = B + (A << 7 | A >>> (32 - 7));
    D += ((A & B) | (~A & C)) + w5 + 0x4787C62A;  D = A + (D << 12 | D >>> (32 - 12));
    C += ((D & A) | (~D & B)) + w6 + 0xA8304613;  C = D + (C << 17 | C >>> (32 - 17));
    B += ((C & D) | (~C & A)) + w7 + 0xFD469501;  B = C + (B << 22 | B >>> (32 - 22));

    A += ((B & C) | (~B & D)) + w8 + 0x698098D8;  A = B + (A << 7 | A >>> (32 - 7));
    D += ((A & B) | (~A & C)) + w9 + 0x8B44F7AF;  D = A + (D << 12 | D >>> (32 - 12));
    C += ((D & A) | (~D & B)) + w10 + 0xFFFF5BB1; C = D + (C << 17 | C >>> (32 - 17));
    B += ((C & D) | (~C & A)) + w11 + 0x895CD7BE; B = C + (B << 22 | B >>> (32 - 22));

    A += ((B & C) | (~B & D)) + w12 + 0x6B901122; A = B + (A << 7 | A >>> (32 - 7));
    D += ((A & B) | (~A & C)) + w13 + 0xFD987193; D = A + (D << 12 | D >>> (32 - 12));
    C += ((D & A) | (~D & B)) + w14 + 0xA679438E; C = D + (C << 17 | C >>> (32 - 17));
    B += ((C & D) | (~C & A)) + w15 + 0x49B40821; B = C + (B << 22 | B >>> (32 - 22));

    // round 2
    A += ((B & D) | (C & ~D)) + w1 + 0xF61E2562;  A = B + (A << 5 | A >>> (32 - 5));
    D += ((A & C) | (B & ~C)) + w6 + 0xC040B340;  D = A + (D << 9 | D >>> (32 - 9));
    C += ((D & B) | (A & ~B)) + w11 + 0x265E5A51; C = D + (C << 14 | C >>> (32 - 14));
    B += ((C & A) | (D & ~A)) + w0 + 0xE9B6C7AA;  B = C + (B << 20 | B >>> (32 - 20));

    A += ((B & D) | (C & ~D)) + w5 + 0xD62F105D;  A = B + (A << 5 | A >>> (32 - 5));
    D += ((A & C) | (B & ~C)) + w10 + 0x02441453; D = A + (D << 9 | D >>> (32 - 9));
    C += ((D & B) | (A & ~B)) + w15 + 0xD8A1E681; C = D + (C << 14 | C >>> (32 - 14));
    B += ((C & A) | (D & ~A)) + w4 + 0xE7D3FBC8;  B = C + (B << 20 | B >>> (32 - 20));

    A += ((B & D) | (C & ~D)) + w9 + 0x21E1CDE6;  A = B + (A << 5 | A >>> (32 - 5));
    D += ((A & C) | (B & ~C)) + w14 + 0xC33707D6; D = A + (D << 9 | D >>> (32 - 9));
    C += ((D & B) | (A & ~B)) + w3 + 0xF4D50D87;  C = D + (C << 14 | C >>> (32 - 14));
    B += ((C & A) | (D & ~A)) + w8 + 0x455A14ED;  B = C + (B << 20 | B >>> (32 - 20));

    A += ((B & D) | (C & ~D)) + w13 + 0xA9E3E905; A = B + (A << 5 | A >>> (32 - 5));
    D += ((A & C) | (B & ~C)) + w2 + 0xFCEFA3F8;  D = A + (D << 9 | D >>> (32 - 9));
    C += ((D & B) | (A & ~B)) + w7 + 0x676F02D9;  C = D + (C << 14 | C >>> (32 - 14));
    B += ((C & A) | (D & ~A)) + w12 + 0x8D2A4C8A; B = C + (B << 20 | B >>> (32 - 20));

    // round 3
    A += (B ^ C ^ D) + w5 + 0xFFFA3942;  A = B + (A << 4 | A >>> (32 - 4));
    D += (A ^ B ^ C) + w8 + 0x8771F681;  D = A + (D << 11 | D >>> (32 - 11));
    C += (D ^ A ^ B) + w11 + 0x6D9D6122; C = D + (C << 16 | C >>> (32 - 16));
    B += (C ^ D ^ A) + w14 + 0xFDE5380C; B = C + (B << 23 | B >>> (32 - 23));

    A += (B ^ C ^ D) + w1 + 0xA4BEEA44;  A = B + (A << 4 | A >>> (32 - 4));
    D += (A ^ B ^ C) + w4 + 0x4BDECFA9;  D = A + (D << 11 | D >>> (32 - 11));
    C += (D ^ A ^ B) + w7 + 0xF6BB4B60;  C = D + (C << 16 | C >>> (32 - 16));
    B += (C ^ D ^ A) + w10 + 0xBEBFBC70; B = C + (B << 23 | B >>> (32 - 23));

    A += (B ^ C ^ D) + w13 + 0x289B7EC6; A = B + (A << 4 | A >>> (32 - 4));
    D += (A ^ B ^ C) + w0 + 0xEAA127FA;  D = A + (D << 11 | D >>> (32 - 11));
    C += (D ^ A ^ B) + w3 + 0xD4EF3085;  C = D + (C << 16 | C >>> (32 - 16));
    B += (C ^ D ^ A) + w6 + 0x04881D05;  B = C + (B << 23 | B >>> (32 - 23));

    A += (B ^ C ^ D) + w9 + 0xD9D4D039;  A = B + (A << 4 | A >>> (32 - 4));
    D += (A ^ B ^ C) + w12 + 0xE6DB99E5; D = A + (D << 11 | D >>> (32 - 11));
    C += (D ^ A ^ B) + w15 + 0x1FA27CF8; C = D + (C << 16 | C >>> (32 - 16));
    B += (C ^ D ^ A) + w2 + 0xC4AC5665;  B = C + (B << 23 | B >>> (32 - 23));

    // round 4
    A += (C ^ (B | ~D)) + w0 + 0xF4292244;  A = B + (A << 6 | A >>> (32 - 6));
    D += (B ^ (A | ~C)) + w7 + 0x432AFF97;  D = A + (D << 10 | D >>> (32 - 10));
    C += (A ^ (D | ~B)) + w14 + 0xAB9423A7; C = D + (C << 15 | C >>> (32 - 15));
    B += (D ^ (C | ~A)) + w5 + 0xFC93A039;  B = C + (B << 21 | B >>> (32 - 21));

    A += (C ^ (B | ~D)) + w12 + 0x655B59C3; A = B + (A << 6 | A >>> (32 - 6));
    D += (B ^ (A | ~C)) + w3 + 0x8F0CCC92;  D = A + (D << 10 | D >>> (32 - 10));
    C += (A ^ (D | ~B)) + w10 + 0xFFEFF47D; C = D + (C << 15 | C >>> (32 - 15));
    B += (D ^ (C | ~A)) + w1 + 0x85845dd1;  B = C + (B << 21 | B >>> (32 - 21));

    A += (C ^ (B | ~D)) + w8 + 0x6FA87E4F;  A = B + (A << 6 | A >>> (32 - 6));
    D += (B ^ (A | ~C)) + w15 + 0xFE2CE6E0; D = A + (D << 10 | D >>> (32 - 10));
    C += (A ^ (D | ~B)) + w6 + 0xA3014314;  C = D + (C << 15 | C >>> (32 - 15));
    B += (D ^ (C | ~A)) + w13 + 0x4E0811A1; B = C + (B << 21 | B >>> (32 - 21));

    A += (C ^ (B | ~D)) + w4 + 0xF7537E82;  A = B + (A << 6 | A >>> (32 - 6));
    D += (B ^ (A | ~C)) + w11 + 0xBD3AF235; D = A + (D << 10 | D >>> (32 - 10));
    C += (A ^ (D | ~B)) + w2 + 0x2AD7D2BB;  C = D + (C << 15 | C >>> (32 - 15));
    B += (D ^ (C | ~A)) + w9 + 0xEB86D391;  B = C + (B << 21 | B >>> (32 - 21));

    a += A;
    b += B;
    c += C;
    d += D;
  }
}
