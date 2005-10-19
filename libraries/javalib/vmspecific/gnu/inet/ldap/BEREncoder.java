/*
 * BEREncoder.java
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

import java.io.UnsupportedEncodingException;

/**
 * Utility class to construct BER-encoded data.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class BEREncoder
{

  private byte[] buffer;
  private int offset;
  private int[] sequenceOffset;
  private int sequenceIndex;
  private boolean utf8;

  /**
   * Constructor.
   * @param utf whether to use UTF-8 for encoding strings
   */
  public BEREncoder(boolean utf8)
  {
    this(utf8, 1024);
  }

  /**
   * Constructor.
   * @param utf whether to use UTF-8 for encoding strings
   * @param initialSize the initial buffer size
   */
  public BEREncoder(boolean utf8, int initialSize)
  {
    this.utf8 = utf8;
    buffer = new byte[initialSize];
    offset = 0;
    sequenceOffset = new int[16];
    sequenceIndex = 0;
  }

  /**
   * Reset this encoder for reuse.
   */
  public void reset()
  {
    for (int i = 0; i < offset; i++)
      {
        buffer[i] = 0;
      }
    offset = 0;
    for (int i = 0; i < sequenceIndex; i++)
      {
        sequenceOffset[i] = 0;
      }
    sequenceIndex = 0;
  }

  /**
   * Returns the current size of the encoded data.
   */
  public int size()
  {
    return offset;
  }

  /**
   * Returns the encoded data.
   */
  public byte[] toByteArray()
  {
    byte[] ret = new byte[offset];
    System.arraycopy(buffer, 0, ret, 0, offset);
    return ret;
  }

  // -- 8.2 Encoding of a boolean value --

  /**
   * Appends a boolean value.
   * @param value the value
   */
  public void append(boolean value)
  {
    append(value, BERConstants.BOOLEAN);
  }
  
  /**
   * Appends a boolean value with the specified ASN.1 type code.
   * @param value the value
   * @param code the type code
   */
  public void append(boolean value, int code)
  {
    allocate(3);
    buffer[offset++] = (byte) code;
    buffer[offset++] = (byte) 1; /* length */
    buffer[offset++] = value ? (byte) 0xff :(byte) 0;
  }

  // -- 8.3 Encoding of an integer value --

  /**
   * Appends an integer value.
   * @param value the value
   */
  public void append(int value)
  {
    append(value, BERConstants.INTEGER);
  }
  
  /**
   * Appends an integer value with the specified ASN.1 type code.
   * @param value the value
   * @param code the type code
   */
  public void append(int value, int code)
  {
    final int mask = 0xff800000;
    int len = 4;
    while (((value & mask) == 0 || (value & mask) == mask) && (len > 1))
      {
        len--;
        value <<= 8;
      }
    allocate(len + 2);
    buffer[offset++] = (byte) code;
    buffer[offset++] = (byte) len;
    for (; len > 0; len--)
      {
        buffer[offset++] = (byte) ((value & 0xff000000) >> 24);
      }
  }

  // TODO -- 8.5 Encoding of a real value --
  
  // TODO -- 8.6 Encoding of a bitstring value --
  
  // -- 8.7 Encoding of an octetstring value --

  /**
   * Appends an octetstring value.
   * @param bytes the value
   */
  public void append(byte[] bytes)
    throws BERException
  {
    append(bytes, BERConstants.OCTET_STRING);
  }
  
  /**
   * Appends an octetstring value with the specified ASN.1 type code.
   * Sequences and sets can be appended by using the corresponding type
   * codes from BERConstants.
   * @param bytes the value
   * @param code the type code
   */
  public void append(byte[] bytes, int code)
    throws BERException
  {
    int len = (bytes == null) ? 0 : bytes.length;
    append(bytes, 0, len, code);
  }

  void append(byte[] bytes, int off, int len, int code)
    throws BERException
  {
    allocate(len + 5);
    buffer[offset++] = (byte) code;
    appendLength(len);
    if (len > 0)
      {
        System.arraycopy(bytes, off, buffer, offset, len);
        offset += len;
      }
  }

  /**
   * Appends a string value.
   * @param value the value
   */
  public void append(String value)
    throws BERException
  {
    append(value, BERConstants.UTF8_STRING);
  }
  
  /**
   * Appends a string value with the specified ASN.1 type code.
   * @param value the value
   * @param code the type code
   */
  public void append(String value, int code)
    throws BERException
  {
    byte[] bytes = null;
    if (value == null)
      {
        bytes = new byte[0];
      }
    else
      {
        String encoding = utf8 ? "UTF-8" : "ISO-8859-1";
        try
          {
            bytes = value.getBytes(encoding);
          }
        catch (UnsupportedEncodingException e)
          {
            throw new BERException("JVM does not support " + encoding);
          }
      }
    int len = bytes.length;
    allocate(len + 5);
    buffer[offset++] = (byte) code;
    appendLength(len);
    System.arraycopy(bytes, 0, buffer, offset, len);
    offset += len;
  }

  // -- 8.8 Encoding of a null value --

  /**
   * Appends a BER NULL value.
   */
  public void appendNull()
  {
    allocate(2);
    buffer[offset++] = BERConstants.NULL;
    buffer[offset++] = (byte) 0; /* length */
  }

  /**
   * Allocate at least len bytes.
   */
  private void allocate(int len)
  {
    if (buffer.length - offset < len)
      {
        int size = buffer.length;
        do
          {
            size *= 2;
          }
        while (size - offset < len);
        byte[] ret = new byte[size];
        System.arraycopy(buffer, 0, ret, 0, offset);
        buffer = ret;
      }
  }

  /**
   * Append the specified length for a string.
   */
  private void appendLength(int len)
    throws BERException
  {
    if (len < 0x80)
      {
        buffer[offset++] = (byte) len;
      }
    else if (len < 0x100)
      {
        buffer[offset++] = (byte) 0x81;
        buffer[offset++] = (byte) len;
      }
    else if (len < 0x10000)
      {
        buffer[offset++] = (byte) 0x82;
        buffer[offset++] = (byte)(len >> 0x08);
        buffer[offset++] = (byte)(len & 0xff);
      }
    else if (len < 0x1000000)
      {
        buffer[offset++] = (byte) 0x83;
        buffer[offset++] = (byte)(len >> 0x10);
        buffer[offset++] = (byte)(len >> 0x08);
        buffer[offset++] = (byte)(len & 0xff);
      }
    else
      {
        throw new BERException("Data too long: " + len);
      }
  }

  /**
   * Appends an RFC2254 search filter to this encoder.
   * @param filter the filter expression
   */
  public void appendFilter(String filter)
    throws BERException
  {
    if (filter == null || filter.length() == 0)
      {
        throw new BERException("Empty filter expression");
      }
    final byte[] bytes;
    String charset = utf8 ? "UTF-8" : "ISO-8859-1";
    try
      {
        bytes = filter.getBytes(charset);
      }
    catch (UnsupportedEncodingException e)
      {
        throw new BERException("JVM does not support " + charset);
      }
    appendFilter(bytes, 0);
  }

  int appendFilter(final byte[] bytes, int off)
    throws BERException
  {
    int depth = 0;
    while(off < bytes.length)
      {
        switch (bytes[off])
          {
          case 0x20: // SP
            off++;
            break; /* NOOP */
          case 0x28: //(
            depth++;
            off++;
            break;
          case 0x29: // )
            depth--;
            if (depth == 0)
              {
                return off + 1;
              }
            break;
          case 0x26: // &
            off = appendFilterList(bytes, off + 1,
                                   BERConstants.FILTER_AND);
            break;
          case 0x2c: // |
            off = appendFilterList(bytes, off + 1,
                                   BERConstants.FILTER_OR);
            break;
          case 0x21: // !
            off = appendFilterList(bytes, off + 1,
                                   BERConstants.FILTER_NOT);
            break;
          default:
            off = appendFilterItem(bytes, off);
          }
      }
    if (depth != 0)
      {
        //System.err.println("depth="+depth+", off="+off);
        throw new BERException("Unbalanced parentheses");
      }
    return off;
  }

  int appendFilterList(final byte[] bytes, int off, int code)
    throws BERException
  {
    BEREncoder sequence = new BEREncoder(utf8);
    while (off < bytes.length && bytes[off] == '(')
      {
        off = sequence.appendFilter(bytes, off);
      }
    append(sequence.toByteArray(), code);
    return off;
  }

  int appendFilterItem(final byte[] bytes, int off)
    throws BERException
  {
    int ei = indexOf(bytes,(byte) 0x3d, off); // =
    if (ei == -1)
      {
        throw new BERException("Missing '='");
      }
    int end = ei;
    int code;
    BEREncoder item = new BEREncoder(utf8);
    switch (bytes[ei - 1])
      {
      case 0x7e: // ~ approx
        code = BERConstants.FILTER_APPROX;
        end--;
        break;
      case 0x3e: // > greater
        code = BERConstants.FILTER_GREATER;
        end--;
        break;
      case 0x3c: // < less
        code = BERConstants.FILTER_LESS;
        end--;
        break;
      case 0x3a: // : ext
        code = BERConstants.FILTER_EXTENSIBLE;
        // TODO return appendFilterExtensibleMatch(bytes, off, ei);
        break;
      default: // equal/substring
        int si = indexOf(bytes,(byte) 0x2a, ei + 1); // *
        if (si == -1)
          {
            code = BERConstants.FILTER_EQUAL;
          }
        else
          {
            if (ei + 1 == bytes.length || bytes[ei + 2] == 0x29) // * present
              {
                code = BERConstants.FILTER_PRESENT;
                end--;
              }
            else
              {
                // substring
                BEREncoder substring = new BEREncoder(utf8);
                substring.append(bytes, off, end, BERConstants.OCTET_STRING);
                end = indexOf(bytes,(byte) 0x29, ei + 1); // )
                if (end == -1)
                  {
                    throw new BERException("No terminating ')'");
                  }
                BEREncoder value = new BEREncoder(utf8);
                value.append(unencode(bytes, ei + 1, end));
                substring.append(value.toByteArray(), 48);
                append(substring.toByteArray(),
                       BERConstants.FILTER_SUBSTRING);
                off = end;
                return off;
              }
          }
          
      }
    item.append(bytes, off,(end - off), BERConstants.OCTET_STRING);
    end = indexOf(bytes,(byte) 0x29, ei + 1); // )
    if (end == -1)
      {
        throw new BERException("No terminating ')'");
      }
    if (code != BERConstants.FILTER_PRESENT)
      {
        item.append(unencode(bytes, ei + 1, end));
      }
    append(item.toByteArray(), code);
    off = end;
    return off;
  }

  /**
   * Returns the index of the first matching byte in the specified
   * octet-string, starting at the given index. The filterlist terminator
   * ')' stops the search.
   */
  static int indexOf(final byte[] bytes, byte c, int off)
  {
    for (int i = off; i < bytes.length; i++)
      {
        if (bytes[i] == c)
          {
            return i;
          }
        else if (bytes[i] == 0x29) // )
          {
            return -1;
          }
      }
    return -1;
  }

  /**
   * Returns the unencoded version of the specified octet-string. The
   * filterlist terminator ')' delimits the end of the string.
   * This routine converts each character encoded as "\xx" where xx is the ASCII
   * character code to a single character.
   */
  static byte[] unencode(final byte[] bytes, int off, int end)
    throws BERException
  {
    byte[] buf = new byte[end - off];
    int pos = 0;
    int bsi = indexOf(bytes,(byte) 0x5c, off); // \
    while(bsi != -1)
      {
        if (bsi + 3 > end)
          {
            throw new BERException("Illegal filter value encoding");
          }
        int l = bsi - off;
        System.arraycopy(bytes, off, buf, pos, l);
        pos += l;
        int c = Character.digit((char) bytes[bsi + 2], 0x10);
        c += Character.digit((char) bytes[bsi + 1], 0x10) * 0x10;
        buf[pos++] = (byte) c;
        off += l + 3;
        bsi = indexOf(bytes,(byte) 0x5c, off); // \
      }
    int l = end - off;
    System.arraycopy(bytes, off, buf, pos, l);
    pos += l;
    off += l;
    if (pos != buf.length)
      {
        byte[] swap = new byte[pos];
        System.arraycopy(buf, 0, swap, 0, pos);
        buf = swap;
      }
    return buf;
  }
  
}

