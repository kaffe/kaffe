/* MD2.java -- The MD2 message digest.
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
import java.util.Arrays;

public final class MD2 extends MessageDigestSpi
{

  // Constants and fields.
  // -------------------------------------------------------------------------

  /** 256 byte "random" permutation of the digits of pi. */
  private static final byte[] PI =
  {
    41, 46, 67, -55, -94, -40, 124, 1, 61, 54, 84, -95, -20, -16, 6,
    19, 98, -89, 5, -13, -64, -57, 115, -116, -104, -109, 43, -39,
    -68, 76, -126, -54, 30, -101, 87, 60, -3, -44, -32, 22, 103, 66,
    111, 24, -118, 23, -27, 18, -66, 78, -60, -42, -38, -98, -34, 73,
    -96, -5, -11, -114, -69, 47, -18, 122, -87, 104, 121, -111, 21,
    -78, 7, 63, -108, -62, 16, -119, 11, 34, 95, 33, -128, 127, 93,
    -102, 90, -112, 50, 39, 53, 62, -52, -25, -65, -9, -105, 3, -1,
    25, 48, -77, 72, -91, -75, -47, -41, 94, -110, 42, -84, 86, -86,
    -58, 79, -72, 56, -46, -106, -92, 125, -74, 118, -4, 107, -30,
    -100, 116, 4, -15, 69, -99, 112, 89, 100, 113, -121, 32, -122,
    91, -49, 101, -26, 45, -88, 2, 27, 96, 37, -83, -82, -80, -71,
    -10, 28, 70, 97, 105, 52, 64, 126, 15, 85, 71, -93, 35, -35, 81,
    -81, 58, -61, 92, -7, -50, -70, -59, -22, 38, 44, 83, 13, 110,
    -123, 40, -124, 9, -45, -33, -51, -12, 65, -127, 77, 82, 106,
    -36, 55, -56, 108, -63, -85, -6, 36, -31, 123, 8, 12, -67, -79,
    74, 120, -120, -107, -117, -29, 99, -24, 109, -23, -53, -43, -2,
    59, 0, 29, 57, -14, -17, -73, 14, 102, 88, -48, -28, -90, 119,
    114, -8, -21, 117, 75, 10, 49, 68, 80, -76, -113, -19, 31, 26,
    -37, -103, -115, 51, -97, 17, -125, 20
  };

  private long count;
  private final byte[] checksum;
  private final byte[] work;
  private final byte[] input;
  private int index;

  // Constructor.
  // -------------------------------------------------------------------------

  public MD2()
  {
    super();
    checksum = new byte[16];
    work = new byte[48];
    input = new byte[16];
    index = 0;
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
    work[index++] = b;
    if (index == 16)
      {
        encryptAndChecksum();
        index = 0;
      }
  }

  protected void engineUpdate(final byte[] in, int off, final int len)
  {
    final int l = off + len;
    while (off < l)
      {
        int i = Math.min(16 - index, l - off);
        System.arraycopy(in, off, input, index, i);
        off += i;
        index += i;
        count += i;
        if (index == 16)
          {
            encryptAndChecksum();
            index = 0;
          }
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
    final int padlen = 16 - (index & 15);
    Arrays.fill(input, index, 16, (byte) padlen);
    encryptAndChecksum();

    for (int i = 0; i < 16; i++)
      {
        byte b = checksum[i];
        work[16+i] = b;
        work[32+i] = (byte)(work[i] ^ b);
      }

    byte t = 0;
    for (int i = 0; i < 18; i++)
      {
        for (int j = 0; j < 48; j++)
          {
            t = (byte)(work[j] ^ PI[t & 0xFF]);
            work[j] = t;
          }
        t = (byte)(t + i);
      }

    final int l = Math.min(16, len);
    System.arraycopy(work, 0, out, off, l);
    engineReset();
    return l;
  }

  protected void engineReset()
  {
    Arrays.fill(checksum, (byte) 0);
    Arrays.fill(work, (byte) 0);
    Arrays.fill(input, (byte) 0);
    index = 0;
  }

  public Object clone() throws CloneNotSupportedException
  {
    return super.clone();
  }

  // Own methods.
  // -------------------------------------------------------------------------

  private void encryptAndChecksum()
  {
    byte l = checksum[15];
    for (int i = 0; i < 16; i++)
      {
        byte b = input[i];
        work[16+i] = b;
        work[32+i] = (byte)(work[i] ^ b);
        l = (byte)(checksum[i] ^ PI[(b ^ l) & 0xFF]);
        checksum[i] = l;
      }

    byte t = 0;
    for (int i = 0; i < 18; i++)
      {
        for (int j = 0; j < 48; j++)
          {
            t = (byte)(work[j] ^ PI[t & 0xFF]);
            work[j] = t;
          }
        t = (byte)(t + i);
      }
  }
}
