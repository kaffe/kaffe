/* 
 * Copyright (c) 2001 Transvirtual Technologies, Inc.  All rights reserved.
 * See the file "COPYING" for details.
 *
 * $tvt: DataLine.java,v 1.1 2001/11/20 01:09:05 samc Exp $ 
 */
package javax.sound.sampled;


/**
 * A source data line is a data line to which data may be written. It
 * acts as a source to its mixer. An application writes audio bytes to a
 * source data line, which handles the buffering of the bytes and delivers
 * them to the mixer. The mixer may mix the samples with those from other
 * sources and then deliver the mix to a target such as an output port
 * (which may represent an audio output device on a sound card).
 */
public interface SourceDataLine
  extends DataLine
{
  public void open (AudioFormat format) throws LineUnavailableException;
  public void open (AudioFormat format, int bufferSize) throws LineUnavailableException;
  public int write (byte[] b, int off, int len);
}
