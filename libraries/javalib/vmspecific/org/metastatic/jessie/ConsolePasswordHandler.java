/* ConsolePasswordHandler.java -- simple password prompter.
   Copyright (C) 2003  Casey Marshall <rsdio@metastatic.org>

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
   USA  */


package org.metastatic.jessie;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.io.IOException;

import javax.security.auth.callback.Callback;
import javax.security.auth.callback.CallbackHandler;
import javax.security.auth.callback.PasswordCallback;
import javax.security.auth.callback.UnsupportedCallbackException;

/**
 * A simple callback handler for password callbacks, which prompts the
 * user for a password on the console.
 *
 * <p>This implementation cannot disable echoing of characters, and is
 * meant to be used as a fallback default when no other implementation
 * is available.
 */
public class ConsolePasswordHandler implements CallbackHandler
{

  // Instance method.
  // -------------------------------------------------------------------------

  public void handle(Callback[] callbacks)
    throws IOException, UnsupportedCallbackException
  {
    if (callbacks == null || callbacks.length == 0)
      throw new IllegalArgumentException();
    for (int i = 0; i < callbacks.length; i++)
      {
        if (!(callbacks[i] instanceof PasswordCallback))
          throw new UnsupportedCallbackException(callbacks[i]);
      }
    BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
    for (int i = 0; i < callbacks.length; i++)
      {
        System.out.print(((PasswordCallback) callbacks[i]).getPrompt());
        ((PasswordCallback) callbacks[i]).setPassword(in.readLine().toCharArray());
      }
  }
}
