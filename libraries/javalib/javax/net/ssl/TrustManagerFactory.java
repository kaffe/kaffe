/* TrustManagerFactory.java -- factory for trust managers.
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


package javax.net.ssl;

import java.lang.reflect.InvocationTargetException;

import java.security.InvalidAlgorithmParameterException;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.Provider;
import java.security.Security;

import gnu.java.security.Engine;

public class TrustManagerFactory
{

  // Constants and fields.
  // -------------------------------------------------------------------------

  private static final String TRUST_MANAGER_FACTORY = "TrustManagerFactory";

  private final TrustManagerFactorySpi tmfSpi;

  private final Provider provider;

  private final String algorithm;

  // Constructor.
  // -------------------------------------------------------------------------

  protected TrustManagerFactory(TrustManagerFactorySpi tmfSpi,
                                Provider provider, String algorithm)
  {
    this.tmfSpi = tmfSpi;
    this.provider = provider;
    this.algorithm = algorithm;
  }

  // Class methods.
  // -------------------------------------------------------------------------

  public static final TrustManagerFactory getInstance(String algorithm)
    throws NoSuchAlgorithmException
  {
    Provider[] provs = Security.getProviders();
    for (int i = 0; i < provs.length; i++)
      {
        try
          {
            return getInstance(algorithm, provs[i]);
          }
        catch (NoSuchAlgorithmException ignore)
          {
          }
      }
    throw new NoSuchAlgorithmException(algorithm);
  }

  public static final TrustManagerFactory getInstance(String algorithm,
                                                      String provider)
    throws NoSuchAlgorithmException, NoSuchProviderException
  {
    if (provider == null)
      throw new IllegalArgumentException();
    Provider p = Security.getProvider(provider);
    if (p == null)
      throw new NoSuchProviderException(provider);
    return getInstance(algorithm, p);
  }

  public static final TrustManagerFactory getInstance(String algorithm,
                                                      Provider provider)
    throws NoSuchAlgorithmException
  {
    try
      {
        return new TrustManagerFactory((TrustManagerFactorySpi)
          Engine.getInstance(TRUST_MANAGER_FACTORY, algorithm, provider),
          provider, algorithm);
      }
    catch (InvocationTargetException ite)
      {
        throw new NoSuchAlgorithmException(algorithm);
      }
    catch (ClassCastException cce)
      {
        throw new NoSuchAlgorithmException(algorithm);
      }
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  public final String getAlgorithm()
  {
    return algorithm;
  }

  public final Provider getProvider()
  {
    return provider;
  }

  public final TrustManager[] getTrustManagers()
  {
    return tmfSpi.engineGetTrustManagers();
  }

  public final void init(ManagerFactoryParameters params)
    throws InvalidAlgorithmParameterException
  {
    tmfSpi.engineInit(params);
  }

  public final void init(KeyStore store) throws KeyStoreException
  {
    tmfSpi.engineInit(store);
  }
}
