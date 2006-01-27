/* GnuDHPublicKey.java -- 
   Copyright (C) 2003, 2006 Free Software Foundation, Inc.

This file is a part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; if not, write to the Free Software
Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301
USA

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
exception statement from your version.  */


package gnu.javax.crypto.key.dh;

import gnu.java.security.Registry;
import gnu.java.security.key.IKeyPairCodec;

import java.math.BigInteger;

import javax.crypto.interfaces.DHPublicKey;

/**
 * <p>An implementation of the Diffie-Hellman public key.</p>
 *
 * <p>Reference:</p>
 * <ol>
 *    <li><a href="http://www.ietf.org/rfc/rfc2631.txt">Diffie-Hellman Key
 *    Agreement Method</a><br>
 *    Eric Rescorla.</li>
 * </ol>
 */
public class GnuDHPublicKey extends GnuDHKey implements DHPublicKey
{

  // Constants and variables
  // -------------------------------------------------------------------------

  private BigInteger y;

  // Constructor(s)
  // -------------------------------------------------------------------------

  public GnuDHPublicKey(BigInteger q, BigInteger p, BigInteger g, BigInteger y)
  {
    super(q, p, g);

    this.y = y;
  }

  // Class methods
  // -------------------------------------------------------------------------

  /**
   * <p>A class method that takes the output of the <code>encodePublicKey()</code>
   * method of a DH keypair codec object (an instance implementing
   * {@link IKeyPairCodec} for DSS keys, and re-constructs an instance of this
   * object.</p>
   *
   * @param k the contents of a previously encoded instance of this object.
   * @exception ArrayIndexOutOfBoundsException if there is not enough bytes,
   * in <code>k</code>, to represent a valid encoding of an instance of this
   * object.
   * @exception IllegalArgumentException if the byte sequence does not
   * represent a valid encoding of an instance of this object.
   */
  public static GnuDHPublicKey valueOf(byte[] k)
  {
    // check magic...
    // we should parse here enough bytes to know which codec to use, and
    // direct the byte array to the appropriate codec.  since we only have one
    // codec, we could have immediately tried it; nevertheless since testing
    // one byte is cheaper than instatiating a codec that will fail we test
    // the first byte before we carry on.
    if (k[0] == Registry.MAGIC_RAW_DH_PUBLIC_KEY[0])
      {
        // it's likely to be in raw format. get a raw codec and hand it over
        IKeyPairCodec codec = new DHKeyPairRawCodec();
        return (GnuDHPublicKey) codec.decodePublicKey(k);
      }
    else
      {
        throw new IllegalArgumentException("magic");
      }
  }

  // Instance methods
  // -------------------------------------------------------------------------

  // java.security.Key interface implementation ------------------------------

  /** @deprecated see getEncoded(int). */
  public byte[] getEncoded()
  {
    return getEncoded(IKeyPairCodec.RAW_FORMAT);
  }

  // javax.crypto.interfaces.DHPublicKey interface implementation ------------

  public BigInteger getY()
  {
    return y;
  }

  // other methods -----------------------------------------------------------

  /**
   * <p>Returns the encoded form of this public key according to the designated
   * format.</p>
   *
   * @param format the desired format identifier of the resulting encoding.
   * @return the byte sequence encoding this key according to the designated
   * format.
   * @exception IllegalArgumentException if the format is not supported.
   * @see gnu.crypto.key.dh.DHKeyPairRawCodec
   */
  public byte[] getEncoded(int format)
  {
    byte[] result;
    switch (format)
      {
      case IKeyPairCodec.RAW_FORMAT:
        result = new DHKeyPairRawCodec().encodePublicKey(this);
        break;
      default:
        throw new IllegalArgumentException("format");
      }
    return result;
  }
}
