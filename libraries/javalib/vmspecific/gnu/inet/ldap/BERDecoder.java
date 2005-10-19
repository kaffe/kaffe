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
 * Linking this library statically or dynamically with other modules is
 * making a combined work based on this library.  Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * As a special exception, the copyright holders of this library give you
 * permission to link this library with independent modules to produce an
 * executable, regardless of the license terms of these independent
 * modules, and to copy and distribute the resulting executable under
 * terms of your choice, provided that you also meet, for each linked
 * independent module, the terms and conditions of the license of that
 * module.  An independent module is a module which is not derived from
 * or based on this library.  If you modify this library, you may extend
 * this exception to your version of the library, but you are not
 * obliged to do so.  If you do not wish to do so, delete this
 * exception statement from your version.
 */

package gnu.inet.ldap;

import java.io.ByteArrayOutputStream;
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

  public BERDecoder(byte[] data, boolean utf8)
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
  public int parseType()
    throws BERException
  {
    if (offset >= buffer.length)
      {
        return -1;
      }
    type = byteToInt(buffer[offset++]);
    len = byteToInt(buffer[offset++]);
    if ((len & 0x80) != 0)
      {
        int lsize = len - 0x80;
        if (lsize > 4)
          {
            throw new BERException("Data too long: " + lsize);
          }
        if (buffer.length - offset < lsize)
          {
            throw new BERException("Insufficient data");
          }
        len = 0;
        for (int i = 0; i < lsize; i++)
          {
            len = (len << 8) + byteToInt(buffer[offset++]);
          }
        if (buffer.length - offset < len)
          {
            throw new BERException("Insufficient data");
          }
      }
    control = false;
    return type;
  }

  static int byteToInt(byte b)
  {
    int ret = (int) b;
    if (ret < 0)
      {
        ret += 0x100;
      }
    return ret;
  }

  int getLength()
  {
    return len;
  }

  public boolean available()
  {
    return (offset < buffer.length);
  }

  public void skip()
  {
    offset += len;
    control = true;
  }

  public boolean parseBoolean()
    throws BERException
  {
    if (control)
      {
        parseType();
      }
    if (type != BERConstants.BOOLEAN)
      {
        throw new BERException("Unexpected type: " + type);
      }
    int c = (int) buffer[offset++];
    control = true;
    return (c != 0);
  }

  public int parseInt()
    throws BERException
  {
    if (control)
      {
        parseType();
      }
    if (type != BERConstants.INTEGER && type != BERConstants.ENUMERATED)
      {
        throw new BERException("Unexpected type: " + type);
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

  public String parseString()
    throws BERException
  {
    if (control)
      {
        parseType();
      }
    if (len == 0)
      {
        control = true;
        return "";
      }
    if (type != BERConstants.UTF8_STRING && type != BERConstants.OCTET_STRING)
      {
        throw new BERException("Unexpected type: " + type);
      }
    String encoding = (type == BERConstants.UTF8_STRING) ? "UTF-8" :
      "ISO-8859-1";
    try
      {
        String ret = new String(buffer, offset, len, encoding);
        offset += len;
        control = true;
        return ret;
      }
    catch(UnsupportedEncodingException e)
      {
        throw new BERException("JVM does not support " + encoding);
      }
  }

  public byte[] parseOctetString()
    throws BERException
  {
    if (control)
      {
        parseType();
      }
    if (type != BERConstants.OCTET_STRING)
      {
        throw new BERException("Unexpected type: " + type);
      }
    byte[] ret = new byte[len];
    System.arraycopy(buffer, offset, ret, 0, len);
    offset += len;
    control = true;
    return ret;
  }

  public BERDecoder parseSequence()
    throws BERException
  {
    return parseSequence(BERConstants.SEQUENCE);
  }
  
  public BERDecoder parseSequence(int code)
    throws BERException
  {
    if (control)
      {
        parseType();
      }
    if (code != -1 && type != code)
      {
        throw new BERException("Unexpected type: " + type);
      }
    byte[] ret = new byte[len];
    System.arraycopy(buffer, offset, ret, 0, len);
    offset += len;
    control = true;
    return new BERDecoder(ret, utf8);
  }
  
  public BERDecoder parseSet()
    throws BERException
  {
    return parseSet(BERConstants.SET);
  }

  public BERDecoder parseSet(int code)
    throws BERException
  {
    return parseSequence(code);
  }

  public static void main(String[] args)
  {
    try
      {
        ByteArrayOutputStream out = new ByteArrayOutputStream();
        for (int c = System.in.read(); c != -1; c = System.in.read())
          {
            out.write(c);
          }
        byte[] code = out.toByteArray();
        BERDecoder decoder = new BERDecoder(code, true);
        debug(decoder, 0);
      }
    catch (Exception e)
      {
        e.printStackTrace(System.err);
      }
  }

  private static void debug(BERDecoder decoder, int depth)
    throws BERException
  {
    for (int t = decoder.parseType(); t != -1; t = decoder.parseType())
      {
        for (int i = 0; i < depth; i++)
          {
            System.out.print('\t');
          } 
        switch (t)
          {
          case BERConstants.BOOLEAN:
            System.out.println("BOOLEAN: " + decoder.parseBoolean());
            break;
          case BERConstants.INTEGER:
            System.out.println("INTEGER: " + decoder.parseInt());
            break;
          case BERConstants.ENUMERATED:
            System.out.println("ENUMERATED: " + decoder.parseInt());
            break;
          case BERConstants.OCTET_STRING:
            System.out.println("OCTET-STRING: " +
                                toString(decoder.parseOctetString()));
            break;
          case BERConstants.UTF8_STRING:
            System.out.println("STRING: \"" + decoder.parseString() + "\"");
            break;
          default:
            System.out.println("SEQUENCE " + t + "(0x" +
                                Integer.toHexString(t) + "): " +
                                decoder.getLength());
            BERDecoder sequence = decoder.parseSequence(t);
            debug(sequence, depth + 1);            
            break;
          }
      }
  }

  private static String toString(byte[] bytes)
  {
    try
      {
        return "\"" + new String(bytes, "UTF-8") + "\"";
      }
    catch (UnsupportedEncodingException e)
      {
        return bytes.toString();
      }
  }
  
}
