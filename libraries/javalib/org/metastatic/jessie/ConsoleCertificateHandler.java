/* ConsoleCertificateHandler.java -- handle confirmations on the console.
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

import java.security.cert.X509Certificate;

import javax.security.auth.callback.Callback;
import javax.security.auth.callback.ConfirmationCallback;
import javax.security.auth.callback.UnsupportedCallbackException;

/**
 * This implementation of {@link CertificateHandler} asks for confirmation
 * from the console, and reads replies from {@link java.lang.System#in}.
 */
public class ConsoleCertificateHandler implements CertificateHandler
{

  // Fields.
  // -------------------------------------------------------------------------

  protected X509Certificate[] chain;

  // Default constructor.

  // Instance methods.
  // -------------------------------------------------------------------------

  public void handle(Callback[] callbacks)
    throws UnsupportedCallbackException, IOException
  {
    if (callbacks == null || callbacks.length == 0)
      throw new IllegalArgumentException();
    for (int i = 0; i < callbacks.length; i++)
      {
        if (!(callbacks[i] instanceof ConfirmationCallback))
          throw new UnsupportedCallbackException(callbacks[i]);
        if (((ConfirmationCallback) callbacks[i]).getOptionType() !=
            ConfirmationCallback.YES_NO_OPTION)
          throw new UnsupportedCallbackException(callbacks[i]);
      }
    BufferedReader in = new BufferedReader(new InputStreamReader(System.in));
    for (int i = 0; i < callbacks.length; i++)
      {
        ConfirmationCallback confirm = (ConfirmationCallback) callbacks[i];
        System.out.println(confirm.getPrompt());
        System.out.print("Enter `yes' or `no': ");
        String line = in.readLine();
        if (line == null)
          confirm.setSelectedIndex(confirm.getDefaultOption());
        else if (line.trim().equalsIgnoreCase("yes") ||
                 line.trim().equalsIgnoreCase("y"))
          confirm.setSelectedIndex(ConfirmationCallback.YES);
        else if (line.trim().equalsIgnoreCase("no") ||
                 line.trim().equalsIgnoreCase("n"))
          confirm.setSelectedIndex(ConfirmationCallback.NO);
        else
          confirm.setSelectedIndex(confirm.getDefaultOption());
      }
  }

  public void setCertificates(X509Certificate[] chain)
  {
    this.chain = chain;
  }
}
