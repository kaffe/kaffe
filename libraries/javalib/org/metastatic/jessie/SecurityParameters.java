/* SecurityParameters.java -- SSL security parameters.
   Copyright (C) 2003  Casey Marshall <rsdio@metastatic.org>

This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

This program is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the

   Free Software Foundation, Inc.,
   59 Temple Place, Suite 330,
   Boston, MA  02111-1307
   USA

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under terms
of your choice, provided that you also meet, for each linked independent
module, the terms and conditions of the license of that module.  An
independent module is a module which is not derived from or based on
this library.  If you modify this library, you may extend this exception
to your version of the library, but you are not obligated to do so.  If
you do not wish to do so, delete this exception statement from your
version.  */


package org.metastatic.jessie;

import java.security.NoSuchAlgorithmException;
import java.security.SecureRandom;

import javax.net.ssl.SSLException;

import gnu.crypto.mac.IMac;
import gnu.crypto.mode.IMode;
import gnu.crypto.prng.IRandom;
import gnu.crypto.prng.LimitReachedException;

public abstract class SecurityParameters
{

  // Fields.
  // -------------------------------------------------------------------------

  /**
   * The CBC block cipher, if any.
   */
  protected IMode blockCipher;

  /**
   * The RC4 PRNG, if any.
   */
  protected IRandom rc4Random;

  /**
   * The MAC algorithm.
   */
  protected IMac mac;

  protected long sequence;
  protected boolean streamCipher;
  protected boolean amServer;
  protected SecureRandom random;

  // Constructors.
  // -------------------------------------------------------------------------

  public SecurityParameters(boolean amServer)
  {
    sequence = 0L;
    this.amServer = amServer;
    try
      {
        random = SecureRandom.getInstance("SHA1PRNG");
      }
    catch (NoSuchAlgorithmException nsae)
      {
        throw new Error(nsae);
      }
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  public void setBlockCipher(IMode blockCipher)
  {
    this.blockCipher = blockCipher;
    streamCipher = false;
  }

  public void setRandom(IRandom rc4Random)
  {
    this.rc4Random = rc4Random;
    streamCipher = true;
  }

  public void setMac(IMac mac)
  {
    this.mac = mac;
  }

  public abstract Text decryptFragment(Text ciphertext) throws SSLException;

  public abstract Text encryptFragment(Text plaintext) throws SSLException;

  // Own methods.
  // -------------------------------------------------------------------------

  /**
   * Encrypt/decrypt a byte array with the RC4 stream cipher.
   */
  protected void transformRC4(byte[] in, int off, int len,
                              byte[] out, int outOffset)
    throws LimitReachedException
  {
    if (rc4Random == null)
      throw new IllegalStateException();
    if (in == null || out == null)
      throw new NullPointerException();
    if (off < 0 || off + len > in.length ||
        outOffset < 0 || outOffset + len > out.length)
      throw new ArrayIndexOutOfBoundsException();

    for (int i = 0; i < len; i++)
      out[outOffset+i] = (byte) (in[off+i] ^ rc4Random.nextByte());
  }
}
