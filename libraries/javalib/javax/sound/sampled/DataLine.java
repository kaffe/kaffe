/* 
 * Copyright (c) 2001 Transvirtual Technologies, Inc.  All rights reserved.
 * See the file "COPYING" for details.
 *
 * $tvt: DataLine.java,v 1.1 2001/11/20 01:09:05 samc Exp $ 
 */
package javax.sound.sampled;


/**
 * DataLine adds media-related functionality to its superinterface,
 * Line. This functionality includes transport-control methods that start,
 * stop, drain, and flush the audio data that passes through the line. A
 * data line can also report the current position, volume, and audio format
 * of the media. Data lines are used for output of audio by means of the
 * subinterfaces SourceDataLine or Clip, which allow an application program
 * to write data. Similarly, audio input is handled by the subinterface
 * TargetDataLine, which allows data to be read.
 */
public interface DataLine
  extends Line
{
  /**
   * Besides the class information inherited from its superclass,
   * DataLine.Info provides additional information specific to data
   * lines. 
   * This information includes:
   * - the audio formats supported by the data line
   * - the minimum and maximum sizes of its internal buffer
   */
  public static class Info
    extends Line.Info
  {
    AudioFormat[] formats;
    int minBufferSize = 0;
    int maxBufferSize = 16 * 1024;

    public Info (Class lineClass, AudioFormat[] formats, int minBufferSize, int maxBufferSize)
    {
      super (lineClass);
      this.formats = formats;
      this.minBufferSize = minBufferSize;
      this.maxBufferSize = maxBufferSize;
    }

    public Info (Class lineClass, AudioFormat format, int bufferSize)
    {
      super (lineClass);
      this.formats = new AudioFormat[] { format };
      this.maxBufferSize = bufferSize;
    }

    public Info (Class lineClass, AudioFormat format)
    {
      super (lineClass);
      this.formats = new AudioFormat[] { format };
    }

    public AudioFormat[] getFormats ()
    {
      return formats;
    }

    public boolean isFormatSupported (AudioFormat format)
    {
      for (int i = 0; i < formats.length; i++)
        {
          if (formats[i].matches (format))
            {
              return true;
            }
        }
      return false;
    }

    public int getMinBufferSize ()
    {
      return minBufferSize;
    }

    public int getMaxBufferSize ()
    {
      return maxBufferSize;
    }

    public boolean matches (Info info)
    {
      if (!super.matches (info) 
          || minBufferSize < info.minBufferSize
          || maxBufferSize > info.maxBufferSize)
        {
          return false;
        }
      for (int i = 0; i < formats.length; i++)
        {
          if (info.isFormatSupported (formats[i]) == false)
            {
              return false;
            }
        }
      return true;
    }

    public String toString()
    {
      return "[" + minBufferSize + "-" + maxBufferSize + "]";
    }
  }

  public void drain ();

  public void flush ();

  public void start ();

  public void stop ();

  public boolean isRunning ();

  public boolean isActive ();

  public AudioFormat getFormat ();

  public int getBufferSize ();

  public int available ();

  public int getFramePosition ();

  public long getMicrosecondPosition ();

  public float getLevel ();
}
