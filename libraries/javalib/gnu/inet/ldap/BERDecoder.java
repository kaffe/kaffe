/*
 * BERDecoder.java
 * Copyright (C) 2004 The Free Software Foundation
 * 
 * This file is part of GNU inetlib, a library.
 * 
 * GNU inetlib is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * GNU inetlib is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * As a special exception, if you link this library with other files to
 * produce an executable, this library does not by itself cause the
 * resulting executable to be covered by the GNU General Public License.
 * This exception does not however invalidate any other reasons why the
 * executable file might be covered by the GNU General Public License.
 */

package gnu.inet.ldap;

import java.io.UnsupportedEncodingException;

/**
 * Utility class for decoding BER values.
 * For each value to be read, the application must call <code>parseType</code>
 * to return the type of the value, then the specific
 * <code>parse<i>XXX</i></code> method for the type to return the actual
 * value, or <code>skip</code> to skip the value.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class BERDecoder
{

  private byte[] buffer;
  private int offset;
  private int type;
  private int len;
  private boolean control;
  private boolean utf8;

  public BERDecoder (byte[] data, boolean utf8)
  {
    buffer = data;
    offset = 0;
    control = true;
    this.utf8 = utf8;
  }

  /**
   * Returns the type of the current value record.
   * If there are no more records to read, this method returns -1.
   */
  public int parseType ()
    throws BERException
  {
    if (offset >= buffer.length)
      {
        return -1;
      }
    type = (int) buffer[offset++];
    len = (int) buffer[offset++];
    if ((len & 0x80) != 0)
      {
        int lsize = len - 0x80;
        if (lsize > 4)
          {
            throw new BERException ("Data too long: " + lsize);
          }
        if (buffer.length - offset < lsize)
          {
            throw new BERException ("Insufficient data");
          }
        len = 0;
        for (int i = 0; i < lsize; i++)
          {
            len = (len << 8) + (((int) buffer[offset++]) & 0xff);
          }
        if (buffer.length - offset < len)
          {
            throw new BERException ("Insufficient data");
          }
      }
    control = false;
    return type;
  }

  public boolean available ()
  {
    return (offset < buffer.length);
  }

  public void skip ()
  {
    offset += len;
    control = true;
  }

  public boolean parseBoolean ()
    throws BERException
  {
    if (control)
      {
        parseType ();
      }
    if (type != BERConstants.BOOLEAN)
      {
        throw new BERException ("Unexpected type: " + type);
      }
    int c = (int) buffer[offset++];
    control = true;
    return (c != 0);
  }

  public int parseInt ()
    throws BERException
  {
    if (control)
      {
        parseType ();
      }
    if (type != BERConstants.INTEGER && type != BERConstants.ENUMERATED)
      {
        throw new BERException ("Unexpected type: " + type);
      }
    byte c = buffer[offset++];
    int val = ((int) c) & 0x7f;
    for (int i = 1; i < len; i++)
      {
        val <<= 0x08;
        val |= ((int) buffer[offset++]) & 0xff;
      }
    if ((c & 0x80) != 0)
      {
        val = -val;
      }
    control = true;
    return val;
  }

  public String parseString ()
    throws BERException
  {
    if (control)
      {
        parseType ();
      }
    if (len == 0)
      {
        control = true;
        return "";
      }
    if (type != BERConstants.UTF8_STRING && type != BERConstants.OCTET_STRING)
      {
        throw new BERException ("Unexpected type: " + type);
      }
    String encoding = (type == BERConstants.UTF8_STRING) ? "UTF-8" :
      "ISO-8859-1";
    try
      {
        String ret = new String (buffer, offset, len, encoding);
        offset += len;
        control = true;
        return ret;
      }
    catch (UnsupportedEncodingException e)
      {
        throw new BERException ("JVM does not support " + encoding);
      }
  }

  public byte[] parseOctetString ()
    throws BERException
  {
    if (control)
      {
        parseType ();
      }
    if (type != BERConstants.OCTET_STRING)
      {
        throw new BERException ("Unexpected type: " + type);
      }
    byte[] ret = new byte[len];
    System.arraycopy (buffer, offset, ret, 0, len);
    offset += len;
    control = true;
    return ret;
  }

  public BERDecoder parseSequence ()
    throws BERException
  {
    return parseSequence (BERConstants.SEQUENCE);
  }
  
  public BERDecoder parseSequence (int code)
    throws BERException
  {
    if (control)
      {
        parseType ();
      }
    if (code != -1 && type != code)
      {
        throw new BERException ("Unexpected type: " + type);
      }
    byte[] ret = new byte[len];
    System.arraycopy (buffer, offset, ret, 0, len);
    offset += len;
    control = true;
    return new BERDecoder (ret, utf8);
  }
  
  public BERDecoder parseSet ()
    throws BERException
  {
    return parseSequence (BERConstants.SET);
  }
  
}
