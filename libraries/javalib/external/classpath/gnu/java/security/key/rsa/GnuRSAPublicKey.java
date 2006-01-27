/* GnuRSAPublicKey.java -- 
   Copyright 2001, 2002, 2003, 2006 Free Software Foundation, Inc.

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


package gnu.java.security.key.rsa;

import gnu.java.security.Registry;
import gnu.java.security.key.IKeyPairCodec;

import java.math.BigInteger;
import java.security.PublicKey;
import java.security.interfaces.RSAPublicKey;

/**
 * <p>An object that encapsulates an RSA public key.</p>
 *
 * <p>References:</p>
 * <ol>
 *    <li><a href="http://www.cosic.esat.kuleuven.ac.be/nessie/workshop/submissions/rsa-pss.zip">
 *    RSA-PSS Signature Scheme with Appendix, part B.</a><br>
 *    Primitive specification and supporting documentation.<br>
 *    Jakob Jonsson and Burt Kaliski.</li>
 * </ol>
 *
 * @version $Revision: 1.1 $
 */
public class GnuRSAPublicKey extends GnuRSAKey implements PublicKey,
    RSAPublicKey
{

  // Constants and variables
  // -------------------------------------------------------------------------

  /** The public exponent of an RSA public key. */
  //   private final BigInteger e;
  // Constructor(s)
  // -------------------------------------------------------------------------
  /**
   * <p>Trivial constructor.</p>
   *
   * @param n the modulus.
   * @param e the public exponent.
   */
  public GnuRSAPublicKey(final BigInteger n, final BigInteger e)
  {
    //      super(n);
    super(n, e);
    //
    //      this.e = e;
  }

  // Class methods
  // -------------------------------------------------------------------------

  /**
   * <p>A class method that takes the output of the <code>encodePublicKey()</code>
   * method of an RSA keypair codec object (an instance implementing
   * {@link gnu.crypto.key.IKeyPairCodec} for RSA keys, and re-constructs an
   * instance of this object.</p>
   *
   * @param k the contents of a previously encoded instance of this object.
   * @throws ArrayIndexOutOfBoundsException if there is not enough bytes, in
   * <code>k</code>, to represent a valid encoding of an instance of this object.
   * @throws IllegalArgumentException if the byte sequence does not represent a
   * valid encoding of an instance of this object.
   */
  public static GnuRSAPublicKey valueOf(final byte[] k)
  {
    // check magic...
    // we should parse here enough bytes to know which codec to use, and
    // direct the byte array to the appropriate codec.  since we only have one
    // codec, we could have immediately tried it; nevertheless since testing
    // one byte is cheaper than instatiating a codec that will fail we test
    // the first byte before we carry on.
    if (k[0] == Registry.MAGIC_RAW_RSA_PUBLIC_KEY[0])
      {
        // it's likely to be in raw format. get a raw codec and hand it over
        final IKeyPairCodec codec = new RSAKeyPairRawCodec();
        return (GnuRSAPublicKey) codec.decodePublicKey(k);
      }
    else
      {
        throw new IllegalArgumentException("magic");
      }
  }

  // Instance methods
  // -------------------------------------------------------------------------

  // java.security.interfaces.RSAPublicKey interface implementation ----------

  //   public BigInteger getPublicExponent() {
  //      return e;
  //   }

  // Other instance methods --------------------------------------------------

  /**
   * <p>Returns the encoded form of this public key according to the designated
   * format.</p>
   *
   * @param format the desired format identifier of the resulting encoding.
   * @return the byte sequence encoding this key according to the designated
   * format.
   * @throws IllegalArgumentException if the format is not supported.
   * @see RSAKeyPairRawCodec
   */
  public byte[] getEncoded(final int format)
  {
    final byte[] result;
    switch (format)
      {
      case IKeyPairCodec.RAW_FORMAT:
        result = new RSAKeyPairRawCodec().encodePublicKey(this);
        break;
      default:
        throw new IllegalArgumentException("format");
      }
    return result;
  }

  /**
   * <p>Returns <code>true</code> if the designated object is an instance of
   * this class and has the same RSA parameter values as this one.</p>
   *
   * @param obj the other non-null RSA key to compare to.
   * @return <code>true</code> if the designated object is of the same type and
   * value as this one.
   */
  public boolean equals(final Object obj)
  {
    if (obj == null)
      {
        return false;
      }
    if (!(obj instanceof RSAPublicKey))
      {
        return false;
      }
    final RSAPublicKey that = (RSAPublicKey) obj;
    //      return super.equals(that) && e.equals(that.getPublicExponent());
    return super.equals(that)
           && getPublicExponent().equals(that.getPublicExponent());
  }
}
