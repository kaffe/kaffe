/* GnuSecurity.java -- GNU Crypto's java.security algorithms.
   Copyright (C) 2004  Free Software Foundation, Inc.

This file is a part of GNU Crypto.

GNU Crypto is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

GNU Crypto is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with GNU Crypto; if not, write to the

   Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor,
   Boston, MA  02110-1301
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


package gnu.crypto.jce;

import gnu.crypto.Registry;
import gnu.crypto.hash.HashFactory;
import gnu.crypto.key.KeyPairGeneratorFactory;
import gnu.crypto.sig.SignatureFactory;

import java.security.AccessController;
import java.security.PrivilegedAction;
import java.security.Provider;
import java.util.Collections;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Set;

/**
 * A provider for classes defined in the <code>java.security</code> package.
 */
public final class GnuSecurity extends Provider
{

  // Constructors.
  // -------------------------------------------------------------------------

  public GnuSecurity()
  {
    super(Registry.GNU_SECURITY, 2.1, "GNU Crypto JCA Provider");

    AccessController.doPrivileged (new PrivilegedAction()
      {
        public Object run()
        {
          // MessageDigest
          put("MessageDigest.HAVAL", gnu.crypto.jce.hash.HavalSpi.class.getName());
          put("MessageDigest.HAVAL ImplementedIn", "Software");
          put("MessageDigest.MD2", gnu.crypto.jce.hash.MD2Spi.class.getName());
          put("MessageDigest.MD2 ImplementedIn", "Software");
          put("MessageDigest.MD4", gnu.crypto.jce.hash.MD4Spi.class.getName());
          put("MessageDigest.MD4 ImplementedIn", "Software");
          put("MessageDigest.MD5", gnu.crypto.jce.hash.MD5Spi.class.getName());
          put("MessageDigest.MD5 ImplementedIn", "Software");
          put("MessageDigest.RIPEMD128", gnu.crypto.jce.hash.RipeMD128Spi.class.getName());
          put("MessageDigest.RIPEMD128 ImplementedIn", "Software");
          put("MessageDigest.RIPEMD160", gnu.crypto.jce.hash.RipeMD160Spi.class.getName());
          put("MessageDigest.RIPEMD160 ImplementedIn", "Software");
          put("MessageDigest.SHA-160", gnu.crypto.jce.hash.Sha160Spi.class.getName());
          put("MessageDigest.SHA-160 ImplementedIn", "Software");
          put("MessageDigest.SHA-256", gnu.crypto.jce.hash.Sha256Spi.class.getName());
          put("MessageDigest.SHA-256 ImplementedIn", "Software");
          put("MessageDigest.SHA-384", gnu.crypto.jce.hash.Sha384Spi.class.getName());
          put("MessageDigest.SHA-384 ImplementedIn", "Software");
          put("MessageDigest.SHA-512", gnu.crypto.jce.hash.Sha512Spi.class.getName());
          put("MessageDigest.SHA-512 ImplementedIn", "Software");
          put("MessageDigest.TIGER", gnu.crypto.jce.hash.TigerSpi.class.getName());
          put("MessageDigest.TIGER ImplementedIn", "Software");
          put("MessageDigest.WHIRLPOOL", gnu.crypto.jce.hash.WhirlpoolSpi.class.getName());
          put("MessageDigest.WHIRLPOOL ImplementedIn", "Software");

          // SecureRandom
          put("SecureRandom.ARCFOUR", gnu.crypto.jce.prng.ARCFourRandomSpi.class.getName());
          put("SecureRandom.ARCFOUR ImplementedIn", "Software");
          put("SecureRandom.CSPRNG", "gnu.crytpo.jce.prng.CSPRNGSpi");
          put("SecureRandom.CSPRNG ImplementedIn", "Software");
          put("SecureRandom.MD2PRNG", gnu.crypto.jce.prng.MD2RandomSpi.class.getName());
          put("SecureRandom.MD2PRNG ImplementedIn", "Software");
          put("SecureRandom.MD4PRNG", gnu.crypto.jce.prng.MD4RandomSpi.class.getName());
          put("SecureRandom.MD4PRNG ImplementedIn", "Software");
          put("SecureRandom.MD5PRNG", gnu.crypto.jce.prng.MD5RandomSpi.class.getName());
          put("SecureRandom.MD5PRNG ImplementedIn", "Software");
          put("SecureRandom.RIPEMD128PRNG", gnu.crypto.jce.prng.RipeMD128RandomSpi.class.getName());
          put("SecureRandom.RIPEMD128PRNG ImplementedIn", "Software");
          put("SecureRandom.RIPEMD160PRNG", gnu.crypto.jce.prng.RipeMD160RandomSpi.class.getName());
          put("SecureRandom.RIPEMD160PRNG ImplementedIn", "Software");
          put("SecureRandom.SHA-160PRNG", gnu.crypto.jce.prng.Sha160RandomSpi.class.getName());
          put("SecureRandom.SHA-160PRNG ImplementedIn", "Software");
          put("SecureRandom.SHA-256PRNG", gnu.crypto.jce.prng.Sha256RandomSpi.class.getName());
          put("SecureRandom.SHA-256PRNG ImplementedIn", "Software");
          put("SecureRandom.SHA-384PRNG", gnu.crypto.jce.prng.Sha384RandomSpi.class.getName());
          put("SecureRandom.SHA-384PRNG ImplementedIn", "Software");
          put("SecureRandom.SHA-512PRNG", gnu.crypto.jce.prng.Sha512RandomSpi.class.getName());
          put("SecureRandom.SHA-512PRNG ImplementedIn", "Software");
          put("SecureRandom.TIGERPRNG", gnu.crypto.jce.prng.TigerRandomSpi.class.getName());
          put("SecureRandom.TIGERPRNG ImplementedIn", "Software");
          put("SecureRandom.HAVALPRNG", gnu.crypto.jce.prng.HavalRandomSpi.class.getName());
          put("SecureRandom.HAVALPRNG ImplementedIn", "Software");
          put("SecureRandom.WHIRLPOOLPRNG", gnu.crypto.jce.prng.WhirlpoolRandomSpi.class.getName());
          put("SecureRandom.WHIRLPOOLPRNG ImplementedIn", "Software");
          put("SecureRandom.ICM", gnu.crypto.jce.prng.ICMRandomSpi.class.getName());
          put("SecureRandom.ICM ImplementedIn", "Software");
          put("SecureRandom.UMAC-KDF", gnu.crypto.jce.prng.UMacRandomSpi.class.getName());
          put("SecureRandom.UMAC-KDF ImplementedIn", "Software");

          // KeyPairGenerator
          put("KeyPairGenerator.DSS", gnu.crypto.jce.sig.DSSKeyPairGeneratorSpi.class.getName());
          put("KeyPairGenerator.DSS KeySize", "1024");
          put("KeyPairGenerator.DSS ImplementedIn", "Software");
          put("KeyPairGenerator.RSA", gnu.crypto.jce.sig.RSAKeyPairGeneratorSpi.class.getName());
          put("KeyPairGenerator.RSA KeySize", "1024");
          put("KeyPairGenerator.RSA ImplementedIn", "Software");

          // Signature
          put("Signature.DSS/RAW", gnu.crypto.jce.sig.DSSRawSignatureSpi.class.getName());
          put("Signature.DSS/RAW KeySize", "1024");
          put("Signature.DSS/RAW ImplementedIn", "Software");
          put("Signature.RSA-PSS/RAW", gnu.crypto.jce.sig.RSAPSSRawSignatureSpi.class.getName());
          put("Signature.RSA-PSS/RAW KeySize", "1024");
          put("Signature.RSA-PSS/RAW ImplementedIn", "Software");

          // KeyStore
          put("KeyStore.GKR", gnu.crypto.jce.keyring.GnuKeyring.class.getName());

          // Aliases
          put("Alg.Alias.MessageDigest.SHS", "SHA-160");
          put("Alg.Alias.MessageDigest.SHA", "SHA-160");
          put("Alg.Alias.MessageDigest.SHA1", "SHA-160");
          put("Alg.Alias.MessageDigest.SHA-1", "SHA-160");
          put("Alg.Alias.MessageDigest.SHA2-256", "SHA-256");
          put("Alg.Alias.MessageDigest.SHA2-384", "SHA-384");
          put("Alg.Alias.MessageDigest.SHA2-512", "SHA-512");
          put("Alg.Alias.MessageDigest.SHA256", "SHA-256");
          put("Alg.Alias.MessageDigest.SHA384", "SHA-384");
          put("Alg.Alias.MessageDigest.SHA512", "SHA-512");
          put("Alg.Alias.MessageDigest.RIPEMD-160", "RIPEMD160");
          put("Alg.Alias.MessageDigest.RIPEMD-128", "RIPEMD128");
          put("Alg.Alias.MessageDigest.OID.1.2.840.11359.2.2", "MD2");
          put("Alg.Alias.MessageDigest.1.2.840.11359.2.2", "MD2");
          put("Alg.Alias.MessageDigest.OID.1.2.840.11359.2.5", "MD5");
          put("Alg.Alias.MessageDigest.1.2.840.11359.2.5", "MD5");
          put("Alg.Alias.MessageDigest.OID.1.3.14.3.2.26", "SHA1");
          put("Alg.Alias.MessageDigest.1.3.14.3.2.26", "SHA1");

          put("Alg.Alias.SecureRandom.RC4", "ARCFOUR");
          put("Alg.Alias.SecureRandom.SHA-1PRNG", "SHA-160PRNG");
          put("Alg.Alias.SecureRandom.SHA1PRNG", "SHA-160PRNG");
          put("Alg.Alias.SecureRandom.SHAPRNG", "SHA-160PRNG");
          put("Alg.Alias.SecureRandom.SHA-256PRNG", "SHA-256PRNG");
          put("Alg.Alias.SecureRandom.SHA-2-1PRNG", "SHA-256PRNG");
          put("Alg.Alias.SecureRandom.SHA-384PRNG", "SHA-384PRNG");
          put("Alg.Alias.SecureRandom.SHA-2-2PRNG", "SHA-384PRNG");
          put("Alg.Alias.SecureRandom.SHA-512PRNG", "SHA-512PRNG");
          put("Alg.Alias.SecureRandom.SHA-2-3PRNG", "SHA-512PRNG");

          put("Alg.Alias.KeyPairGenerator.DSA", "DSS");

          put("Alg.Alias.Signature.DSA", "DSS/RAW");
          put("Alg.Alias.Signature.SHAwithDSA", "DSS/RAW");
          put("Alg.Alias.Signature.SHA1withDSA", "DSS/RAW");
          put("Alg.Alias.Signature.SHA160withDSA", "DSS/RAW");
          put("Alg.Alias.Signature.SHA/DSA", "DSS/RAW");
          put("Alg.Alias.Signature.SHA1/DSA", "DSS/RAW");
          put("Alg.Alias.Signature.SHA-1/DSA", "DSS/RAW");
          put("Alg.Alias.Signature.SHA-160/DSA", "DSS/RAW");
          put("Alg.Alias.Signature.DSAwithSHA", "DSS/RAW");
          put("Alg.Alias.Signature.DSAwithSHA1", "DSS/RAW");
          put("Alg.Alias.Signature.DSAwithSHA160", "DSS/RAW");
          put("Alg.Alias.Signature.RSA-PSS", "RSA-PSS/RAW");
          put("Alg.Alias.Signature.RSAPSS", "RSA-PSS/RAW");
          put("Alg.Alias.KeyStore.GnuKeyring", "GKR");

          return null;
        }
      });
  }

