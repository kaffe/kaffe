/* 
 * Copyright (c) 2001 Transvirtual Technologies, Inc.  All rights reserved.
 * See the file "COPYING" for details.
 *
 * $tvt: AudioFormat.java,v 1.2 2001/11/20 01:09:05 samc Exp $ 
 */
package javax.sound.sampled;

/**
 * AudioFormat is the class that specifies a particular arrangement of 
 * data in a sound stream. 
 * By examing the information stored in the audio format, you can discover 
 * how to interpret the bits in the binary sound data.
 */
public class AudioFormat
{
  protected boolean bigEndian;
  protected int channels;
  protected Encoding encoding;
  protected float frameRate;
  protected int frameSize;
  protected float sampleRate;
  protected int sampleSizeInBits;

  public static class Encoding
  {
    String name;

    public static final Encoding ALAW         = new Encoding ("ALAW");
    public static final Encoding PCM_SIGNED   = new Encoding ("PCM_SIGNED");
    public static final Encoding PCM_UNSIGNED = new Encoding ("PCM_UNSIGNED");
    public static final Encoding ULAW         = new Encoding ("ULAW");

    protected Encoding (String name) 
    {
      this.name = name;
    }

    public boolean equals (Object obj) 
    {
      return obj == this;
    }

    public final int hashCode ()
    {
      return super.hashCode ();
    }

    public final String toString ()
    {
      return name;
    }
  }

  public AudioFormat (Encoding encoding, float sampleRate, int sampleSizeInBits, 
                      int channels, int frameSize, float frameRate, boolean bigEndian)
  {
    this.encoding = encoding;
    this.sampleRate = sampleRate;
    this.sampleSizeInBits = sampleSizeInBits;
    this.channels = channels;
    this.frameSize = frameSize;
    this.frameRate = frameRate;
    this.bigEndian = bigEndian;
  }

  public AudioFormat(float sampleRate, int sampleSizeInBits, int channels, 
                     boolean signed, boolean bigEndian)
  {
    this.sampleRate = sampleRate;
    this.sampleSizeInBits = sampleSizeInBits;
    this.channels = channels;
    this.bigEndian = bigEndian;
    if (signed) 
      {
        encoding = Encoding.PCM_SIGNED;
      }
    else
      {
        encoding = Encoding.PCM_UNSIGNED;
      }
  }

  public int getChannels ()
  {
    return channels;
  }

  public Encoding getEncoding ()
  {
    return encoding;
  }

  public int getFrameSize ()
  {
    return frameSize;
  }

  public float getFrameRate ()
  {
    return frameRate;
  }

  public float getSampleRate () 
  {
    return sampleRate;
  }

  public int getSampleSizeInBits () 
  {
    return sampleSizeInBits;
  }

  public boolean isBigEndian ()
  { 
    return bigEndian;
  }

  public boolean matches (AudioFormat format)
  {
    return (bigEndian == format.bigEndian) && (frameRate == format.frameRate)
           && (sampleRate == format.sampleRate) && (sampleSizeInBits == format.sampleSizeInBits);
  }

  public String toString ()
  {
    return sampleRate + "Hz " + sampleSizeInBits + "bit " + encoding.toString () + " audio data";
  }
}
