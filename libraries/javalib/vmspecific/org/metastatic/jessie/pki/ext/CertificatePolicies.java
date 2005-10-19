/* CertificatePolicies.java -- certificate policy extension.
   Copyright (C) 2003  Casey Marshall <csm@metastatic.org>

This file is part of GNU PKI, a PKI library.

GNU PKI is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

GNU PKI is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with GNU PKI; see the file COPYING.  If not, write to the Free
Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
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


package org.metastatic.jessie.pki.ext;

import java.io.IOException;
import java.math.BigInteger;
// import java.security.cert.PolicyQualifierInfo;
import java.util.Collections;
import java.util.HashMap;
import java.util.LinkedList;
import java.util.List;
import java.util.Map;

import org.metastatic.jessie.pki.der.DER;
import org.metastatic.jessie.pki.der.DERReader;
import org.metastatic.jessie.pki.der.DERValue;
import org.metastatic.jessie.pki.der.OID;
import org.metastatic.jessie.pki.io.ASN1ParsingException;

public class CertificatePolicies extends Extension.Value
{

  // Constants and fields.
  // -------------------------------------------------------------------------

  public static final OID ID = new OID("2.5.29.32");

  private final List policies;
  private final Map policyQualifierInfos;

  // Constructor.
  // -------------------------------------------------------------------------

  public CertificatePolicies(final byte[] encoded) throws IOException
  {
    super(encoded);
    DERReader der = new DERReader(encoded);
    DERValue pol = der.read();
    if (!pol.isConstructed())
      throw new ASN1ParsingException("malformed CertificatePolicies");

    int len = 0;
    LinkedList policyList = new LinkedList();
    HashMap qualifierMap = new HashMap();
    while (len < pol.getLength())
      {
        DERValue policyInfo = der.read();
        if (!policyInfo.isConstructed())
          throw new ASN1ParsingException("malformed PolicyInformation");
        DERValue val = der.read();
        if (val.getTag() != DER.OBJECT_IDENTIFIER)
          throw new ASN1ParsingException("malformed CertPolicyId");
        OID policyId = (OID) val.getValue();
        policyList.add(policyId);
        if (val.getEncodedLength() < policyInfo.getLength())
          {
            DERValue qual = der.read();
            int len2 = 0;
            LinkedList quals = new LinkedList();
            while (len2 < qual.getLength())
              {
                val = der.read();
//                 quals.add(new PolicyQualifierInfo(val.getEncoded()));
                quals.add (val.getEncoded());
                der.skip(val.getLength());
                len2 += val.getEncodedLength();
              }
            qualifierMap.put(policyId, quals);
          }
        len += policyInfo.getEncodedLength();
      }

    policies = Collections.unmodifiableList(policyList);
    policyQualifierInfos = Collections.unmodifiableMap(qualifierMap);
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  public List getPolicies()
  {
    return policies;
  }

  public List getPolicyQualifierInfos(OID oid)
  {
    return (List) policyQualifierInfos.get(oid);
  }
}
