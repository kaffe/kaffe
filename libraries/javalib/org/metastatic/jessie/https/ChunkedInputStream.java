/* ChunkedInputStream.java -- input stream for Transfer-encoding: chunked
   Copyright (C) 2004  Casey Marshall <csm@gnu.org>

This file is a part of Jessie.

Jessie is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2 of the License, or (at your
option) any later version.

Jessie is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License along
with Jessie; if not, write to the

   Free Software Foundation, Inc.,
   51 Franklin Street, Fifth Floor,
   Boston, MA  02110-1301
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


package org.metastatic.jessie.https;

import java.io.EOFException;
import java.io.FilterInputStream;
import java.io.IOException;
import java.io.InputStream;

public class ChunkedInputStream extends FilterInputStream
{

  // Fields.
  // -------------------------------------------------------------------------

  private long chunkLen;
  private long count;

  // Constructors.
  // -------------------------------------------------------------------------

  public ChunkedInputStream (final InputStream in)
  {
    super (in);
    chunkLen = -1;
    count = -1;
  }

  // Instance methods.
  // -------------------------------------------------------------------------

  public synchronized int available() throws IOException
  {
    if (chunkLen == -1)
      return 0;
    if (chunkLen == 0)
      return -1;
    return (int) Math.min (chunkLen - count, super.available());
  }

  public boolean markSupported()
  {
    return false;
  }

  public void mark (int readLimit)
  {
  }

  public void reset()
  {
  }

  public synchronized int read() throws IOException
  {
    if (count == chunkLen)
      readChunk();
    if (chunkLen == 0)
      return -1;
    int i = super.read();
    count++;
    return i;
  }

  public synchronized int read (byte[] buf, int off, int len) throws IOException
  {
    if (count == chunkLen)
      readChunk();
    if (chunkLen == 0)
      return -1;
    len = (int) Math.min ((long) len, chunkLen - count);
    len = super.read (buf, off, len);
    if (len == -1)
      throw new EOFException ("unexpected end-of-file while reading chunked data");
    count += len;
    return len;
  }

  public synchronized long skip (long bytes) throws IOException
  {
    if (count == chunkLen)
      readChunk();
    if (chunkLen == 0)
      return -1;
    bytes = Math.min (bytes, chunkLen - count);
    bytes = super.skip (bytes);
    count += bytes;
    return bytes;
  }

  // Own methods.
  // -------------------------------------------------------------------------

  boolean eof()
  {
    return chunkLen == 0;
  }

  private void readChunk() throws IOException
  {
    if (chunkLen != -1)
      {
        if (super.read() != '\r')
          throw new IOException ("malformed chunked input");
        if (super.read() != '\n')
          throw new IOException ("malformed chunked input");
      }
    StringBuffer buf = new StringBuffer();
    int i;
    while ((i = super.read()) != -1)
      {
        if ((i >= '0' && i <= '9') || (i >= 'a' && i <= 'f') ||
            (i >= 'A' && i <= 'F'))
          buf.append ((char) i);
        else
          break;
      }
    try
      {
        chunkLen = Long.parseLong (buf.toString(), 16);
      }
    catch (NumberFormatException nfe)
      {
        IOException ioe = new IOException ("malformed chunk-size");
        ioe.initCause (nfe);
        throw ioe;
      }
    count = 0L;
    boolean sawCr = false;
    do
      {
        if (i == '\r')
          sawCr = true;
        if (i == '\n')
          {
            if (!sawCr)
              throw new IOException ("malformed chunked input");
            break;
          }
        // Ignore anything else.
      }
    while ((i = super.read()) != -1);
  }
}
