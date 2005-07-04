/* X509CertificateBuilder.java -- Mutable X.509 certificate.
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
along with GNU Crypto; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */


package gnu.crypto.pki;

import gnu.crypto.der.*;
import gnu.crypto.pki.ext.*;

import java.io.IOException;
import java.math.BigInteger;

import java.security.InvalidKeyException;
import java.security.KeyFactory;
import java.security.NoSuchAlgorithmException;
import java.security.NoSuchProviderException;
import java.security.PrivateKey;
import java.security.PublicKey;
import java.security.Signature;
import java.security.SignatureException;
import java.security.cert.CertificateEncodingException;
import java.security.cert.CertificateException;
import java.security.cert.X509Certificate;

import java.util.ArrayList;
import java.util.Date;
import java.util.Iterator;
import java.util.List;

import javax.security.auth.x500.X500Principal;

public final class X509CertificateBuilder extends X509CertificateImpl
{

  private static final long ONE_YEAR = 31536000000L;

  // Certificate.
//   private transient byte[] tbsCertBytes;
//   private transient int version;
//   private transient BigInteger serialNo;
//   private transient OID algId;
//   private transient byte[] algVal;
//   private transient X500Name issuer;
//   private transient Date notBefore;
//   private transient Date notAfter;
//   private transient X500Name subject;
//   private transient PublicKey subjectKey;
//   private transient BitString issuerUniqueId;
//   private transient BitString subjectUniqueId;
//   private transient Map extensions;

  // Signature.
//   private transient OID sigAlgId;
//   private transient byte[] sigAlgVal;
//   private transient byte[] signature;

  // Constructors.
  // -------------------------------------------------------------------------

