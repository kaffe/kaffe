/* Context.java -- SSLContext implementation.
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

import java.io.InputStream;
import java.net.URL;

import java.security.KeyStoreException;
import java.security.KeyManagementException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.SecureRandom;
import java.security.Security;
import java.security.UnrecoverableKeyException;

import javax.net.ssl.KeyManager;
import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.SSLContextSpi;
import javax.net.ssl.SSLServerSocketFactory;
import javax.net.ssl.SSLSessionContext;
import javax.net.ssl.SSLSocketFactory;
import javax.net.ssl.TrustManager;
import javax.net.ssl.TrustManagerFactory;
import javax.net.ssl.X509KeyManager;
import javax.net.ssl.X509TrustManager;

public class Context extends SSLContextSpi
{

  // Fields.
  // -------------------------------------------------------------------------

  private SessionContext clientSessions;

  private SessionContext serverSessions;

  private X509KeyManager keyManager;

  private X509TrustManager trustManager;

  private SecureRandom random;

  // Constructor.
  // -------------------------------------------------------------------------

  public Context()
  {
    clientSessions = new SessionContext();
    serverSessions = new SessionContext();
  }

  // Engine methods.
  // -------------------------------------------------------------------------

  protected SSLSessionContext engineGetClientSessionContext()
  {
    return clientSessions;
  }

  protected SSLSessionContext engineGetServerSessionContext()
  {
    return serverSessions;
  }

  protected SSLServerSocketFactory engineGetServerSocketFactory()
  {
    if (keyManager == null || trustManager == null || random == null)
      throw new IllegalStateException();
    return null;  // XXX
  }

  protected SSLSocketFactory engineGetSocketFactory()
  {
    if (keyManager == null || trustManager == null || random == null)
      throw new IllegalStateException();
    return null;  // XXX
  }

  protected void engineInit(KeyManager[] keyManagers,
                            TrustManager[] trustManagers, SecureRandom random)
    throws KeyManagementException
  {
    keyManager = null;
    trustManager = null;
    if (keyManagers != null)
      for (int i = 0; i < keyManagers.length; i++)
        {
          if (keyManagers[i] instanceof X509KeyManager)
            {
              keyManager = (X509KeyManager) keyManagers[i];
              break;
            }
        }
    if (keyManager == null)
      keyManager = defaultKeyManager();
    if (trustManagers != null)
      for (int i = 0; i < trustManagers.length; i++)
        {
          if (trustManagers[i] instanceof X509TrustManager)
            {
              trustManager = (X509TrustManager) trustManagers[i];
            }
        }
    if (trustManager == null)
      trustManager = defaultTrustManager();
    if (random != null)
      this.random = random;
    else
      this.random = defaultRandom();
  }

  // Own methods.
  // -------------------------------------------------------------------------

  private X509KeyManager defaultKeyManager()
    throws KeyManagementException
  {
    try
      {
        KeyManagerFactory fact = KeyManagerFactory.getInstance("GnuX509",
                                                               "GnuJSSE");
        String pass = Security.getProperty("gnu.net.ssl.keyPassword");
        if (pass == null)
          pass = "";
        fact.init(null, pass.toCharArray());
        return (X509KeyManager) fact.getKeyManagers()[0];
      }
    catch (NoSuchAlgorithmException nsae)
      {
        throw new KeyManagementException();
      }
    catch (NoSuchProviderException nspe)
      {
        throw new KeyManagementException();
      }
    catch (KeyStoreException kse)
      {
        throw new KeyManagementException();
      }
    catch (UnrecoverableKeyException uke)
      {
        throw new KeyManagementException();
      }
  }

  private X509TrustManager defaultTrustManager()
    throws KeyManagementException
  {
    try
      {
        TrustManagerFactory fact = TrustManagerFactory.getInstance("GnuX509",
                                                                   "GnuJSSE");
        fact.init((java.security.KeyStore) null);
        return (X509TrustManager) fact.getTrustManagers()[0];
      }
    catch (NoSuchAlgorithmException nsae)
      {
        throw new KeyManagementException(nsae.toString());
      }
    catch (NoSuchProviderException nspe)
      {
        throw new KeyManagementException(nspe.toString());
      }
    catch (KeyStoreException kse)
      {
        throw new KeyManagementException(kse.toString());
      }
  }

  private SecureRandom defaultRandom()
    throws KeyManagementException
  {
    String alg = Security.getProperty("gnu.net.ssl.secureRandom");
    if (alg == null)
      alg = "SHA1PRNG";
    SecureRandom rand = null;
    try
      {
        rand = SecureRandom.getInstance(alg);
      }
    catch (NoSuchAlgorithmException nsae)
      {
        throw new KeyManagementException(nsae.toString());
      }

    String entropy = Security.getProperty("jessie.egd");
    if (entropy != null)
      try
        {
          InputStream in = new URL(entropy).openStream();
          byte[] seed = new byte[20];
          in.read(seed);
          rand.setSeed(seed);
        }
      catch (Exception e)
        {
        }

    return rand;
  }
}
