/* 
 * Copyright (c) 2001 Transvirtual Technologies, Inc.  All rights reserved.
 * See the file "COPYING" for details.
 *
 * $tvt: AudioFileFormat.java,v 1.1 2001/11/19 20:28:44 samc Exp $ 
 */
package javax.sound.sampled;


/**
 * The Clip interface represents a special kind of data line whose
 * audio data can be loaded prior to playback, instead of being streamed
 * in real time.
 */
public interface Clip
  extends DataLine
{
  public static final int LOOP_CONTINUOUSLY = -1;

  public void open (AudioFormat format, 
                    byte[] data,
                    int offset,
                    int bufferSize) throws LineUnavailableException;

  public int getFrameLength ();

  public long getMicrosecondLength ();

  public void setFramePosition (int frames);

  public void setMicrosecondPosition (long microseconds);

  public void setLoopPoints(int start, int end);

  public void loop (int count);
}
