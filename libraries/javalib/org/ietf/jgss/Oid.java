/* Oid.java -- Object identifier class.
   Copyright (C) 2004  Free Software Foundation, Inc.

This file is a part of GNU Crypto.

GNU Crypto is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

GNU Crypto is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License along
with GNU Crypto; if not, write to the

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


package org.ietf.jgss;

import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.DataInputStream;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;

import java.math.BigInteger;

import java.util.Arrays;
import java.util.StringTokenizer;

/**
 * <p>This class represents Universal Object Identifiers (Oids) and their
 * associated operations.</p>
 *
 * <p>Oids are hierarchically globally-interpretable identifiers used
 * within the GSS-API framework to identify mechanisms and name formats.</p>
 *
 * <p>The structure and encoding of Oids is defined in ISOIEC-8824 and
 * ISOIEC-8825.  For example the Oid representation of Kerberos V5
 * mechanism is "1.2.840.113554.1.2.2".</p>
 *
 * <p>The {@link GSSName} name class contains <code>public static Oid</code>
 * objects representing the standard name types defined in GSS-API.</p>
 */
public class Oid
{

  // Constants and fields.
  // -------------------------------------------------------------------------

  private static final int OBJECT_IDENTIFIER = 0x06;
  private static final int RELATIVE_OID      = 0x0d;

  private final int[] components;
  private byte[] derOid;
  private String strOid;
  private boolean relative;

  // Constructors.
  // -------------------------------------------------------------------------

  /**
   * Creates an Oid object from a string representation of its integer
   * components (e.g. "1.2.840.113554.1.2.2").
   *
   * @param strOid The string representation for the oid.
   * @throws GSSException If the argument is badly formed.
   */
  public Oid(String strOid) throws GSSException
  {
    if (strOid == null)
      throw new NullPointerException();
    this.strOid = strOid;
    try
      {
        StringTokenizer tok = new StringTokenizer(strOid, ".");
        components = new int[tok.countTokens()];
        int i = 0;
        while (tok.hasMoreTokens() && i < components.length)
          {
            components[i++] = Integer.parseInt(tok.nextToken());
          }
      }
    catch (Exception x)
      {
        throw new GSSException(GSSException.FAILURE);
      }
    relative = false;
  }

  /**
   * Creates an Oid object from its DER encoding. This refers to the full
   * encoding including tag and length.  The structure and encoding of
   * Oids is defined in ISOIEC-8824 and ISOIEC-8825.  This method is
   * identical in functionality to its byte array counterpart.
   *
   * @param derOid Stream containing the DER encoded oid.
   * @throws GSSException If the DER stream is badly formed, or if the
   *                      input stream throws an exception.
   */
  public Oid(InputStream derOid) throws GSSException
  {
    DataInputStream in = new DataInputStream(derOid);
    try
      {
        int tag = in.read() & 0x1F;
        if (tag != OBJECT_IDENTIFIER && tag != RELATIVE_OID)
          throw new IOException();
        int len = in.read();
        if ((len & ~0x7F) != 0)
          {
            byte[] buf = new byte[len & 0x7F];
            in.readFully(buf);
            len = new BigInteger(1, buf).intValue();
          }
        if (len < 0)
          throw new IOException();
        byte[] enc = new byte[len];
        in.readFully(enc);
        int[] comp = new int[len + 1];
        int count = 0;
        int i = 0;
        relative = tag == RELATIVE_OID;
        if (!relative && i < len)
          {
            int j = (enc[i] & 0xFF);
            comp[count++] = j / 40;
            comp[count++] = j % 40;
            i++;
          }
        while (i < len)
          {
            int j = 0;
            do
              {
                j = enc[i++] & 0xFF;
                comp[count] <<= 7;
                comp[count]  |= j & 0x7F;
                if (i >= len && (j & 0x80) != 0)
                  throw new IOException();
              }
            while ((j & 0x80) != 0);
            count++;
          }
        if (count == len)
          this.components = comp;
        else
          {
            this.components = new int[count];
            System.arraycopy(comp, 0, components, 0, count);
          }
      }
    catch (IOException ioe)
      {
        throw new GSSException(GSSException.FAILURE);
      }
  }

