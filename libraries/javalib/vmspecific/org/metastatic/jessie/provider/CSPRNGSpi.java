/* CSPRNGSpi.java -- continuously seeded PRNG SecureRandom.
   Copyright (C) 2004 Casey Marshall <rsdio@metastatic.org>

This file is a part of Jessie.

Jessie is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

Jessie is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with Jessie; if not, write to the

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


package org.metastatic.jessie.provider;

import java.net.MalformedURLException;
import java.security.SecureRandomSpi;
import gnu.crypto.prng.LimitReachedException;

public final class CSPRNGSpi extends SecureRandomSpi
{

  // Fields.
  // -------------------------------------------------------------------------

  private final CSPRNG instance;

  // Constructors.
  // -------------------------------------------------------------------------

  public CSPRNGSpi()
    throws ClassNotFoundException, MalformedURLException, NumberFormatException
  {
    instance = CSPRNG.getSystemInstance();
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  protected void engineSetSeed(byte[] seed)
  {
    instance.addRandomBytes(seed, 0, seed.length);
  }

  protected void engineNextBytes(byte[] buffer)
  {
    try
      {
        instance.nextBytes(buffer, 0, buffer.length);
      }
    catch (LimitReachedException lre)
      {
        throw new IllegalStateException("CSPRNG limit reached");
      }
  }

  protected byte[] engineGenerateSeed(int nbytes)
  {
    byte[] buffer = new byte[nbytes];
    engineNextBytes(buffer);
    return buffer;
  }
}
