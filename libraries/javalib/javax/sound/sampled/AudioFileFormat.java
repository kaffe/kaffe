/* 
 * Copyright (c) 2001 Transvirtual Technologies, Inc.  All rights reserved.
 * See the file "COPYING" for details.
 *
 * $tvt: AudioFileFormat.java,v 1.1 2001/11/19 20:28:44 samc Exp $ 
 */
package javax.sound.sampled;

/**
 * An instance of the AudioFileFormat class describes an audio file,
 * including the file type, the file's length in bytes, the length in
 * sample frames of the audio data contained in the file, and the format
 * of the audio data.
 *
 * The AudioSystem class includes methods for determining the format
 * of an audio file, obtaining an audio input stream from an audio file,
 * and writing an audio file from an audio input stream.
 */
public class AudioFileFormat
{
  int byteLength;
  int frameLength;
  Type type;
  AudioFormat format;

  static class Type 
  {
    String name;
    String extention;

    public static final Type AIFC = new Type ("AIFC", "aifc");
    public static final Type SND  = new Type ("SND" , "snd");
    public static final Type AIFF = new Type ("AIFF", "aiff");
    public static final Type AU   = new Type ("AU"  , "au");
    public static final Type WAVE = new Type ("WAVE", "wav");

    protected Type (String name, String extension)
    {
      this.name = name;
      this.extention = extention;
    }

    public String getExtension () 
    {
      return extention;
    }

    public final boolean equals (Object obj)
    {
      if (obj instanceof Type)
        {
          Type type = (Type) obj;
          return type.name.equals (name) && type.extention.equals (extention);
        }
      return false;
    }

    public final int hashCode ()
    {
      return super.hashCode ();
    }

    public final String toString ()
    {
      return "AudioFileFormat.Type: " + name + " extention: " + extention;
    }
  }

  public AudioFileFormat (Type type, 
                          AudioFormat format, 
                          int frameLength) 
  {
    this (type, format.getFrameSize () + frameLength, format, frameLength);
  }

  protected AudioFileFormat (Type type,
                             int byteLength,
                             AudioFormat format,
                             int frameLength)
  {
    this.type = type;
    this.byteLength = byteLength;
    this.format = format;
    this.frameLength = frameLength;
  }


  public Type getType ()
  {
    return type;
  }

  public int getByteLength ()
  {
    return byteLength;
  }

  public AudioFormat getFormat()
  {
    return format;
  }

  public int getFrameLength ()
  {
    return frameLength;
  }

  public String toString()
  {
    return "AudioFileFormat: " + type + ", " + frameLength  + " frames, " + byteLength + " bytes";
  }
}
