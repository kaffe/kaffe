/* Export.java -- export command.
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

import gnu.crypto.util.Base64;

import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import java.security.KeyStore;
import java.security.KeyStoreException;

import javax.security.auth.callback.Callback;
import javax.security.auth.callback.NameCallback;
import javax.security.auth.callback.PasswordCallback;

class Export extends Command
{

  // Constructors.
  // -------------------------------------------------------------------------

  Export (Command previous)
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
    NameCallback aliasIn = new NameCallback ("alias: ", alias);
    PasswordCallback pass =
      new PasswordCallback ("keystore password: ", false);
    Callback[] cb = null;
    if (alias.equals (DEFAULT_ALIAS))
      cb = new Callback[] { aliasIn, pass };
    else
      cb = new Callback[] { pass };
    handler.handle (cb);
    store.load (new FileInputStream (storeFile), pass.getPassword());
    String alias = this.alias;
    if (alias.equals (DEFAULT_ALIAS))
      alias = aliasIn.getName();
    if (store.isCertificateEntry (alias))
      {
        OutputStream out = null;
        if ((file == null || file.equals ("-")) ||
            (outFile == null || outFile.equals ("-")))
          out = System.out;
        else
          out = new FileOutputStream (file != null ? file : outFile);
        byte[] encoded = store.getCertificate (alias).getEncoded();
        if (armor)
          encoded = ("-----BEGIN CERTIFICATE----" +
                     System.getProperty ("line.separator") +
                     Base64.encode (encoded) +
                     System.getProperty ("line.separator") +
                     "-----END CERTIFICATE----" +
                     System.getProperty ("line.separator")).getBytes();
        out.write (encoded);
        if (out != System.out)
          {
            out.flush();
            out.close();
          }
      }
    else if (store.isKeyEntry (alias))
      throw new RuntimeException (alias + ": is a private key");
    else
      throw new RuntimeException (alias + ": no such alias");
  }

  public String toString()
  {
    return "export";
  }
}
