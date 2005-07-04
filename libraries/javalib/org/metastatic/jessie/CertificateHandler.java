/* CertificateHandler.java -- confirmation for untrusted certificates.
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
import javax.security.auth.callback.CallbackHandler;

/**
 * This extended version of {@link CallbackHandler} can take an array of
 * {@link X509Certificate} objects, so they can be displayed to the user
 * when asking for confirmation.
 */
public interface CertificateHandler extends CallbackHandler
{

  /**
   * Sets the certificate chain in question. This should be used for
   * informational purposes, i.e. to display the certificate to the
   * user, when asking for confirmation.
   *
   * @param chain The chain in question.
   */
  void setCertificates(X509Certificate[] chain);
}