  // Class methods.
  // -------------------------------------------------------------------------

  /**
   * <p>Returns a {@link Set} of names of message digest algorithms available
   * from this {@link Provider}.</p>
   *
   * @return a {@link Set} of hash names (Strings).
   */
  public static final Set getMessageDigestNames()
  {
    return HashFactory.getNames();
  }

  /**
   * <p>Returns a {@link Set} of names of secure random implementations
   * available from this {@link Provider}.</p>
   *
   * @return a {@link Set} of secure random names (Strings).
   */
  public static final Set getSecureRandomNames()
  {
    Set result = new HashSet();
    // do all the hash-based prng algorithms
    Set md = gnu.crypto.hash.HashFactory.getNames();
    for (Iterator it = md.iterator(); it.hasNext();)
      {
        result.add(((String) it.next()).toUpperCase() + "PRNG");
      }
    // add ICM and UMAC based generators
    result.add(Registry.ICM_PRNG.toUpperCase());
    result.add(Registry.UMAC_PRNG.toUpperCase());
    result.add(Registry.ARCFOUR_PRNG.toUpperCase());

    return Collections.unmodifiableSet(result);
  }

  /**
   * <p>Returns a {@link Set} of names of keypair generator implementations
   * available from this {@link Provider}.</p>
   *
   * @return a {@link Set} of key pair generator names (Strings).
   */
  public static final Set getKeyPairGeneratorNames()
  {
    return KeyPairGeneratorFactory.getNames();
  }

  /**
   * <p>Returns a {@link Set} of names of signature scheme implementations
   * available from this {@link Provider}.</p>
   *
   * @return a {@link Set} of signature names (Strings).
   */
  public static final Set getSignatureNames()
  {
    return SignatureFactory.getNames();
  }
}
