/* List.java -- list command.
   Copyright (C) 2004  Free Software Foundation, Inc.

This file is part of GNU Crypto.

GNU Crypto is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU Crypto is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; see the file COPYING.  If not, write to the
Free Software Foundation Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301  USA  */


package gnu.crypto.tool.keytool;

import gnu.crypto.util.Util;

import java.io.FileInputStream;
import java.io.IOException;
import java.io.PrintWriter;
import java.io.StringWriter;

import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.MessageDigest;
import java.security.cert.Certificate;
import java.security.cert.X509Certificate;
import java.util.Enumeration;

import javax.security.auth.callback.Callback;
import javax.security.auth.callback.PasswordCallback;
import javax.security.auth.callback.TextOutputCallback;

class List extends Command
{

  // Constructors.
  // -------------------------------------------------------------------------

  List (Command previous)
  {
    super (previous);
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  public void run() throws Exception
  {
    KeyStore store = null;
    if (provider != null)
      store = KeyStore.getInstance (storeType, provider);
    else
      store = KeyStore.getInstance (storeType);
    PasswordCallback pass =
      new PasswordCallback ("keystore password: ", false);
    handler.handle (new Callback[] { pass });
    store.load (new FileInputStream (storeFile),
                pass.getPassword());
    list (store);
  }

  private void list (final KeyStore store) throws Exception
  {
    Enumeration aliases = store.aliases();
    StringWriter str = new StringWriter();
    PrintWriter out = new PrintWriter (str);
    while (aliases.hasMoreElements())
      {
        String alias = (String) aliases.nextElement();
        out.print (alias);
        out.print (": ");
        out.print (store.getCreationDate (alias));
        out.print (", ");
        if (store.isCertificateEntry (alias))
          {
            out.println ("certificate entry");
            if (verbose == 1)
              {
                Certificate cert = store.getCertificate (alias);
                out.print ("  Type: ");
                out.println (cert.getType());
                if (cert instanceof X509Certificate)
                  {
                    out.print ("  Issuer: ");
                    out.println (((X509Certificate) cert).getIssuerDN());
                    out.print ("  Subject: ");
                    out.println (((X509Certificate) cert).getSubjectDN());
                    out.print ("  Valid at ");
                    out.print (date);
                    try
                      {
                        ((X509Certificate) cert).checkValidity (date);
                        out.println ("? yes");
                      }
                    catch (Throwable t)
                      {
                        out.println ("? no");
                      }
                  }
                try
                  {
                    MessageDigest md = MessageDigest.getInstance ("SHA-160");
                    byte[] digest = md.digest (cert.getPublicKey().getEncoded());
                    out.print ("  Public key fingerprint (SHA-160): ");
                    out.println (Util.toString (digest));
                  }
                catch (Throwable t)
                  {
                  }
              }
            else if (verbose >= 2)
              out.println (store.getCertificate (alias));
          }
        else
          out.println ("key entry");
      }
    print (str.toString());
  }

  public String toString()
  {
    return "list";
  }
}