  /**
   * Creates an Oid object from its DER encoding. This refers to the full
   * encoding including tag and length.  The structure and encoding of
   * Oids is defined in ISOIEC-8824 and ISOIEC-8825.  This method is
   * identical in functionality to its streaming counterpart.
   *
   * @param derOid Byte array storing a DER encoded oid.
   * @throws GSSException If the DER bytes are badly formed.
   */
  public Oid(byte[] derOid) throws GSSException
  {
    this(new ByteArrayInputStream(derOid));
    this.derOid = (byte[]) derOid.clone();
  }

  Oid(int[] components)
  {
    this.components = components;
    relative = false;
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  /**
   * Returns a string representation of the oid's integer components in
   * dot separated notation (e.g. "1.2.840.113554.1.2.2").
   *
   * @return The string representation of this oid.
   */
  public String toString()
  {
    if (strOid == null)
      {
        StringBuffer buf = new StringBuffer();
        for (int i = 0; i < components.length; i++)
          {
            buf.append(components[i]);
            if (i < components.length - 1)
              buf.append('.');
          }
        strOid = buf.toString();
      }
    return strOid;
  }

  /**
   * Returns the full ASN.1 DER encoding for this oid object, which
   * includes the tag and length.
   *
   * @return The ASN.1 DER encoding for this oid.
   * @throws GSSException If encoding fails.
   */
  public byte[] getDER() throws GSSException
  {
    if (derOid == null)
      {
        ByteArrayOutputStream out = new ByteArrayOutputStream(256);
        try
          {
            int i = 0;
            if (!relative)
              {
                int b = components[i++] * 40 + (components.length > 1
                                                ? components[i++] : 0);
                encodeSubId(out, b);
              }
            for ( ; i < components.length; i++)
              encodeSubId(out, components[i]);
            byte[] oid = out.toByteArray();
            out.reset();
            if (relative)
              out.write(RELATIVE_OID);
            else
              out.write(OBJECT_IDENTIFIER);
            if (oid.length < 128)
              out.write(oid.length);
            else if (oid.length < 256)
              {
                out.write(0x81);
                out.write(oid.length);
              }
            else if (oid.length < 65536)
              {
                out.write(0x82);
                out.write((oid.length >>> 8) & 0xFF);
                out.write(oid.length & 0xFF);
              }
            else if (oid.length < 16777216)
              {
                out.write(0x83);
                out.write((oid.length >>> 16) & 0xFF);
                out.write((oid.length >>>  8) & 0xFF);
                out.write(oid.length & 0xFF);
              }
            else
              {
                out.write(0x84);
                out.write((oid.length >>> 24) & 0xFF);
                out.write((oid.length >>> 16) & 0xFF);
                out.write((oid.length >>>  8) & 0xFF);
                out.write(oid.length & 0xFF);
              }
            out.write(oid);
          }
        catch (IOException ioe)
          {
            throw new GSSException(GSSException.FAILURE);
          }
        derOid = out.toByteArray();
      }
    return (byte[]) derOid.clone();
  }

  /**
   * A utility method to test if an Oid object is contained within the
   * supplied Oid object array.
   *
   * @param oids An array of oids to search.
   * @return True if this oid is contained in the given array.
   */
  public boolean containedIn(Oid[] oids)
  {
    for (int i = 0; i < oids.length; i++)
      {
        if (equals(oids[i]))
          return true;
      }
    return false;
  }

  public boolean equals(Object o)
  {
    if (!(o instanceof Oid))
      return false;
    Oid that = (Oid) o;
    return Arrays.equals(components, that.components);
  }

  public int hashCode()
  {
    int code = 0;
    for (int i = 0; i < components.length; i++)
      code += components[i];
    return code;
  }

  // Own methods.
  // -------------------------------------------------------------------------

  private static void encodeSubId(OutputStream out, int id) throws IOException
  {
    if (id < 128)
      {
        out.write(id);
      }
    else if (id < 16384)
      {
        out.write((id >>> 7) | 0x80);
        out.write(id & 0x7F);
      }
    else if (id < 2097152)
      {
        out.write((id >>> 14) | 0x80);
        out.write(((id >>> 7) | 0x80) & 0xFF);
        out.write(id & 0x7F);
      }
    else if (id < 268435456)
      {
        out.write( (id >>> 21) | 0x80);
        out.write(((id >>> 14) | 0x80) & 0xFF);
        out.write(((id >>>  7) | 0x80) & 0xFF);
        out.write(id & 0x7F);
      }
  }
}
