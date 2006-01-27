/* SignatureFactory.java -- 
   Copyright (C) 2001, 2002, 2003, 2006 Free Software Foundation, Inc.

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


package gnu.java.security.sig;

import gnu.java.security.Registry;
import gnu.java.security.sig.dss.DSSSignature;
import gnu.java.security.sig.rsa.RSAPSSSignature;
import gnu.java.security.sig.rsa.RSAPKCS1V1_5Signature;

import java.util.Collections;
import java.util.HashSet;
import java.util.Set;

/**
 * <p>A Factory to instantiate signature-with-appendix handlers.</p>
 *
 * @version $Revision: 1.1 $
 */
public class SignatureFactory
{

  // Constants and variables
  // -------------------------------------------------------------------------

  // Constructor(s)
  // -------------------------------------------------------------------------

  /** Trivial constructor to enforce Singleton pattern. */
  private SignatureFactory()
  {
    super();
  }

  // Class methods
  // -------------------------------------------------------------------------

  /**
   * <p>Returns an instance of a signature-with-appendix scheme given its name.</p>
   *
   * @param ssa the case-insensitive signature-with-appendix scheme name.
   * @return an instance of the scheme, or <code>null</code> if none found.
   */
  public static final ISignature getInstance(String ssa)
  {
    if (ssa == null)
      {
        return null;
      }

    ssa = ssa.trim();
    ISignature result = null;
    if (ssa.equalsIgnoreCase(Registry.DSA_SIG) || ssa.equals(Registry.DSS_SIG))
      {
        result = new DSSSignature();
      }
    else if (ssa.equalsIgnoreCase(Registry.RSA_PSS_SIG))
      {
        result = new RSAPSSSignature();
      }
    else if (ssa.equalsIgnoreCase(Registry.RSA_PKCS1_V1_5_SIG))
      {
        result = new RSAPKCS1V1_5Signature();
      }

    return result;
  }

  /**
   * <p>Returns a {@link Set} of signature-with-appendix scheme names supported
   * by this <i>Factory</i>.</p>
   *
   * @return a {@link Set} of signature-with-appendix scheme names (Strings).
   */
  public static final Set getNames()
  {
    HashSet hs = new HashSet();
    hs.add(Registry.DSS_SIG);
    hs.add(Registry.RSA_PSS_SIG);
    hs.add(Registry.RSA_PKCS1_V1_5_SIG);

    return Collections.unmodifiableSet(hs);
  }

  // Instance methods
  // -------------------------------------------------------------------------
}
