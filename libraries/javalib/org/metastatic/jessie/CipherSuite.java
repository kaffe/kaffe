/* CipherSuite.java -- Supported cipher suites.
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


package org.metastatic.jessie;

import java.lang.reflect.Field;

import java.security.Provider;
import java.security.Security;

import java.util.HashMap;
import java.util.Iterator;
import java.util.LinkedList;
import java.util.Set;

public final class CipherSuite
{

  // Constants and fields.
  // -------------------------------------------------------------------------

  // SSL CipherSuites.
  public static final CipherSuite SSL_NULL_WITH_NULL_NULL =
    new CipherSuite("null", "null", "null", 0, 0x00, 0x00);
  public static final CipherSuite SSL_RSA_WITH_NULL_MD5 =
    new CipherSuite("null", "RSA", "HMAC-MD5", 0, 0x00, 0x01);
  public static final CipherSuite SSL_RSA_WITH_NULL_SHA =
    new CipherSuite("null", "RSA", "HMAC-SHA", 0, 0x00, 0x02);
  public static final CipherSuite SSL_RSA_EXPORT_WITH_RC4_40_MD5 =
    new CipherSuite("RC4", "RSA", "HMAC-MD5", 5, 0x00, 0x03);
  public static final CipherSuite SSL_RSA_WITH_RC4_128_MD5 =
    new CipherSuite("RC4", "RSA", "HMAC-MD5", 16, 0x00, 0x04);
  public static final CipherSuite SSL_RSA_WITH_RC4_128_SHA =
    new CipherSuite("RC4", "RSA", "HMAC-SHA", 16, 0x00, 0x05);
  public static final CipherSuite SSL_RSA_EXPORT_WITH_DES40_CBC_SHA =
    new CipherSuite("DES", "RSA", "HMAC-SHA", 5, 0x00, 0x08);
  public static final CipherSuite SSL_RSA_WITH_DES_CBC_SHA =
    new CipherSuite("DES", "RSA", "HMAC-SHA", 8, 0x00, 0x09);
  public static final CipherSuite SSL_RSA_WITH_3DES_EDE_CBC_SHA =
    new CipherSuite("TripleDES", "RSA", "HMAC-SHA", 24, 0x00, 0x0A);
  public static final CipherSuite SSL_DH_DSS_EXPORT_WITH_DES40_CBC_SHA =
    new CipherSuite("DES", "DH_DSS", "HMAC-SHA", 5, 0x00, 0x0B);
  public static final CipherSuite SSL_DH_DSS_WITH_DES_CBC_SHA =
    new CipherSuite("DES", "DH_DSS", "HMAC-SHA", 8, 0x00, 0x0C);
  public static final CipherSuite SSL_DH_DSS_WITH_3DES_EDE_CBC_SHA =
    new CipherSuite("TripleDES", "DH_DSS", "HMAC-SHA", 24, 0x00, 0x0D);
  public static final CipherSuite SSL_DH_RSA_EXPORT_WITH_DES40_CBC_SHA =
    new CipherSuite("DES", "DH_RSA", "HMAC-SHA", 5, 0x00, 0x0E);
  public static final CipherSuite SSL_DH_RSA_WITH_DES_CBC_SHA =
    new CipherSuite("DES", "DH_RSA", "HMAC-SHA", 8, 0x00, 0x0F);
  public static final CipherSuite SSL_DH_RSA_WITH_3DES_EDE_CBC_SHA =
    new CipherSuite("TripleDES", "DH_RSA", "HMAC-SHA", 24, 0x00, 0x10);
  public static final CipherSuite SSL_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA =
    new CipherSuite("DES", "DHE_DSS", "HMAC-SHA", 5, 0x00, 0x11);
  public static final CipherSuite SSL_DHE_DSS_WITH_DES_CBC_SHA =
    new CipherSuite("DES", "DHE_DSS", "HMAC-SHA", 8, 0x00, 0x12);
  public static final CipherSuite SSL_DHE_DSS_WITH_3DES_EDE_CBC_SHA =
    new CipherSuite("TripleDES", "DHE_DSS", "HMAC-SHA", 24, 0x00, 0x13);
  public static final CipherSuite SSL_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA =
    new CipherSuite("DES", "DHE_RSA", "HMAC-SHA", 5, 0x00, 0x14);
  public static final CipherSuite SSL_DHE_RSA_WITH_DES_CBC_SHA =
    new CipherSuite("DES", "DHE_RSA", "HMAC-SHA", 8, 0x00, 0x15);
  public static final CipherSuite SSL_DHE_RSA_WITH_3DES_EDE_CBC_SHA =
    new CipherSuite("TripleDES", "DHE_RSA", "HMAC-SHA", 24, 0x00, 0x16);

  // TLS CipherSuites (the same as SSL CipherSuites);
  // We have these in addition to the SSL_* ones purely for naming reasons.
  public static final CipherSuite TLS_NULL_WITH_NULL_NULL =
    new CipherSuite("null", "null", "null", 0, 0x00, 0x00);
  public static final CipherSuite TLS_RSA_WITH_NULL_MD5 =
    new CipherSuite("null", "RSA", "HMAC-MD5", 0, 0x00, 0x01);
  public static final CipherSuite TLS_RSA_WITH_NULL_SHA =
    new CipherSuite("null", "RSA", "HMAC-SHA", 0, 0x00, 0x02);
  public static final CipherSuite TLS_RSA_EXPORT_WITH_RC4_40_MD5 =
    new CipherSuite("RC4", "RSA", "HMAC-MD5", 5, 0x00, 0x03);
  public static final CipherSuite TLS_RSA_WITH_RC4_128_MD5 =
    new CipherSuite("RC4", "RSA", "HMAC-MD5", 16, 0x00, 0x04);
  public static final CipherSuite TLS_RSA_WITH_RC4_128_SHA =
    new CipherSuite("RC4", "RSA", "HMAC-SHA", 16, 0x00, 0x05);
  public static final CipherSuite TLS_RSA_EXPORT_WITH_DES40_CBC_SHA =
    new CipherSuite("DES", "RSA", "HMAC-SHA", 5, 0x00, 0x08);
  public static final CipherSuite TLS_RSA_WITH_DES_CBC_SHA =
    new CipherSuite("DES", "RSA", "HMAC-SHA", 8, 0x00, 0x09);
  public static final CipherSuite TLS_RSA_WITH_3DES_EDE_CBC_SHA =
    new CipherSuite("TripleDES", "RSA", "HMAC-SHA", 24, 0x00, 0x0A);
  public static final CipherSuite TLS_DH_DSS_EXPORT_WITH_DES40_CBC_SHA =
    new CipherSuite("DES", "DH_DSS", "HMAC-SHA", 5, 0x00, 0x0B);
  public static final CipherSuite TLS_DH_DSS_WITH_DES_CBC_SHA =
    new CipherSuite("DES", "DH_DSS", "HMAC--SHA", 8, 0x00, 0x0C);
  public static final CipherSuite TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA =
    new CipherSuite("TripleDES", "DH_DSS", "HMAC-SHA", 24, 0x00, 0x0D);
  public static final CipherSuite TLS_DH_RSA_EXPORT_WITH_DES40_CBC_SHA =
    new CipherSuite("DES", "DH_RSA", "HMAC-SHA", 5, 0x00, 0x0E);
  public static final CipherSuite TLS_DH_RSA_WITH_DES_CBC_SHA =
    new CipherSuite("DES", "DH_RSA", "HMAC-SHA", 8, 0x00, 0x0F);
  public static final CipherSuite TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA =
    new CipherSuite("TripleDES", "DH_RSA", "HMAC-SHA", 24, 0x00, 0x10);
  public static final CipherSuite TLS_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA =
    new CipherSuite("DES", "DHE_DSS", "HMAC-SHA", 5, 0x00, 0x11);
  public static final CipherSuite TLS_DHE_DSS_WITH_DES_CBC_SHA =
    new CipherSuite("DES", "DHE_DSS", "HMAC-SHA", 8, 0x00, 0x12);
  public static final CipherSuite TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA =
    new CipherSuite("TripleDES", "DHE_DSS", "HMAC-SHA", 24, 0x00, 0x13);
  public static final CipherSuite TLS_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA =
    new CipherSuite("DES", "DHE_RSA", "HMAC-SHA", 5, 0x00, 0x14);
  public static final CipherSuite TLS_DHE_RSA_WITH_DES_CBC_SHA =
    new CipherSuite("DES", "DHE_RSA", "HMAC-SHA", 8, 0x00, 0x15);
  public static final CipherSuite TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA =
    new CipherSuite("TripleDES", "DHE_RSA", "HMAC-SHA", 24, 0x00, 0x16);

  // AES CipherSuites.
  public static final CipherSuite TLS_RSA_WITH_AES_128_CBC_SHA =
    new CipherSuite("AES", "RSA", "HMAC-SHA", 16, 0x00, 0x2F);
  public static final CipherSuite TLS_DH_DSS_WITH_AES_128_CBC_SHA =
    new CipherSuite("AES", "DH_DSS", "HMAC-SHA", 16, 0x00, 0x30);
  public static final CipherSuite TLS_DH_RSA_WITH_AES_128_CBC_SHA =
    new CipherSuite("AES", "DH_RSA", "HMAC-SHA", 16, 0x00, 0x31);
  public static final CipherSuite TLS_DHE_DSS_WITH_AES_128_CBC_SHA =
    new CipherSuite("AES", "DHE_DSS", "HMAC-SHA", 16, 0x00, 0x32);
  public static final CipherSuite TLS_DHE_RSA_WITH_AES_128_CBC_SHA =
    new CipherSuite("AES", "DHE_RSA", "HMAC-SHA", 16, 0x00, 0x33);
  public static final CipherSuite TLS_RSA_WITH_AES_256_CBC_SHA =
    new CipherSuite("AES", "RSA", "HMAC-SHA", 32, 0x00, 0x35);
  public static final CipherSuite TLS_DH_DSS_WITH_AES_256_CBC_SHA =
    new CipherSuite("AES", "DH_DSS", "HMAC-SHA", 32, 0x00, 0x36);
  public static final CipherSuite TLS_DH_RSA_WITH_AES_256_CBC_SHA =
    new CipherSuite("AES", "DH_RSA", "HMAC-SHA", 32, 0x00, 0x37);
  public static final CipherSuite TLS_DHE_DSS_WITH_AES_256_CBC_SHA =
    new CipherSuite("AES", "DHE_DSS", "HMAC-SHA", 32, 0x00, 0x38);
  public static final CipherSuite TLS_DHE_RSA_WITH_AES_256_CBC_SHA =
    new CipherSuite("AES", "DHE_RSA", "HMAC-SHA", 32, 0x00, 0x39);

  private static final HashMap namesToSuites = new HashMap();

  static
  {
    namesToSuites.put("SSL_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA", SSL_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA);
    namesToSuites.put("SSL_DHE_DSS_WITH_3DES_EDE_CBC_SHA", SSL_DHE_DSS_WITH_3DES_EDE_CBC_SHA);
    namesToSuites.put("SSL_DHE_DSS_WITH_DES_CBC_SHA", SSL_DHE_DSS_WITH_DES_CBC_SHA);
    namesToSuites.put("SSL_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA", SSL_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA);
    namesToSuites.put("SSL_DHE_RSA_WITH_3DES_EDE_CBC_SHA", SSL_DHE_RSA_WITH_3DES_EDE_CBC_SHA);
    namesToSuites.put("SSL_DHE_RSA_WITH_DES_CBC_SHA", SSL_DHE_RSA_WITH_DES_CBC_SHA);
    namesToSuites.put("SSL_DH_DSS_EXPORT_WITH_DES40_CBC_SHA", SSL_DH_DSS_EXPORT_WITH_DES40_CBC_SHA);
    namesToSuites.put("SSL_DH_DSS_WITH_3DES_EDE_CBC_SHA", SSL_DH_DSS_WITH_3DES_EDE_CBC_SHA);
    namesToSuites.put("SSL_DH_DSS_WITH_DES_CBC_SHA", SSL_DH_DSS_WITH_DES_CBC_SHA);
    namesToSuites.put("SSL_DH_RSA_EXPORT_WITH_DES40_CBC_SHA", SSL_DH_RSA_EXPORT_WITH_DES40_CBC_SHA);
    namesToSuites.put("SSL_DH_RSA_WITH_3DES_EDE_CBC_SHA", SSL_DH_RSA_WITH_3DES_EDE_CBC_SHA);
    namesToSuites.put("SSL_DH_RSA_WITH_DES_CBC_SHA", SSL_DH_RSA_WITH_DES_CBC_SHA);
    namesToSuites.put("SSL_NULL_WITH_NULL_NULL", SSL_NULL_WITH_NULL_NULL);
    namesToSuites.put("SSL_RSA_EXPORT_WITH_DES40_CBC_SHA", SSL_RSA_EXPORT_WITH_DES40_CBC_SHA);
    namesToSuites.put("SSL_RSA_EXPORT_WITH_RC4_40_MD5", SSL_RSA_EXPORT_WITH_RC4_40_MD5);
    namesToSuites.put("SSL_RSA_WITH_3DES_EDE_CBC_SHA", SSL_RSA_WITH_3DES_EDE_CBC_SHA);
    namesToSuites.put("SSL_RSA_WITH_DES_CBC_SHA", SSL_RSA_WITH_DES_CBC_SHA);
    namesToSuites.put("SSL_RSA_WITH_NULL_MD5", SSL_RSA_WITH_NULL_MD5);
    namesToSuites.put("SSL_RSA_WITH_NULL_SHA", SSL_RSA_WITH_NULL_SHA);
    namesToSuites.put("SSL_RSA_WITH_RC4_128_MD5", SSL_RSA_WITH_RC4_128_MD5);
    namesToSuites.put("SSL_RSA_WITH_RC4_128_SHA", SSL_RSA_WITH_RC4_128_SHA);
    namesToSuites.put("TLS_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA", TLS_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA);
    namesToSuites.put("TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA", TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA);
    namesToSuites.put("TLS_DHE_DSS_WITH_AES_128_CBC_SHA", TLS_DHE_DSS_WITH_AES_128_CBC_SHA);
    namesToSuites.put("TLS_DHE_DSS_WITH_AES_256_CBC_SHA", TLS_DHE_DSS_WITH_AES_256_CBC_SHA);
    namesToSuites.put("TLS_DHE_DSS_WITH_DES_CBC_SHA", TLS_DHE_DSS_WITH_DES_CBC_SHA);
    namesToSuites.put("TLS_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA", TLS_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA);
    namesToSuites.put("TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA", TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA);
    namesToSuites.put("TLS_DHE_RSA_WITH_AES_128_CBC_SHA", TLS_DHE_RSA_WITH_AES_128_CBC_SHA);
    namesToSuites.put("TLS_DHE_RSA_WITH_AES_256_CBC_SHA", TLS_DHE_RSA_WITH_AES_256_CBC_SHA);
    namesToSuites.put("TLS_DHE_RSA_WITH_DES_CBC_SHA", TLS_DHE_RSA_WITH_DES_CBC_SHA);
    namesToSuites.put("TLS_DH_DSS_EXPORT_WITH_DES40_CBC_SHA", TLS_DH_DSS_EXPORT_WITH_DES40_CBC_SHA);
    namesToSuites.put("TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA", TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA);
    namesToSuites.put("TLS_DH_DSS_WITH_AES_128_CBC_SHA", TLS_DH_DSS_WITH_AES_128_CBC_SHA);
    namesToSuites.put("TLS_DH_DSS_WITH_AES_256_CBC_SHA", TLS_DH_DSS_WITH_AES_256_CBC_SHA);
    namesToSuites.put("TLS_DH_DSS_WITH_DES_CBC_SHA", TLS_DH_DSS_WITH_DES_CBC_SHA);
    namesToSuites.put("TLS_DH_RSA_EXPORT_WITH_DES40_CBC_SHA", TLS_DH_RSA_EXPORT_WITH_DES40_CBC_SHA);
    namesToSuites.put("TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA", TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA);
    namesToSuites.put("TLS_DH_RSA_WITH_AES_128_CBC_SHA", TLS_DH_RSA_WITH_AES_128_CBC_SHA);
    namesToSuites.put("TLS_DH_RSA_WITH_AES_256_CBC_SHA", TLS_DH_RSA_WITH_AES_256_CBC_SHA);
    namesToSuites.put("TLS_DH_RSA_WITH_DES_CBC_SHA", TLS_DH_RSA_WITH_DES_CBC_SHA);
    namesToSuites.put("TLS_NULL_WITH_NULL_NULL", TLS_NULL_WITH_NULL_NULL);
    namesToSuites.put("TLS_RSA_EXPORT_WITH_DES40_CBC_SHA", TLS_RSA_EXPORT_WITH_DES40_CBC_SHA);
    namesToSuites.put("TLS_RSA_EXPORT_WITH_RC4_40_MD5", TLS_RSA_EXPORT_WITH_RC4_40_MD5);
    namesToSuites.put("TLS_RSA_WITH_3DES_EDE_CBC_SHA", TLS_RSA_WITH_3DES_EDE_CBC_SHA);
    namesToSuites.put("TLS_RSA_WITH_AES_128_CBC_SHA", TLS_RSA_WITH_AES_128_CBC_SHA);
    namesToSuites.put("TLS_RSA_WITH_AES_256_CBC_SHA", TLS_RSA_WITH_AES_256_CBC_SHA);
    namesToSuites.put("TLS_RSA_WITH_DES_CBC_SHA", TLS_RSA_WITH_DES_CBC_SHA);
    namesToSuites.put("TLS_RSA_WITH_NULL_MD5", TLS_RSA_WITH_NULL_MD5);
    namesToSuites.put("TLS_RSA_WITH_NULL_SHA", TLS_RSA_WITH_NULL_SHA);
    namesToSuites.put("TLS_RSA_WITH_RC4_128_MD5", TLS_RSA_WITH_RC4_128_MD5);
    namesToSuites.put("TLS_RSA_WITH_RC4_128_SHA", TLS_RSA_WITH_RC4_128_SHA);
    SSL_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA.name = "SSL_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA";
    SSL_DHE_DSS_WITH_3DES_EDE_CBC_SHA.name = "SSL_DHE_DSS_WITH_3DES_EDE_CBC_SHA";
    SSL_DHE_DSS_WITH_DES_CBC_SHA.name = "SSL_DHE_DSS_WITH_DES_CBC_SHA";
    SSL_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA.name = "SSL_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA";
    SSL_DHE_RSA_WITH_3DES_EDE_CBC_SHA.name = "SSL_DHE_RSA_WITH_3DES_EDE_CBC_SHA";
    SSL_DHE_RSA_WITH_DES_CBC_SHA.name = "SSL_DHE_RSA_WITH_DES_CBC_SHA";
    SSL_DH_DSS_EXPORT_WITH_DES40_CBC_SHA.name = "SSL_DH_DSS_EXPORT_WITH_DES40_CBC_SHA";
    SSL_DH_DSS_WITH_3DES_EDE_CBC_SHA.name = "SSL_DH_DSS_WITH_3DES_EDE_CBC_SHA";
    SSL_DH_DSS_WITH_DES_CBC_SHA.name = "SSL_DH_DSS_WITH_DES_CBC_SHA";
    SSL_DH_RSA_EXPORT_WITH_DES40_CBC_SHA.name = "SSL_DH_RSA_EXPORT_WITH_DES40_CBC_SHA";
    SSL_DH_RSA_WITH_3DES_EDE_CBC_SHA.name = "SSL_DH_RSA_WITH_3DES_EDE_CBC_SHA";
    SSL_DH_RSA_WITH_DES_CBC_SHA.name = "SSL_DH_RSA_WITH_DES_CBC_SHA";
    SSL_NULL_WITH_NULL_NULL.name = "SSL_NULL_WITH_NULL_NULL";
    SSL_RSA_EXPORT_WITH_DES40_CBC_SHA.name = "SSL_RSA_EXPORT_WITH_DES40_CBC_SHA";
    SSL_RSA_EXPORT_WITH_RC4_40_MD5.name = "SSL_RSA_EXPORT_WITH_RC4_40_MD5";
    SSL_RSA_WITH_3DES_EDE_CBC_SHA.name = "SSL_RSA_WITH_3DES_EDE_CBC_SHA";
    SSL_RSA_WITH_DES_CBC_SHA.name = "SSL_RSA_WITH_DES_CBC_SHA";
    SSL_RSA_WITH_NULL_MD5.name = "SSL_RSA_WITH_NULL_MD5";
    SSL_RSA_WITH_NULL_SHA.name = "SSL_RSA_WITH_NULL_SHA";
    SSL_RSA_WITH_RC4_128_MD5.name = "SSL_RSA_WITH_RC4_128_MD5";
    SSL_RSA_WITH_RC4_128_SHA.name = "SSL_RSA_WITH_RC4_128_SHA";
    TLS_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA.name = "TLS_DHE_DSS_EXPORT_WITH_DES40_CBC_SHA";
    TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA.name = "TLS_DHE_DSS_WITH_3DES_EDE_CBC_SHA";
    TLS_DHE_DSS_WITH_AES_128_CBC_SHA.name = "TLS_DHE_DSS_WITH_AES_128_CBC_SHA";
    TLS_DHE_DSS_WITH_AES_256_CBC_SHA.name = "TLS_DHE_DSS_WITH_AES_256_CBC_SHA";
    TLS_DHE_DSS_WITH_DES_CBC_SHA.name = "TLS_DHE_DSS_WITH_DES_CBC_SHA";
    TLS_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA.name = "TLS_DHE_RSA_EXPORT_WITH_DES40_CBC_SHA";
    TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA.name = "TLS_DHE_RSA_WITH_3DES_EDE_CBC_SHA";
    TLS_DHE_RSA_WITH_AES_128_CBC_SHA.name = "TLS_DHE_RSA_WITH_AES_128_CBC_SHA";
    TLS_DHE_RSA_WITH_AES_256_CBC_SHA.name = "TLS_DHE_RSA_WITH_AES_256_CBC_SHA";
    TLS_DHE_RSA_WITH_DES_CBC_SHA.name = "TLS_DHE_RSA_WITH_DES_CBC_SHA";
    TLS_DH_DSS_EXPORT_WITH_DES40_CBC_SHA.name = "TLS_DH_DSS_EXPORT_WITH_DES40_CBC_SHA";
    TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA.name = "TLS_DH_DSS_WITH_3DES_EDE_CBC_SHA";
    TLS_DH_DSS_WITH_AES_128_CBC_SHA.name = "TLS_DH_DSS_WITH_AES_128_CBC_SHA";
    TLS_DH_DSS_WITH_AES_256_CBC_SHA.name = "TLS_DH_DSS_WITH_AES_256_CBC_SHA";
    TLS_DH_DSS_WITH_DES_CBC_SHA.name = "TLS_DH_DSS_WITH_DES_CBC_SHA";
    TLS_DH_RSA_EXPORT_WITH_DES40_CBC_SHA.name = "TLS_DH_RSA_EXPORT_WITH_DES40_CBC_SHA";
    TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA.name = "TLS_DH_RSA_WITH_3DES_EDE_CBC_SHA";
    TLS_DH_RSA_WITH_AES_128_CBC_SHA.name = "TLS_DH_RSA_WITH_AES_128_CBC_SHA";
    TLS_DH_RSA_WITH_AES_256_CBC_SHA.name = "TLS_DH_RSA_WITH_AES_256_CBC_SHA";
    TLS_DH_RSA_WITH_DES_CBC_SHA.name = "TLS_DH_RSA_WITH_DES_CBC_SHA";
    TLS_NULL_WITH_NULL_NULL.name = "TLS_NULL_WITH_NULL_NULL";
    TLS_RSA_EXPORT_WITH_DES40_CBC_SHA.name = "TLS_RSA_EXPORT_WITH_DES40_CBC_SHA";
    TLS_RSA_EXPORT_WITH_RC4_40_MD5.name = "TLS_RSA_EXPORT_WITH_RC4_40_MD5";
    TLS_RSA_WITH_3DES_EDE_CBC_SHA.name = "TLS_RSA_WITH_3DES_EDE_CBC_SHA";
    TLS_RSA_WITH_AES_128_CBC_SHA.name = "TLS_RSA_WITH_AES_128_CBC_SHA";
    TLS_RSA_WITH_AES_256_CBC_SHA.name = "TLS_RSA_WITH_AES_256_CBC_SHA";
    TLS_RSA_WITH_DES_CBC_SHA.name = "TLS_RSA_WITH_DES_CBC_SHA";
    TLS_RSA_WITH_NULL_MD5.name = "TLS_RSA_WITH_NULL_MD5";
    TLS_RSA_WITH_NULL_SHA.name = "TLS_RSA_WITH_NULL_SHA";
    TLS_RSA_WITH_RC4_128_MD5.name = "TLS_RSA_WITH_RC4_128_MD5";
    TLS_RSA_WITH_RC4_128_SHA.name = "TLS_RSA_WITH_RC4_128_SHA";
  }

  private final String cipherName;

  private final String kexName;

  private final String macName;

  private final boolean exportable;

  private final boolean isStream;

  private final int keyLength;

  private final byte[] id;

  private String name;

  // Constructors.
  // -------------------------------------------------------------------------

  private CipherSuite(String cipherName, String kexName, String macName,
                      int keyLength, int id1, int id2)
  {
    this.cipherName = cipherName;
    this.kexName = kexName;
    this.macName = macName;
    this.exportable = keyLength <= 5;
    this.isStream = cipherName.equals("null") || cipherName.equals("RC4");
    this.keyLength = keyLength;
    this.id = new byte[] { (byte) id1, (byte) id2 };
  }

  // Class methods.
  // -------------------------------------------------------------------------

  /**
   * Returns the cipher suite for the given name, or null if there is no
   * such suite.
   *
   * @return The named cipher suite.
   */
  public static CipherSuite forName(String name)
  {
    return (CipherSuite) namesToSuites.get(name);
  }

  public static Set availableSuiteNames()
  {
    return namesToSuites.keySet();
  }

  // Intance methods.
  // -------------------------------------------------------------------------

  public String getCipher()
  {
    return cipherName;
  }

  public int getKeyLength()
  {
    return keyLength;
  }

  public String getKeyExchange()
  {
    return kexName;
  }

  public String getMac()
  {
    return macName;
  }

  public boolean isExportable()
  {
    return exportable;
  }

  public boolean isStreamCipher()
  {
    return isStream;
  }

  public byte[] getId()
  {
    return (byte[]) id.clone();
  }

  public String toString()
  {
    return name;
  }
}
