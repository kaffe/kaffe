/*
 * SaslOutputStream.java
 * Copyright (C) 2002 The Free Software Foundation
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

import java.io.FilterOutputStream;
import java.io.IOException;
import java.io.OutputStream;

import javax.security.sasl.SaslClient;

/**
 * A filter output stream that encodes data written to it using a SASL
 * client.
 *
 * @author <a href="mailto:dog@gnu.org">Chris Burdess</a>
 */
public class SaslOutputStream
  extends FilterOutputStream
{

  /*
   * The SASL client used for encoding data.
   */
  private final SaslClient sasl;

  /**
   * Constructor.
   * @param sasl the SASL client
   * @param out the target output stream
   */
  public SaslOutputStream(SaslClient sasl, OutputStream out)
  {
    super(out);
    this.sasl = sasl;
  }

  /**
   * Character write.
   */
  public void write(int c)
    throws IOException
  {
    byte[] bytes = new byte[1];
    bytes[0] = (byte) c;
    write(bytes, 0, 1);
  }

  public void write(byte[] bytes)
    throws IOException
  {
    write(bytes, 0, bytes.length);
  }
  
  /**
   * Block write.
   */
  public void write(byte[] bytes, int off, int len)
    throws IOException
  {
    byte[] wrapped = sasl.wrap(bytes, off, len);
    super.write(wrapped, 0, wrapped.length);
  }
  
}

