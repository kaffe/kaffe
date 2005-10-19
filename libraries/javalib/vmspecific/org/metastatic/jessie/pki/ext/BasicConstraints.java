/* BasicConstraints.java -- the basic constraints extension.
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


/* NOTICE: This class is not a permanent part of Jessie! GNU Crypto
   will eventually include a fully-compliant X.509 PKI, at which
   point Jessie will use that instead. That implementation is
   based on this one, so nothing will be lost when these classes
   are obsolete.

   These classes are thus included in Jessie merely as a convenience,
   so there will be a basic PKI implementation available for all free
   runtimes.  Thus you should not use these classes in new programs. */

package org.metastatic.jessie.pki.ext;

import java.io.IOException;
import java.math.BigInteger;
import org.metastatic.jessie.pki.der.DER;
import org.metastatic.jessie.pki.der.DERReader;
import org.metastatic.jessie.pki.der.DERValue;
import org.metastatic.jessie.pki.der.OID;
import org.metastatic.jessie.pki.io.ASN1ParsingException;

public class BasicConstraints extends Extension.Value
{

  // Constants and fields.
  // -------------------------------------------------------------------------

  public static final OID ID = new OID("2.5.29.19");

  private final boolean ca;
  private final int pathLenConstraint;

  // Constructor.
  // -------------------------------------------------------------------------

  public BasicConstraints(final byte[] encoded) throws IOException
  {
    super(encoded);
    DERReader der = new DERReader(encoded);
    DERValue bc = der.read();
    if (!bc.isConstructed())
      throw new ASN1ParsingException("malformed BasicConstraints");
    DERValue val = bc;
    if (bc.getLength() > 0)
      val = der.read();
    if (val.getTag() == DER.BOOLEAN)
      {
        ca = ((Boolean) val.getValue()).booleanValue();
        if (val.getEncodedLength() < bc.getLength())
          val = der.read();
      }
    else
      ca = false;
    if (val.getTag() == DER.INTEGER)
      {
        pathLenConstraint = ((BigInteger) val.getValue()).intValue();
      }
    else
      pathLenConstraint = -1;
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  public boolean isCA()
  {
    return ca;
  }

  public int getPathLengthConstraint()
  {
    return pathLenConstraint;
  }
}
