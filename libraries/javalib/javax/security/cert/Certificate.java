/* Certificate.java -- base class of public-key certificates.
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


package javax.security.cert;

import java.security.InvalidKeyException;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.PublicKey;
import java.security.SignatureException;

import java.util.Arrays;
import java.util.zip.Adler32;

public abstract class Certificate
{

  // Constructors.
  // -------------------------------------------------------------------------

  public Certificate()
  {
    super();
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  public boolean equals(Object other)
  {
    try
      {
        return Arrays.equals(getEncoded(), ((Certificate) other).getEncoded());
      }
    catch (CertificateEncodingException cee)
      {
        return false;
      }
  }

  public int hashCode()
  {
    try
      {
        Adler32 csum = new Adler32();
        csum.update(getEncoded());
        return (int) csum.getValue();
      }
    catch (CertificateEncodingException cee)
      {
        return 0;
      }
  }

  // Abstract methods.
  // -------------------------------------------------------------------------

  public abstract byte[] getEncoded() throws CertificateEncodingException;

  public abstract void verify(PublicKey key)
    throws CertificateException, NoSuchAlgorithmException, InvalidKeyException,
           NoSuchProviderException, SignatureException;

  public abstract void verify(PublicKey key, String sigProvider)
    throws CertificateException, NoSuchAlgorithmException, InvalidKeyException,
           NoSuchProviderException, SignatureException;

  public abstract String toString();

  public abstract PublicKey getPublicKey();
}
