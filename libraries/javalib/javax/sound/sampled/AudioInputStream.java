/* 
 * Copyright (c) 2001 Transvirtual Technologies, Inc.  All rights reserved.
 * See the file "COPYING" for details.
 *
 * $tvt: Line.java,v 1.1 2001/11/20 01:09:05 samc Exp $ 
 */
package javax.sound.sampled;

import java.io.InputStream;
import java.io.IOException;

/**
 * An audio input stream is an input stream with a specified audio format
 * and length. The length is expressed in sample frames, not bytes. Several
 * methods are provided for reading a certain number of bytes from the
 * stream, or an unspecified number of bytes. The audio input stream keeps
 * track of the last byte that was read. You can skip over an arbitrary
 * number of bytes to get to a later position for reading. An audio input
 * stream may support marks. When you set a mark, the current position is
 * remembered so that you can return to it later.
 */
public class AudioInputStream
  extends InputStream 
{
  protected AudioFormat format;
  protected long frameLength;
  protected int frameSize;
  protected long framePos;
  InputStream in;

  public AudioInputStream (InputStream stream,
                           AudioFormat format,
                           long length)
  {
    in = stream;
    this.format = format;
    this.frameLength = length;
  }

  public AudioFormat getFormat ()
  {
    return format;
  }

  public long getFrameLength ()
  {
    return frameLength;
  }

  public int read ()
    throws IOException
  {
    return in.read ();
  }

  public int read (byte[] b)
    throws IOException
  {
    return in.read (b);
  }

  public int read (byte[] b,
                   int off,
                   int len)
    throws IOException
  {
    return in.read (b, off, len);
  }

  public long skip (long n)
    throws IOException
  {
    return in.skip (n);
  }

  public int available ()
    throws IOException
  {
    return in.available ();
  }

  public void close ()
    throws IOException
  {
    in.close ();
  }

  public void mark (int readlimit)
  {
    in.mark (readlimit);
  }
 
  public void reset()
    throws IOException
  {
    in.reset ();
  }

  public boolean markSupported ()
  {
    return in.markSupported ();
  }













}
