/* SSLHMac.java -- SSLv3's MAC algorithm.
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

import java.util.Map;

import gnu.crypto.hash.IMessageDigest;
import gnu.crypto.mac.IMac;

public class SSLHMac implements IMac, Cloneable
{

  // Fields.
  // -------------------------------------------------------------------------

  public static final byte PAD1 = 0x36;
  public static final byte PAD2 = 0x5c;

  protected IMessageDigest md;

  protected byte[] key;

  // Constructors.
  // -------------------------------------------------------------------------

  public SSLHMac(IMessageDigest md)
  {
    super();
    this.md = md;
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  public Object clone()
  {
    try
      {
        return super.clone();
      }
    catch (CloneNotSupportedException cnse)
      {
        throw new Error();
      }
  }

  public String name()
  {
    return "SSLHMac-" + md.name();
  }

  public int macSize()
  {
    return md.hashSize();
  }

  public void init(Map attributes)
  {
    key = (byte[]) attributes.get(MAC_KEY_MATERIAL);
    if (key == null)
      throw new NullPointerException();
    reset();
  }

  public void reset()
  {
    md.reset();
    md.update(key, 0, key.length);
    for (int i = 0; i < 80 - md.hashSize(); i++)
      md.update(PAD1);
  }

  public byte[] digest()
  {
    byte[] h1 = md.digest();
    md.reset();
    md.update(key, 0, key.length);
    for (int i = 0; i < 80 - md.hashSize(); i++)
      md.update(PAD2);
    md.update(h1, 0, h1.length);
    byte[] result = md.digest();
    reset();
    return result;
  }

  public void update(byte b)
  {
    md.update(b);
  }

  public void update(byte[] buf, int off, int len)
  {
    md.update(buf, off, len);
  }

  public boolean selfTest()
  {
    return true; // XXX
  }
}
