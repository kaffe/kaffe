/* DefaultHandler.java -- non-interactive default callback.
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

import java.security.cert.X509Certificate;
import java.util.Locale;

import javax.security.auth.callback.Callback;
import javax.security.auth.callback.ChoiceCallback;
import javax.security.auth.callback.ConfirmationCallback;
import javax.security.auth.callback.LanguageCallback;
import javax.security.auth.callback.NameCallback;
import javax.security.auth.callback.PasswordCallback;
import javax.security.auth.callback.TextInputCallback;

/**
 * This trivial implementation of {@link CertificateHandler} sets its
 * {@link Callback} arguments to default values, with no user interaction.
 */
public class DefaultHandler implements CertificateHandler
{

  // Method.
  // -------------------------------------------------------------------------

  public void handle(Callback[] c)
  {
    if (c == null) return;
    for (int i = 0; i < c.length; i++)
      {
        if (c[i] instanceof ChoiceCallback)
          ((ChoiceCallback) c[i]).setSelectedIndex(((ChoiceCallback) c[i]).getDefaultChoice());
        else if (c[i] instanceof ConfirmationCallback)
          {
            if (((ConfirmationCallback) c[i]).getOptionType() == ConfirmationCallback.YES_NO_OPTION)
              ((ConfirmationCallback) c[i]).setSelectedIndex(ConfirmationCallback.NO);
            else if (((ConfirmationCallback) c[i]).getOptionType() == ConfirmationCallback.YES_NO_CANCEL_OPTION)
              ((ConfirmationCallback) c[i]).setSelectedIndex(ConfirmationCallback.NO);
            else if (((ConfirmationCallback) c[i]).getOptionType() == ConfirmationCallback.OK_CANCEL_OPTION)
              ((ConfirmationCallback) c[i]).setSelectedIndex(ConfirmationCallback.OK);
            else
              ((ConfirmationCallback) c[i]).setSelectedIndex(((ConfirmationCallback) c[i]).getDefaultOption());
          }
        else if (c[i] instanceof LanguageCallback)
          ((LanguageCallback) c[i]).setLocale(Locale.getDefault());
        else if (c[i] instanceof NameCallback)
          ((NameCallback) c[i]).setName(System.getProperty("user.name"));
        else if (c[i] instanceof PasswordCallback)
          ((PasswordCallback) c[i]).setPassword(new char[0]);
        else if (c[i] instanceof TextInputCallback)
          ((TextInputCallback) c[i]).setText("");
      }
  }

  public void setCertificates(X509Certificate[] certs)
  {
  }
}
