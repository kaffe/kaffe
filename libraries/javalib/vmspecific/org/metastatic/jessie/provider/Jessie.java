/* Jessie.java -- JESSIE's JSSE provider.
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
   51 Franklin Street, Fifth Floor,
   Boston, MA  02110-1301
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


package org.metastatic.jessie.provider;

import java.security.AccessController;
import java.security.PrivilegedAction;
import java.security.Provider;

/**
 * This is the security provider for Jessie. It implements the following
 * algorithms:
 *
 * <pre>
 * {@link javax.net.ssl.SSLContext}.SSLv3
 * {@link javax.net.ssl.SSLContext}.SSL
 * {@link javax.net.ssl.SSLContext}.TLSv1
 * {@link javax.net.ssl.SSLContext}.TLS
 * {@link javax.net.ssl.KeyManagerFactory}.JessieX509
 * {@link javax.net.ssl.TrustManagerFactory}.JessieX509
 * {@link javax.net.ssl.TrustManagerFactory}.SRP
 * {@link java.security.SecureRandom}.CSPRNG
 * </pre>
 *
 */
public class Jessie extends Provider
{

  public static final String VERSION = "1.0.0";
  public static final double VERSION_DOUBLE = 1.0;

  public Jessie()
  {
    super("Jessie", VERSION_DOUBLE,
          "Implementing SSLv3, TLSv1 SSL Contexts; X.509 Key Manager Factories;" +
          System.getProperty("line.separator") +
          "X.509 and SRP Trust Manager Factories, continuously-seeded secure random." );

    AccessController.doPrivileged(new PrivilegedAction()
      {
        public Object run()
        {
          put("SSLContext.SSLv3", Context.class.getName());
          put("Alg.Alias.SSLContext.SSL",     "SSLv3");
          put("Alg.Alias.SSLContext.TLSv1",   "SSLv3");
          put("Alg.Alias.SSLContext.TLS",     "SSLv3");
          //put("Alg.Alias.SSLContext.TLSv1.1", "SSLv3");

          put("KeyManagerFactory.JessieX509",   X509KeyManagerFactory.class.getName());
          put("TrustManagerFactory.JessieX509", X509TrustManagerFactory.class.getName());
          put("TrustManagerFactory.SRP",        SRPTrustManagerFactory.class.getName());

          put("SecureRandom.CSPRNG", CSPRNGSpi.class.getName());

          put("AlgorithmParameters.DSA", org.metastatic.jessie.pki.provider.DSAParameters.class.getName());
          put("Alg.Alias.AlgorithmParameters.DSS", "DSA");
          put("Alg.Alias.AlgorithmParameters.SHAwithDSA", "DSA");
          put("Alg.Alias.AlgorithmParameters.OID.1.2.840.10040.4.3", "DSA");
          put("Alg.Alias.AlgorithmParameters.1.2.840.10040.4.3", "DSA");

          put("CertificateFactory.X509", org.metastatic.jessie.pki.provider.X509CertificateFactory.class.getName());
          put("CertificateFactory.X509 ImplementedIn", "Software");
          put("CertificateFactory.X.509", org.metastatic.jessie.pki.provider.X509CertificateFactory.class.getName());
          put("CertificateFactory.X.509 ImplementedIn", "Software");

          put("KeyFactory.Encoded", org.metastatic.jessie.pki.provider.EncodedKeyFactory.class.getName());
          put("KeyFactory.Encoded ImplementedIn", "Software");
          put("KeyFactory.X.509", org.metastatic.jessie.pki.provider.EncodedKeyFactory.class.getName());
          put("KeyFactory.X509", org.metastatic.jessie.pki.provider.EncodedKeyFactory.class.getName());
          put("KeyFactory.PKCS#8", org.metastatic.jessie.pki.provider.EncodedKeyFactory.class.getName());
          put("KeyFactory.PKCS8", org.metastatic.jessie.pki.provider.EncodedKeyFactory.class.getName());

          put("KeyFactory.RSA", org.metastatic.jessie.pki.provider.RSAKeyFactory.class.getName());

          put("MessageDigest.MD2", org.metastatic.jessie.pki.provider.MD2.class.getName());
          put("MessageDigest.MD2 ImplementedIn", "Software");
          put("Alg.Alias.MessageDigest.OID.1.2.840.11359.2.2", "MD2");
          put("Alg.Alias.MessageDigest.1.2.840.11359.2.2", "MD2");

          put("MessageDigest.MD5", org.metastatic.jessie.pki.provider.MD5.class.getName());
          put("MessageDigest.MD5 ImplementedIn", "Software");
          put("Alg.Alias.MessageDigest.OID.1.2.840.11359.2.5", "MD5");
          put("Alg.Alias.MessageDigest.1.2.840.11359.2.5", "MD5");

          put("MessageDigest.SHA1", org.metastatic.jessie.pki.provider.SHA1.class.getName());
          put("MessageDigest.SHA1 ImplementedIn", "Software");
          put("Alg.Alias.MessageDigest.SHA", "SHA1");
          put("Alg.Alias.MessageDigest.SHA-1", "SHA1");
          put("Alg.Alias.MessageDigest.OID.1.3.14.3.2.26", "SHA1");
          put("Alg.Alias.MessageDigest.1.3.14.3.2.26", "SHA1");

          put("Signature.DSA", org.metastatic.jessie.pki.provider.DSASignature.class.getName());
          put("Alg.Alias.Signature.DSS", "DSA");
          put("Alg.Alias.Signature.SHAwithDSA", "DSA");
          put("Alg.Alias.Signature.OID.1.2.840.10040.4.3", "DSA");
          put("Alg.Alias.Signature.1.2.840.10040.4.3", "DSA");

          put("Signature.MD2withRSA", org.metastatic.jessie.pki.provider.MD2withRSA.class.getName());
          put("Signature.MD2withRSA ImplementedIn", "Software");
          put("Alg.Alias.Signature.md2WithRSAEncryption", "MD2withRSA");
          put("Alg.Alias.Signature.OID.1.2.840.113549.1.1.2", "MD2withRSA");
          put("Alg.Alias.Signature.1.2.840.113549.1.1.2", "MD2withRSA");

          put("Signature.MD5withRSA", org.metastatic.jessie.pki.provider.MD5withRSA.class.getName());
          put("Signature.MD5withRSA ImplementedIn", "Software");
          put("Alg.Alias.Signature.md5WithRSAEncryption", "MD5withRSA");
          put("Alg.Alias.Signature.OID.1.2.840.113549.1.1.4", "MD5withRSA");
          put("Alg.Alias.Signature.1.2.840.113549.1.1.4", "MD5withRSA");

          put("Signature.SHA1withRSA", org.metastatic.jessie.pki.provider.SHA1withRSA.class.getName());
          put("Signature.SHA1withRSA ImplementedIn", "Software");
          put("Alg.Alias.Signature.sha-1WithRSAEncryption", "SHA1withRSA");
          put("Alg.Alias.Signature.OID.1.2.840.113549.1.1.5", "SHA1withRSA");
          put("Alg.Alias.Signature.1.2.840.113549.1.1.5", "SHA1withRSA");

          return null;
        }
      });
  }
}
