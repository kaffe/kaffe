/* 
 * Copyright (c) 2001 Transvirtual Technologies, Inc.  All rights reserved.
 * See the file "COPYING" for details.
 *
 * $tvt: DataLine.java,v 1.1 2001/11/20 01:09:05 samc Exp $ 
 */
package javax.sound.sampled;


/**
 * A target data line is a type of DataLine from which audio data can be
 * read. The most common example is a data line that gets its data from
 * an audio capture device. (The device is implemented as a mixer that
 * writes to the target data line.)
 *
 * Note that the naming convention for this interface reflects the
 * relationship between the line and its mixer. From the perspective of
 * an application, a target data line may act as a source for audio data.
 */
public interface TargetDataLine
  extends DataLine
{
  public void open (AudioFormat format) throws LineUnavailableException;
  public void open (AudioFormat format, int bufferSize) throws LineUnavailableException;
  public int read (byte[] b, int off, int len);
}
