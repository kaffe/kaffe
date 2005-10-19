/*
 * SaslCramMD5.java
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

package gnu.inet.util;

import java.io.UnsupportedEncodingException;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;
import javax.security.sasl.SaslClient;
import javax.security.sasl.SaslException;

/**
 * SASL mechanism for CRAM-MD5.
 *
 * @author <a href='mailto:dog@gnu.org'>Chris Burdess</a>
 */
public class SaslCramMD5
  implements SaslClient
{

  private String username;
  private String password;
  private boolean complete;

  public SaslCramMD5(String username, String password)
  {
    this.username = username;
    this.password = password;
  }

  public String getMechanismName()
  {
    return "CRAM-MD5";
  }

  public boolean hasInitialResponse()
  {
    return false;
  }

  public byte[] evaluateChallenge(byte[] challenge)
    throws SaslException
  {
    try
      {
        byte[] s = password.getBytes("US-ASCII");
        byte[] digest = hmac_md5(s, challenge);
        byte[] r0 = username.getBytes("US-ASCII");
        byte[] r1 = new byte[r0.length + digest.length + 1];
        System.arraycopy(r0, 0, r1, 0, r0.length); // add username
        r1[r0.length] = 0x20; // SPACE
        System.arraycopy(digest, 0, r1, r0.length+1, digest.length);
        complete = true;
        return r1;
      }
    catch (UnsupportedEncodingException e)
      {
        String msg = "Username or password contains non-ASCII characters";
        throw new SaslException(msg, e);
      }
    catch (NoSuchAlgorithmException e)
      {
        String msg = "MD5 algorithm not available";
        throw new SaslException(msg, e);
      }
  }

  public boolean isComplete()
  {
    return complete;
  }

  public byte[] unwrap(byte[] incoming, int off, int len)
    throws SaslException
  {
    byte[] ret = new byte[len - off];
    System.arraycopy(incoming, off, ret, 0, len);
    return ret;
  }

  public byte[] wrap(byte[] outgoing, int off, int len)
    throws SaslException
  {
    byte[] ret = new byte[len - off];
    System.arraycopy(outgoing, off, ret, 0, len);
    return ret;
  }

  public Object getNegotiatedProperty(String name)
  {
    return null;
  }

  public void dispose()
  {
  }

  /**
   * Computes a CRAM digest using the HMAC algorithm:
   * <pre>
   * MD5(key XOR opad, MD5(key XOR ipad, text))
   * </pre>.
   * <code>secret</code> is null-padded to a length of 64 bytes.
   * If the shared secret is longer than 64 bytes, the MD5 digest of the
   * shared secret is used as a 16 byte input to the keyed MD5 calculation.
   * See RFC 2104 for details.
   */
  private static byte[] hmac_md5(byte[] key, byte[] text)
    throws NoSuchAlgorithmException
  {
    byte[] k_ipad = new byte[64];
    byte[] k_opad = new byte[64];
    byte[] digest;
    MessageDigest md5 = MessageDigest.getInstance("MD5");
    // if key is longer than 64 bytes reset it to key=MD5(key)
    if (key.length>64)
      {
        md5.update(key);
        key = md5.digest();
      }
    // start out by storing key in pads
    System.arraycopy(key, 0, k_ipad, 0, key.length);
    System.arraycopy(key, 0, k_opad, 0, key.length);
    // XOR key with ipad and opad values
    for (int i=0; i<64; i++)
      {
        k_ipad[i] ^= 0x36;
        k_opad[i] ^= 0x5c;
      }
    // perform inner MD5
    md5.reset();
    md5.update(k_ipad);
    md5.update(text);
    digest = md5.digest();
    // perform outer MD5
    md5.reset();
    md5.update(k_opad);
    md5.update(digest);
    digest = md5.digest();
    return digest;
  }
  
}

