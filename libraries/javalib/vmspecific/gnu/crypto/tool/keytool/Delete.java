/* Delete.java -- delete command.
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

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;

import java.security.KeyStore;
import java.security.KeyStoreException;
import java.security.cert.CertificateFactory;

import javax.security.auth.callback.Callback;
import javax.security.auth.callback.NameCallback;
import javax.security.auth.callback.PasswordCallback;

class Delete extends Command
{

  // Constructors.
  // -------------------------------------------------------------------------

  Delete (Command previous)
  {
    super (previous);
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  public void run() throws Exception
  {
    KeyStore store = null;
    KeyStore store2 = null;
    CertificateFactory cert = null;
    if (provider != null)
      {
        store = KeyStore.getInstance (storeType, provider);
        cert  = CertificateFactory.getInstance (certType, provider);
        if (secretStore != null)
          store2 = KeyStore.getInstance (storeType, provider);
      }
    else
      {
        store = KeyStore.getInstance (storeType);
        cert  = CertificateFactory.getInstance (certType);
        if (secretStore != null)
          store2 = KeyStore.getInstance (storeType, provider);
      }

    NameCallback aliasIn = new NameCallback ("alias: ", alias);
    PasswordCallback pass =
      new PasswordCallback ("keystore password: ", false);
    PasswordCallback pass2 =
      new PasswordCallback ("secret keystore password: ", false);
    Callback[] cb = null;
    if (store2 != null)
      {
        if (alias.equals (DEFAULT_ALIAS))
          cb = new Callback[] { aliasIn, pass, pass2 };
        else
          cb = new Callback[] { pass, pass2 };
      }
    else
      {
        if (alias.equals (DEFAULT_ALIAS))
          cb = new Callback[] { aliasIn, pass };
        else
          cb = new Callback[] { pass };
      }
    handler.handle (cb);

    if (storeFile.equals ("-"))
      {
        if (verbose > 0)
          println ("Loading keystore from standard input");
        store.load (System.in, pass.getPassword());
      }
    else
      {
        if (verbose > 0)
          println ("Loading keystore from " + storeFile);
        FileInputStream in = new FileInputStream (storeFile);
        store.load (in, pass.getPassword());
        in.close();
      }
    if (store2 != null)
      {
        if (secretStore.equals ("-"))
          {
            if (verbose > 0)
              println ("Loading secret keystore from standard input");
            store2.load (System.in, pass2.getPassword());
          }
        else
          {
            if (verbose > 0)
              println ("Loading secret keystore from " + storeFile);
            FileInputStream in = new FileInputStream (storeFile);
            store.load (in, pass.getPassword());
            in.close();
          }
      }
    String alias = this.alias;
    if (alias.equals (DEFAULT_ALIAS))
      alias = aliasIn.getName();

    if (!store.containsAlias (alias))
      throw new Exception ("no such alias: " + alias);
    store.deleteEntry (alias);

    OutputStream out = null;
    if (outFile != null)
      out = (outFile.equals ("-") ? (OutputStream) System.out
             : (OutputStream) new FileOutputStream (outFile));
    else
      out = (storeFile.equals ("-") ? (OutputStream) System.out
             : (OutputStream) new FileOutputStream (storeFile));

    if (verbose > 0)
      println ("Writing keystore to " +
               (out == System.out ? "standard output"
                : (outFile == null ? storeFile : outFile)));

    store.store (out, pass.getPassword());
    if (out != System.out)
      {
        out.flush();
        out.close();
      }

    if (store2 != null)
      {
        if (!store2.containsAlias (alias))
          throw new Exception ("no such alias: " + alias);
        store2.deleteEntry (alias);
      }
    if (store2 != null)
      {
        out = null;
        if (outFile != null)
          out = (outFile.equals ("-") ? (OutputStream) System.out
                 : (OutputStream) new FileOutputStream (outFile));
        else
          out = (storeFile.equals ("-") ? (OutputStream) System.out
                 : (OutputStream) new FileOutputStream (storeFile));

        if (verbose > 0)
          println ("Writing secret keystore to " +
                   (out == System.out ? "standard output"
                    : (outFile == null ? storeFile : outFile)));

        store2.store (out, pass.getPassword());
        if (out != System.out)
          {
            out.flush();
            out.close();
          }
      }
  }

  public String toString()
  {
    return "delete";
  }
}