  public X509CertificateBuilder()
  {
    super();
    version = 3;
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  public byte[] getEncoded() throws CertificateEncodingException
  {
    if (encoded == null)
      throw new CertificateEncodingException ("not encoded");
    return (byte[]) encoded.clone();
  }

  public Date getNotBefore()
  {
    if (notBefore == null)
      throw new IllegalStateException ("notBefore not set");
    return (Date) notBefore.clone();
  }

  public Date getNotAfter()
  {
    if (notAfter == null)
      throw new IllegalStateException ("notAfter not set");
    return (Date) notAfter.clone();
  }

  public byte[] getSignature()
  {
    if (signature != null)
      throw new IllegalStateException ("certificate has not been signed");
    return (byte[]) signature.clone();
  }

  public byte[] getTBSCertificate() throws CertificateEncodingException
  {
    if (tbsCertBytes == null)
      throw new CertificateEncodingException ("not encoded");
    return (byte[]) tbsCertBytes.clone();
  }

  public void setVersion (int version)
  {
    if (version <= 0 || version > 3)
      throw new IllegalArgumentException ("invalid version: " + version);
    this.version = version;
    encoded = null;
  }

  public void setSerialNumber (BigInteger serialNo)
  {
    if (serialNo.signum() < 0 || serialNo.equals (BigInteger.ZERO))
      throw new IllegalArgumentException ("serial numbers must be positive");
    this.serialNo = serialNo;
    encoded = null;
  }

  public void setIssuer (X500Name issuer)
  {
    this.issuer = issuer;
    encoded = null;
  }

  public void setIssuer (X500Principal issuer) throws IOException
  {
    this.issuer = new X500Name (issuer.getEncoded());
    encoded = null;
  }

  public void setNotBefore (Date notBefore)
  {
    this.notBefore = notBefore;
    encoded = null;
  }

  public void setNotAfter (Date notAfter)
  {
    this.notAfter = notAfter;
    encoded = null;
  }

  public void setSubject (X500Name subject)
  {
    this.subject = subject;
    encoded = null;
  }

  public void setSubject (X500Principal subject) throws IOException
  {
    this.subject = new X500Name (subject.getEncoded());
    encoded = null;
  }

  public void setPublicKey (PublicKey subjectKey)
  {
    this.subjectKey = subjectKey;
    encoded = null;
  }

  public void setIssuerUniqueId (BitString issuerUniqueId)
  {
    this.issuerUniqueId = issuerUniqueId;
    encoded = null;
  }

  public void setSubjectUniqueId (BitString subjectUniqueId)
  {
    this.subjectUniqueId = subjectUniqueId;
    encoded = null;
  }

  public void addExtension (Extension extension)
  {
    extensions.put (extension.getOid(), extension);
    encoded = null;
  }

  public void setSigAlg (String sigAlg)
  {
    if (sigAlg.equals ("DSAwithSHA1") || sigAlg.equals ("DSS"))
      sigAlgId = ID_DSA_WITH_SHA1;
    else if (sigAlg.equals ("MD2withRSA"))
      sigAlgId = ID_RSA_WITH_MD2;
    else if (sigAlg.equals ("MD5withRSA"))
      sigAlgId = ID_RSA_WITH_MD5;
    else if (sigAlg.equals ("SHA1withRSA"))
      sigAlgId = ID_RSA_WITH_SHA1;
    else
      sigAlgId = new OID (sigAlg);
  }

  public void setSigParams (byte[] sigAlgVal)
  {
    this.sigAlgVal = (sigAlgVal == null ? null : (byte[]) sigAlgVal.clone());
    encoded = null;
  }

  /**
   * Signs this certificate, encoding it and preparing for a call to
   * {@link getEncoded()}.
   */
  public void sign (PrivateKey key, String sigAlg, String provider)
    throws CertificateException, InvalidKeyException, NoSuchAlgorithmException,
           NoSuchProviderException, SignatureException
  {
    setSigAlg (sigAlg);
    Signature sig = Signature.getInstance (sigAlg, provider);
    sig.initSign (key);
    try
      {
        encoded = sign (sig);
      }
    catch (IOException ioe)
      {
        CertificateEncodingException cee = new CertificateEncodingException();
        cee.initCause (ioe);
        throw cee;
      }
  }

  public void sign (PrivateKey key, String sigAlg)
    throws CertificateException, InvalidKeyException, NoSuchAlgorithmException,
           SignatureException
  {
    setSigAlg (sigAlg);
    Signature sig = Signature.getInstance (sigAlg);
    sig.initSign (key);
    try
      {
        encoded = sign (sig);
      }
    catch (IOException ioe)
      {
        CertificateEncodingException cee = new CertificateEncodingException();
        cee.initCause (ioe);
        throw cee;
      }
  }

  private byte[] sign (Signature sig)
    throws CertificateException, IOException, InvalidKeyException,
           NoSuchAlgorithmException, SignatureException
  {
    List cert = new ArrayList (3);
    List tbsCert = new ArrayList (9);
    if (version != 1)
      tbsCert.add (new DERValue (DER.CONSTRUCTED|DER.APPLICATION,
                                 BigInteger.valueOf ((long) (version - 1))));

    if (serialNo == null)
      throw new CertificateException ("no serial number");
    tbsCert.add (new DERValue (DER.INTEGER, serialNo));

    List algId = new ArrayList (2);
    algId.add (new DERValue (DER.OBJECT_IDENTIFIER, sigAlgId));
    if (sigAlgVal != null)
      algId.add (DERReader.read (sigAlgVal));
    else
      algId.add (new DERValue (DER.NULL, null));
    DERValue algIdVal = new DERValue (DER.CONSTRUCTED|DER.SEQUENCE, algId);
    tbsCert.add (algIdVal);

    if (issuer == null)
      throw new CertificateException ("no issuer");
    tbsCert.add (DERReader.read (issuer.getDer()));

    if (notBefore == null)
      notBefore = new Date();
    if (notAfter == null)
      notAfter = new Date (notBefore.getTime() + ONE_YEAR);
    List validity = new ArrayList (2);
    validity.add (new DERValue (DER.GENERALIZED_TIME, notBefore));
    validity.add (new DERValue (DER.GENERALIZED_TIME, notAfter));
    tbsCert.add (new DERValue (DER.CONSTRUCTED|DER.SEQUENCE, validity));

    if (subject == null)
      throw new CertificateException ("no subject");
    tbsCert.add (DERReader.read (subject.getDer()));

    if (subjectKey == null)
      throw new CertificateException ("no public key");
    if (!"X.509".equals (subjectKey.getFormat()))
      {
        KeyFactory factory = KeyFactory.getInstance (subjectKey.getAlgorithm());
        PublicKey key2 = (PublicKey) factory.translateKey (subjectKey);
        tbsCert.add (DERReader.read (key2.getEncoded()));
      }
    else
      tbsCert.add (DERReader.read (subjectKey.getEncoded()));

    if (issuerUniqueId != null)
      {
        if (version < 2)
          throw new CertificateException ("issuerUniqueId is only valid for version 2 or 3");
        tbsCert.add (new DERValue (DER.APPLICATION|1, issuerUniqueId));
      }

    if (subjectUniqueId != null)
      {
        if (version < 2)
          throw new CertificateException ("subjectUniqueId is only valid for version 2 or 3");
        tbsCert.add (new DERValue (DER.APPLICATION|2, subjectUniqueId));
      }

    if (extensions.size() > 0 && version < 3)
      throw new CertificateException ("extensions are only valid for version 3");
    if (extensions.size() > 0)
      {
        List exts = new ArrayList (extensions.size());
        for (Iterator it = extensions.values().iterator(); it.hasNext(); )
          {
            Extension e = (Extension) it.next();
            exts.add (e.getDerValue());
          }
        tbsCert.add (new DERValue (DER.APPLICATION|3, exts));
      }

    DERValue tbsCertVal = new DERValue (DER.CONSTRUCTED|DER.SEQUENCE, tbsCert);

    tbsCertBytes = tbsCertVal.getEncoded();
    sig.update (tbsCertBytes);
    signature = sig.sign();

    cert.add (tbsCertVal);
    cert.add (algIdVal);
    cert.add (new DERValue (DER.OCTET_STRING, signature));

    return new DERValue (DER.CONSTRUCTED|DER.SEQUENCE, cert).getEncoded();
  }
}
