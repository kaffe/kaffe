/* X509TrustManagerFactory.java -- X.509 trust manager factory.
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

import java.io.FileInputStream;
import java.io.IOException;

import java.util.Arrays;
import java.util.Enumeration;
import java.util.HashSet;
import java.util.LinkedList;

import java.security.InvalidAlgorithmParameterException;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.cert.Certificate;
import java.security.cert.CertificateException;
import java.security.cert.CertificateFactory;
import java.security.cert.CertPath;
import java.security.cert.CertPathValidator;
import java.security.cert.CertPathValidatorException;
import java.security.cert.PKIXParameters;
import java.security.cert.TrustAnchor;
import java.security.cert.X509Certificate;

import javax.net.ssl.ManagerFactoryParameters;
import javax.net.ssl.TrustManager;
import javax.net.ssl.TrustManagerFactorySpi;
import javax.net.ssl.X509TrustManager;

/**
 * A trust manager for X.509 certificates.
 */
public class X509TrustManagerFactory extends TrustManagerFactorySpi
{

  // Constants and fields.
  // -------------------------------------------------------------------------

  /**
   * The location of the JSSE key store.
   */
  private static final String JSSE_CERTS = System.getProperty("java.home")
    + System.getProperty("file.separator") + "lib"
    + System.getProperty("file.separator") + "security"
    + System.getProperty("file.separator") + "jssecerts";

  /**
   * The location of the system key store, containing the CA certs.
   */
  private static final String CA_CERTS = System.getProperty("java.home")
    + System.getProperty("file.separator") + "lib"
    + System.getProperty("file.separator") + "security"
    + System.getProperty("file.separator") + "cacerts";

  private Manager current;

  // Construtors.
  // -------------------------------------------------------------------------

  public X509TrustManagerFactory()
  {
    super();
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  protected TrustManager[] engineGetTrustManagers()
  {
    if (current == null)
      throw new IllegalStateException("not initialized");
    return new TrustManager[] { current };
  }

  protected void engineInit(ManagerFactoryParameters params)
    throws InvalidAlgorithmParameterException
  {
    throw new InvalidAlgorithmParameterException();
  }

  protected void engineInit(KeyStore store) throws KeyStoreException
  {
    if (store == null)
      {
        String s = System.getProperty("javax.net.ssl.trustStoreType");
        if (s == null)
          s = KeyStore.getDefaultType();
        store = KeyStore.getInstance(s);
        try
          {
            s = System.getProperty("javax.net.ssl.trustStore");
            FileInputStream in = null;
            if (s == null)
              {
                try
                  {
                    in = new FileInputStream(JSSE_CERTS);
                  }
                catch (IOException e)
                  {
                    in = new FileInputStream(CA_CERTS);
                  }
              }
            else
              {
                in = new FileInputStream(s);
              }
            String p = System.getProperty("javax.net.ssl.trustStorePassword");
            store.load(in, p != null ? p.toCharArray() : null);
          }
        catch (IOException ioe)
          {
            throw new KeyStoreException(ioe.toString());
          }
        catch (CertificateException ce)
          {
            throw new KeyStoreException(ce.toString());
          }
        catch (NoSuchAlgorithmException nsae)
          {
            throw new KeyStoreException(nsae.toString());
          }
      }

    LinkedList l = new LinkedList();
    Enumeration aliases = store.aliases();
    while (aliases.hasMoreElements())
      {
        String alias = (String) aliases.nextElement();
        if (!store.isCertificateEntry(alias))
          continue;
        Certificate c = store.getCertificate(alias);
        if (!(c instanceof X509Certificate))
          continue;
        l.add(c);
      }
    current = this.new Manager((X509Certificate[])
                               l.toArray(new X509Certificate[l.size()]));
  }

  // Inner class.
  // -------------------------------------------------------------------------

  /**
   * The actual manager implementation returned.
   */
  private class Manager implements X509TrustManager
  {

    // Fields.
    // -----------------------------------------------------------------------

    private final X509Certificate[] trusted;

    // Constructor.
    // -----------------------------------------------------------------------

    Manager(X509Certificate[] trusted)
    {
      this.trusted = trusted;
    }

    // Instance methodns.
    // -----------------------------------------------------------------------

    public void checkClientTrusted(X509Certificate[] chain, String authType)
      throws CertificateException
    {
      checkTrusted(chain, authType);
    }

    public void checkServerTrusted(X509Certificate[] chain, String authType)
      throws CertificateException
    {
      checkTrusted(chain, authType);
    }

    public X509Certificate[] getAcceptedIssuers()
    {
      if (trusted == null)
        return new X509Certificate[0];
      return (X509Certificate[]) trusted.clone();
    }

    // Own methods.
    // -----------------------------------------------------------------------

    private void checkTrusted(X509Certificate[] chain, String authType)
      throws CertificateException
    {
      CertificateFactory fact = null;
      CertPathValidator valid = null;
      try
        {
          fact = CertificateFactory.getInstance("X.509");
          valid = CertPathValidator.getInstance("PKIX");
        }
      catch (NoSuchAlgorithmException nsae)
        {
          throw new CertificateException(nsae.toString());
        }
      CertPath path = fact.generateCertPath(Arrays.asList(chain));
      HashSet anchors = new HashSet();
      for (int i = 0; i < trusted.length; i++)
        {
          anchors.add(new TrustAnchor(trusted[i], null));
        }
      try
        {
          valid.validate(path, new PKIXParameters(anchors));
        }
      catch (InvalidAlgorithmParameterException iape)
        {
          throw new CertificateException();
        }
      catch (CertPathValidatorException cpve)
        {
          throw new CertificateException();
        }
    }
  }
}
