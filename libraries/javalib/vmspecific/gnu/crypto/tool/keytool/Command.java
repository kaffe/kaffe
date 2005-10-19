/* Command.java -- a keytool command.
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

import gnu.crypto.auth.callback.ConsoleCallbackHandler;
import java.io.IOException;
import java.security.KeyStore;
import java.security.Provider;
import java.util.Date;
import javax.security.auth.callback.Callback;
import javax.security.auth.callback.CallbackHandler;
import javax.security.auth.callback.TextOutputCallback;
import javax.security.auth.callback.UnsupportedCallbackException;

class Command
{

  // Fields.
  // -------------------------------------------------------------------------

  static final String DEFAULT_ALIAS = "mykey";

  boolean armor;
  boolean noKeystore;
  int validity;
  int verbose;
  String alias;
  String caAlias;
  String file;
  String outFile;
  String storeType;
  String certType;
  String storeFile;
  String secretStore;
  CallbackHandler handler;
  Date date;
  String providerName;
  Provider provider;

  // Constructors.
  // -------------------------------------------------------------------------

  Command (Command previous)
  {
    armor        = previous.armor;
    noKeystore   = previous.noKeystore;
    validity     = previous.validity;
    verbose      = previous.verbose;
    alias        = previous.alias;
    caAlias      = previous.caAlias;
    file         = previous.file;
    outFile      = previous.outFile;
    storeType    = previous.storeType;
    certType     = previous.certType;
    storeFile    = previous.storeFile;
    secretStore  = previous.secretStore;
    date         = previous.date;
    handler      = previous.handler;
    providerName = previous.providerName;
    provider     = previous.provider;
  }

  Command()
  {
    validity = 365;
    verbose = 0;
    alias = DEFAULT_ALIAS;
    storeType = KeyStore.getDefaultType();
    certType = "X.509";
    storeFile = (System.getProperty ("user.home") +
                 System.getProperty ("file.separator") +
                 ".keystore");
    date = new Date();
    handler = new ConsoleCallbackHandler (System.err);
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  public void run() throws Exception
  {
    throw new RuntimeException ("not implemented");
  }

  protected void println (String mesg)
    throws UnsupportedCallbackException, IOException
  {
    handler.handle (new Callback[] {
      new TextOutputCallback (TextOutputCallback.INFORMATION,
                              mesg + System.getProperty ("line.separator"))
    });
  }

  protected void print (String mesg)
    throws UnsupportedCallbackException, IOException
  {
    handler.handle (new TextOutputCallback[] {
      new TextOutputCallback (TextOutputCallback.INFORMATION, mesg)
    });
  }
}
