/* X509KeyManagerFactory.java -- X.509 key manager factory.
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
import java.net.Socket;

import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Enumeration;

import java.security.InvalidAlgorithmParameterException;
import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.NoSuchAlgorithmException;
import java.security.Principal;
import java.security.PrivateKey;
import java.security.Security;
import java.security.UnrecoverableKeyException;
import java.security.cert.Certificate;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;

import java.util.HashMap;

import javax.net.ssl.KeyManager;
import javax.net.ssl.KeyManagerFactorySpi;
import javax.net.ssl.ManagerFactoryParameters;
import javax.net.ssl.X509KeyManager;

public class X509KeyManagerFactory extends KeyManagerFactorySpi
{

  // Fields.
  // -------------------------------------------------------------------------

  private Manager current;

  // Constructor.
  // -------------------------------------------------------------------------

  public X509KeyManagerFactory()
  {
    super();
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  protected KeyManager[] engineGetKeyManagers()
  {
    if (current == null)
      throw new IllegalStateException();
    return new KeyManager[] { current };
  }

  protected void engineInit(ManagerFactoryParameters params)
    throws InvalidAlgorithmParameterException
  {
    throw new InvalidAlgorithmParameterException();
  }

  protected void engineInit(KeyStore store, char[] passwd)
    throws KeyStoreException, NoSuchAlgorithmException,
           UnrecoverableKeyException
  {
    if (store == null)
      {
        String s = System.getProperty("javax.net.ssl.keyStoreType");
        if (s == null)
          s = KeyStore.getDefaultType();
        store = KeyStore.getInstance(s);
        s = System.getProperty("javax.net.ssl.keyStore");
        if (s == null)
          return;
        String p = System.getProperty("javax.net.ssl.keyStorePassword");
        try
          {
            store.load(new FileInputStream(s), p != null ? p.toCharArray() : null);
          }
        catch (IOException ioe)
          {
            throw new KeyStoreException(ioe.toString());
          }
        catch (CertificateException ce)
          {
            throw new KeyStoreException(ce.toString());
          }
      }

    HashMap p = new HashMap();
    HashMap c = new HashMap();
    Enumeration aliases = store.aliases();
    while (aliases.hasMoreElements())
      {
        String alias = (String) aliases.nextElement();
        if (!store.isKeyEntry(alias))
          continue;
        X509Certificate[] chain = new X509Certificate[0];
        if (store.getCertificateChain(alias).length > 0 &&
            (store.getCertificateChain(alias)[0] instanceof X509Certificate))
          chain = toX509Chain(store.getCertificateChain(alias));
        PrivateKey key = (PrivateKey) store.getKey(alias, passwd);
        p.put(alias, key);
        c.put(alias, chain);
      }
    current = this.new Manager(p, c);
  }

  private static X509Certificate[] toX509Chain(Certificate[] chain)
  {
    X509Certificate[] _chain = new X509Certificate[chain.length];
    for (int i = 0; i < chain.length; i++)
      _chain[i] = (X509Certificate) chain[i];
    return _chain;
  }

  // Inner class.
  // -------------------------------------------------------------------------

  private class Manager implements X509KeyManager
  {
    // Fields.
    // -----------------------------------------------------------------------

    private final HashMap privateKeys;

    private final HashMap certChains;

    // Constructor.
    // -----------------------------------------------------------------------

    Manager(HashMap privateKeys, HashMap certChains)
    {
      this.privateKeys = privateKeys;
      this.certChains = certChains;
    }

    // Instance methods.
    // -----------------------------------------------------------------------

    public String chooseClientAlias(String[] keyTypes, Principal[] issuers,
                                    Socket socket)
    {
      for (int i = 0; i < keyTypes.length; i++)
        {
          String[] s = getClientAliases(keyTypes[i], issuers);
          if (s.length > 0)
            return s[0];
        }
      return null;
    }

    public String[] getClientAliases(String keyType, Principal[] issuers)
    {
      return getAliases(keyType, issuers);
    }

    public String chooseServerAlias(String keyType, Principal[] issuers,
                                    Socket socket)
    {
      String[] s = getServerAliases(keyType, issuers);
      if (s.length > 0)
        return s[0];
      return null;
    }

    public String[] getServerAliases(String keyType, Principal[] issuers)
    {
      return getAliases(keyType, issuers);
    }

    private String[] getAliases(String keyType, Principal[] issuers)
    {
      LinkedList l = new LinkedList();
      for (Iterator i = privateKeys.keySet().iterator(); i.hasNext(); )
        {
          String alias = (String) i.next();
          if (!keyType.equalsIgnoreCase(getPrivateKey(alias).getAlgorithm()))
            continue;
          X509Certificate[] chain = getCertificateChain(alias);
          if (chain.length == 0)
            continue;
          if (issuers == null || issuers.length == 0)
            {
              l.add(alias);
              continue;
            }
          for (int j = 0; j < issuers.length; j++)
            if (chain[0].getIssuerDN().equals(issuers[j]))
              {
                l.add(alias);
                break;
              }
        }
      return (String[]) l.toArray(new String[l.size()]);
    }

    public X509Certificate[] getCertificateChain(String alias)
    {
      X509Certificate[] c = (X509Certificate[]) certChains.get(alias);
      return (X509Certificate[]) c.clone();
    }

    public PrivateKey getPrivateKey(String alias)
    {
      return (PrivateKey) privateKeys.get(alias);
    }
  }
}
