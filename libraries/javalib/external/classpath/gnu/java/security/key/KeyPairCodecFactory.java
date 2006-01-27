/* KeyPairCodecFactory.java -- 
   Copyright 2001, 2002, 2006 Free Software Foundation, Inc.

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


package gnu.java.security.key;

import gnu.java.security.Registry;
import gnu.java.security.key.dss.DSSKeyPairRawCodec;
import gnu.java.security.key.dss.DSSPrivateKey;
import gnu.java.security.key.dss.DSSPublicKey;
import gnu.java.security.key.rsa.GnuRSAPrivateKey;
import gnu.java.security.key.rsa.GnuRSAPublicKey;
import gnu.java.security.key.rsa.RSAKeyPairRawCodec;

import java.lang.reflect.Constructor;
import java.util.Set;
import java.security.Key;
import java.security.PrivateKey;
import java.security.PublicKey;

/**
 * <p>A <i>Factory</i> class to instantiate key encoder/decoder instances.</p>
 *
 * @version $Revision: 1.1 $
 */
public class KeyPairCodecFactory
{

  // Constants and variables
  // -------------------------------------------------------------------------

  // Constructor(s)
  // -------------------------------------------------------------------------

  /** Trivial constructor to enforce Singleton pattern. */
  private KeyPairCodecFactory()
  {
    super();
  }

  // Class methods
  // -------------------------------------------------------------------------

  /**
   * <p>Returns an instance of a keypair codec given its name.</p>
   *
   * @param name the case-insensitive key codec name.
   * @return an instance of the keypair codec, or <code>null</code> if none
   * found.
   */
  public static IKeyPairCodec getInstance(String name)
  {
    if (name == null)
      {
        return null;
      }

    name = name.trim();
    IKeyPairCodec result = null;
    if (name.equalsIgnoreCase(Registry.DSA_KPG)
        || name.equals(Registry.DSS_KPG))
      {
        result = new DSSKeyPairRawCodec();
      }
    else if (name.equalsIgnoreCase(Registry.RSA_KPG))
      {
        result = new RSAKeyPairRawCodec();
      }
    else if (name.equalsIgnoreCase(Registry.DH_KPG))
      {
        result = makeInstance ("gnu.javax.crypto.key.dh.DHKeyPairRawCodec");
      }
    else if (name.equalsIgnoreCase(Registry.SRP_KPG))
      {
        result = makeInstance ("gnu.javax.crypto.key.srp6.SRPKeyPairRawCodec");
      }

    return result;
  }

  /**
   * <p>Returns an instance of a keypair codec given a byte array that is
   * assumed to contain a previously encoded key (public or private).</p>
   *
   * @param buffer a byte array containing a previously encoded key.
   * @return an instance of the keypair codec, or <code>null</code> if none
   * found.
   */
  public static IKeyPairCodec getInstance(byte[] buffer)
  {
    if (buffer == null)
      {
        return null;
      }
    // our codecs prefix the stream with a 4-byte magic
    if (buffer.length < 5)
      {
        return null;
      }
    // the first byte is always 0x47
    if (buffer[0] != 0x47)
      {
        return null;
      }
    // so far we only have RAW codecs
    if (buffer[1] != Registry.RAW_ENCODING_ID)
      {
        return null;
      }
    IKeyPairCodec result = null;
    switch (buffer[2])
      {
      case 0x44:
        result = new DSSKeyPairRawCodec();
        break;
      case 0x52:
        result = new RSAKeyPairRawCodec();
        break;
      case 0x48:
        result = makeInstance ("gnu.javax.crypto.key.dh.DHKeyPairRawCodec");
        break;
      case 0x53:
        result = makeInstance ("gnu.javax.crypto.key.srp6.SRPKeyPairRawCodec");
        break;
      }

    return result;
  }

  /**
   * <p>Returns an instance of a keypair codec given a key.</p>
   *
   * @param key the key to encode.
   * @return an instance of the keypair codec, or <code>null</code> if none
   * found.
   */
  public static IKeyPairCodec getInstance(Key key)
  {
    if (key == null)
      {
        return null;
      }

    IKeyPairCodec result = null;
    if (key instanceof PublicKey)
      {
        if (key instanceof DSSPublicKey)
          {
            result = new DSSKeyPairRawCodec();
          }
        else if (key instanceof GnuRSAPublicKey)
          {
            result = new RSAKeyPairRawCodec();
          }
        else if (matches (key, "gnu.javax.crypto.key.dh.GnuDHPublicKey"))
          {
            result = makeInstance ("gnu.javax.crypto.key.dh.DHKeyPairRawCodec");
          }
        else if (matches (key, "gnu.javax.crypto.key.srp6.SRPPublicKey"))
          {
            result = makeInstance ("gnu.javax.crypto.key.srp6.SRPKeyPairRawCodec");
          }
      }
    else if (key instanceof PrivateKey)
      {
        if (key instanceof DSSPrivateKey)
          {
            result = new DSSKeyPairRawCodec();
          }
        else if (key instanceof GnuRSAPrivateKey)
          {
            result = new RSAKeyPairRawCodec();
          }
        else if (matches (key, "gnu.javax.crypto.key.dh.GnuDHPrivateKey"))
          {
            result = makeInstance ("gnu.javax.crypto.key.dh.DHKeyPairRawCodec");
          }
        else if (matches (key, "gnu.javax.crypto.key.srp6.SRPPrivateKey"))
          {
            result = makeInstance ("gnu.javax.crypto.key.srp6.SRPKeyPairRawCodec");
          }
      }

    return result;
  }

  /**
   * <p>Returns a {@link Set} of keypair codec names supported by this
   * <i>Factory</i>.</p>
   *
   * @return a {@link Set} of keypair codec names (Strings).
   */
  public static final Set getNames()
  {
    return KeyPairGeneratorFactory.getNames();
  }

  private static IKeyPairCodec makeInstance (String clazz)
  {
    try
      {
        Class c = Class.forName (clazz);
        Constructor ctor = c.getConstructor (new Class[0]);
        return (IKeyPairCodec) ctor.newInstance (new Object[0]);
      }
    catch (Exception x)
      {
        IllegalArgumentException iae =
          new IllegalArgumentException ("strong crypto key codec not available: "
                                        + clazz);
        iae.initCause (x);
        throw iae;
      }
  }

  private static boolean matches (Object o, String clazz)
  {
    try
      {
        Class c = Class.forName (clazz);
        return c.isAssignableFrom (o.getClass ());
      }
    catch (Exception x)
      {
        // Can't match.
        return false;
      }
  }
}
