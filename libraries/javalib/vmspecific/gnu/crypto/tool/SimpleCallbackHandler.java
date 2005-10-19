package gnu.crypto.tool;

// ----------------------------------------------------------------------------
// $Id: SimpleCallbackHandler.java,v 1.1 2005/10/19 20:15:52 guilhem Exp $
//
// Copyright (C) 2003, Free Software Foundation, Inc.
//
// This file is part of GNU Crypto.
//
// GNU Crypto is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// GNU Crypto is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; see the file COPYING.  If not, write to the
//
//    Free Software Foundation Inc.,
//    51 Franklin Street, Fifth Floor,
//    Boston, MA 02110-1301
//    USA
//
// Linking this library statically or dynamically with other modules is
// making a combined work based on this library.  Thus, the terms and
// conditions of the GNU General Public License cover the whole
// combination.
//
// As a special exception, the copyright holders of this library give
// you permission to link this library with independent modules to
// produce an executable, regardless of the license terms of these
// independent modules, and to copy and distribute the resulting
// executable under terms of your choice, provided that you also meet,
// for each linked independent module, the terms and conditions of the
// license of that module.  An independent module is a module which is
// not derived from or based on this library.  If you modify this
// library, you may extend this exception to your version of the
// library, but you are not obligated to do so.  If you do not wish to
// do so, delete this exception statement from your version.
// ----------------------------------------------------------------------------

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;

import javax.security.auth.callback.Callback;
import javax.security.auth.callback.CallbackHandler;
import javax.security.auth.callback.NameCallback;
import javax.security.auth.callback.PasswordCallback;
import javax.security.auth.callback.UnsupportedCallbackException;

/**
 * A simple {@link CallbackHandler} for test purposes.
 *
 * @version $Revision: 1.1 $
 */
public class SimpleCallbackHandler implements CallbackHandler {

   // Constants and variables
   // -------------------------------------------------------------------------

   // Constructor(s)
   // -------------------------------------------------------------------------

   // default 0-arguments ctor

   // Class methods
   // -------------------------------------------------------------------------

   // Instance methods
   // -------------------------------------------------------------------------

   public void handle(Callback[] callbacks)
   throws IOException, UnsupportedCallbackException {
      for (int i = 0; i < callbacks.length; i++) {
         if (callbacks[i] instanceof NameCallback) {
            NameCallback namecb = (NameCallback) callbacks[i];
            String defaultName = namecb.getDefaultName();
            if (defaultName != null) {
               System.out.print(namecb.getPrompt()+" ");
               System.out.println("["+defaultName+"]");
            } else {
               System.out.println(namecb.getPrompt());
            }
            String name = (new BufferedReader(new InputStreamReader(System.in)))
                  .readLine();

            namecb.setName(name.length() > 0 ? name : defaultName);

         } else if (callbacks[i] instanceof PasswordCallback) {
            PasswordCallback passwdcb = (PasswordCallback) callbacks[i];
            System.out.println(passwdcb.getPrompt());
            String password = (new BufferedReader(new InputStreamReader(System.in)))
                  .readLine();

            passwdcb.setPassword(password.toCharArray());

         } else {
            System.err.println("Unknown callback type");
         }
      }
   }
}
