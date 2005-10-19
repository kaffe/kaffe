/* GnuSecurity.java -- GNU Crypto's javax.security.sasl algorithms.
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
import gnu.crypto.sasl.ClientFactory;
import gnu.crypto.sasl.ServerFactory;

import java.security.AccessController;
import java.security.PrivilegedAction;
import java.security.Provider;
import java.util.Set;

public final class GnuSasl extends Provider
{

  // Constructor.
  // -------------------------------------------------------------------------

  public GnuSasl()
  {
    super (Registry.GNU_SASL, 2.1, "GNU Crypto SASL Provider");

    AccessController.doPrivileged (new PrivilegedAction()
      {
        public Object run()
        {
          // SASL Client and Server mechanisms
          put("SaslClientFactory.ANONYMOUS", gnu.crypto.sasl.ClientFactory.class.getName());
          put("SaslClientFactory.PLAIN", gnu.crypto.sasl.ClientFactory.class.getName());
          put("SaslClientFactory.CRAM-MD5", gnu.crypto.sasl.ClientFactory.class.getName());
          put("SaslClientFactory.SRP", gnu.crypto.sasl.ClientFactory.class.getName());

          put("SaslServerFactory.ANONYMOUS", gnu.crypto.sasl.ServerFactory.class.getName());
          put("SaslServerFactory.PLAIN", gnu.crypto.sasl.ServerFactory.class.getName());
          put("SaslServerFactory.CRAM-MD5", gnu.crypto.sasl.ServerFactory.class.getName());
          put("SaslServerFactory.SRP-MD5", gnu.crypto.sasl.ServerFactory.class.getName());
          put("SaslServerFactory.SRP-SHA-160", gnu.crypto.sasl.ServerFactory.class.getName());
          put("SaslServerFactory.SRP-RIPEMD128", gnu.crypto.sasl.ServerFactory.class.getName());
          put("SaslServerFactory.SRP-RIPEMD160", gnu.crypto.sasl.ServerFactory.class.getName());
          put("SaslServerFactory.SRP-TIGER", gnu.crypto.sasl.ServerFactory.class.getName());
          put("SaslServerFactory.SRP-WHIRLPOOL", gnu.crypto.sasl.ServerFactory.class.getName());

          put("Alg.Alias.SaslServerFactory.SRP-SHS", "SRP-SHA-160");
          put("Alg.Alias.SaslServerFactory.SRP-SHA", "SRP-SHA-160");
          put("Alg.Alias.SaslServerFactory.SRP-SHA1", "SRP-SHA-160");
          put("Alg.Alias.SaslServerFactory.SRP-SHA-1", "SRP-SHA-160");
          put("Alg.Alias.SaslServerFactory.SRP-SHA160", "SRP-SHA-160");
          put("Alg.Alias.SaslServerFactory.SRP-RIPEMD-128", "SRP-RIPEMD128");
          put("Alg.Alias.SaslServerFactory.SRP-RIPEMD-160", "SRP-RIPEMD160");

          return null;
        }
      });
  }

  /**
   * <p>Returns a {@link Set} of names of SASL Client mechanisms available from
   * this {@link Provider}.</p>
   *
   * @return a {@link Set} of SASL Client mechanisms (Strings).
   */
  public static final Set getSaslClientMechanismNames()
  {
    return ClientFactory.getNames();
  }

  /**
   * <p>Returns a {@link Set} of names of SASL Server mechanisms available from
   * this {@link Provider}.</p>
   *
   * @return a {@link Set} of SASL Server mechanisms (Strings).
   */
  public static final Set getSaslServerMechanismNames()
  {
    return ServerFactory.getNames();
  }
}
