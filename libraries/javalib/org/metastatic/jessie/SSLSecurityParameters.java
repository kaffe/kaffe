/* SSLSecurityParameters.java -- SSL/TLSv1 security parameters.
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

import java.util.Arrays;

import javax.net.ssl.SSLException;
import javax.net.ssl.SSLProtocolException;

import gnu.crypto.prng.LimitReachedException;

public class SSLSecurityParameters extends SecurityParameters
{

  // Constructors.
  // -------------------------------------------------------------------------

  public SSLSecurityParameters(boolean amServer)
  {
    super(amServer);
  }

  // Methods.
  // -------------------------------------------------------------------------

  public Text decryptFragment(Text ct) throws SSLException
  {
    int macLen = (mac != null) ? mac.macSize() : 0;
    byte[] ctFrag = (byte[]) ct.getFragment().clone();
    byte[] ptFrag = null;
    byte[] ptMac  = null;
    if (rc4Random == null && blockCipher == null)
      {
        ptFrag = new byte[ctFrag.length - macLen];
        ptMac = new byte[macLen];
        System.arraycopy(ctFrag, 0, ptFrag, 0, ptFrag.length);
        if (macLen > 0)
          System.arraycopy(ctFrag, ptFrag.length, ptMac, 0, macLen);
      }
    else if (streamCipher)
      {
        ptFrag = new byte[ctFrag.length - macLen];
        ptMac = new byte[macLen];
        try
          {
            transformRC4(ctFrag, 0, ctFrag.length, ctFrag, 0);
          }
        catch (LimitReachedException lre)
          {
            throw new SSLProtocolException("RC4 overflow");
          }
        System.arraycopy(ctFrag, 0, ptFrag, 0, ptFrag.length);
        System.arraycopy(ctFrag, ptFrag.length, ptMac, 0, macLen);
      }
    else
      {
        for (int i = 0; i < ctFrag.length; i += blockCipher.currentBlockSize())
          {
            blockCipher.update(ctFrag, i, ctFrag, i);
          }
        int padLen = ctFrag[ctFrag.length - 1] & 0xFF;
        byte[] padding = new byte[padLen];
        System.arraycopy(ctFrag, ctFrag.length - padLen - 1, padding, 0, padLen);
        ptFrag = new byte[ctFrag.length - (padLen + macLen + 1)];
        ptMac = new byte[macLen];
        System.arraycopy(ctFrag, 0, ptFrag, 0, ptFrag.length);
        System.arraycopy(ctFrag, ptFrag.length, ptMac, 0, macLen);
      }

    Text pt = new Text(ct.getType(), ct.getVersion(), ptFrag);

    if (mac != null)
      {
        for (int i = 56; i >= 0; i -= 8)
          mac.update((byte) (sequence >>> i));
        byte[] ptEnc = pt.getEncoded();
        mac.update(ptEnc, 0, ptEnc.length);
        byte[] ptMac2 = mac.digest();
        if (!Arrays.equals(ptMac, ptMac2))
          throw new SSLException("MAC verification failed");
      }

    sequence++;
    return pt;
  }

  public Text encryptFragment(Text pt) throws SSLException
  {
    int macLen = (mac != null) ? mac.macSize() : 0;
    byte[] ptFrag = pt.getFragment();
    byte[] ctFrag = null;
    byte[] ptMac = null;
    if (mac != null)
      {
        for (int i = 56; i >= 0; i -= 8)
          mac.update((byte) (sequence >>> i));
        byte[] ptEnc = pt.getEncoded();
        mac.update(ptEnc, 0, ptEnc.length);
        ptMac = mac.digest();
      }
    sequence++;

    if (rc4Random == null && blockCipher == null)
      {
        ctFrag = new byte[ptFrag.length + macLen];
        System.arraycopy(ptFrag, 0, ctFrag, 0, ptFrag.length);
        if (macLen > 0)
          System.arraycopy(ptMac, 0, ctFrag, ptFrag.length, macLen);
      }
    else if (streamCipher)
      {
        ctFrag = new byte[ptFrag.length + macLen];
        System.arraycopy(ptFrag, 0, ctFrag, 0, ptFrag.length);
        System.arraycopy(ptMac,  0, ctFrag, ptFrag.length, macLen);
        try
          {
            transformRC4(ctFrag, 0, ctFrag.length, ctFrag, 0);
          }
        catch (LimitReachedException lre)
          {
            throw new SSLProtocolException("RC4 overflow");
          }
      }
    else
      {
        int padLen = (ptFrag.length + macLen + 1)
          % blockCipher.currentBlockSize() +
          (random.nextInt(4) * blockCipher.currentBlockSize());
        while (padLen > 255)
          padLen -= blockCipher.currentBlockSize();
        ctFrag = new byte[ptFrag.length + macLen + padLen + 1];
        System.arraycopy(ptFrag, 0, ctFrag, 0, ptFrag.length);
        System.arraycopy(ptMac,  0, ctFrag, ptFrag.length, macLen);
        for (int i = ptFrag.length + macLen; i < ctFrag.length; i++)
          ctFrag[i] = (byte) padLen;
        for (int i = 0; i < ctFrag.length; i += blockCipher.currentBlockSize())
          blockCipher.update(ctFrag, i, ctFrag, i);
      }

    return new Text(pt.getType(), pt.getVersion(), ctFrag);
  }
}
