/* 
 * Copyright (c) 2001 Transvirtual Technologies, Inc.  All rights reserved.
 * See the file "COPYING" for details.
 *
 * $tvt: LineEvent.java,v 1.1 2001/11/20 01:09:05 samc Exp $ 
 */
package javax.sound.sampled;

import java.io.File;
import java.io.InputStream;
import java.io.IOException;
import java.io.OutputStream;
import java.net.URL;

/**
 * The AudioSystem class acts as the entry point to the sampled-audio
 * system resources. This class lets you query and access the mixers
 * that are installed on the system. AudioSystem includes a number of
 * methods for converting audio data between different formats, and for
 * translating between audio files and streams. It also provides a method
 * for obtaining a Line directly from the AudioSystem without dealing
 * explicitly with mixers.
 */
public class AudioSystem
{
  public static final int NOT_SPECIFIED = -1;

  public static Mixer.Info[] getMixerInfo ()
  {
    return new Mixer.Info[0];
  }

  public static Mixer getMixer (Mixer.Info info)
  {
    return null;
  }

  public static Line.Info[] getSourceLineInfo (Line.Info info)
  {
    return new Line.Info[0];
  }

  public static Line.Info[] getTargetLineInfo (Line.Info info)
  {
    return new Line.Info[0];
  }

  public static boolean isLineSupported (Line.Info info)
  {
    return false;
  }

  public static Line getLine (Line.Info info) 
    throws LineUnavailableException
  {
    throw new LineUnavailableException ("AudioSystem not implemented");
  }

  public static AudioFormat.Encoding[] getTargetEncodings (AudioFormat.Encoding sourceEncoding)
  {
    return new AudioFormat.Encoding[] { sourceEncoding };
  }

  public static AudioFormat.Encoding[] getTargetEncodings (AudioFormat sourceFormat)

  {
    return new AudioFormat.Encoding[] { sourceFormat.getEncoding () };
  }

  public static boolean isConversionSupported (AudioFormat.Encoding targetEncoding,
                                               AudioFormat sourceFormat)
  {
    return false;
  }

  public static AudioInputStream getAudioInputStream(AudioFormat.Encoding targetEncoding,
                                                     AudioInputStream sourceStream)

  {
    throw new IllegalArgumentException ("AudioSystem not implemented");
  }


  public static AudioFormat[] getTargetFormats (AudioFormat.Encoding targetEncoding,
                                                AudioFormat sourceFormat)
  {
    return new AudioFormat[0];
  }

  public static boolean isConversionSupported (AudioFormat targetFormat,
                                               AudioFormat sourceFormat)
  {
    return false;
  }

  public static AudioInputStream getAudioInputStream (AudioFormat targetFormat,
                                                      AudioInputStream sourceStream)

  {
    throw new IllegalArgumentException ("AudioSystem not implemented");
  }

  public static AudioFileFormat getAudioFileFormat (InputStream stream)
    throws UnsupportedAudioFileException, IOException
  {
    throw new UnsupportedAudioFileException ("AudioSystem not implemented");
  }

  public static AudioFileFormat getAudioFileFormat (URL url)
    throws UnsupportedAudioFileException, IOException
  {
    throw new UnsupportedAudioFileException ("AudioSystem not implemented");
  }

  public static AudioFileFormat getAudioFileFormat (File file)
    throws UnsupportedAudioFileException, IOException
  {
    throw new UnsupportedAudioFileException ("AudioSystem not implemented");
  }

  public static AudioInputStream getAudioInputStream (InputStream stream)
    throws UnsupportedAudioFileException, IOException
  {
    throw new UnsupportedAudioFileException ("AudioSystem not implemented");
  }

  public static AudioInputStream getAudioInputStream (URL url)
    throws UnsupportedAudioFileException, IOException
  {
    throw new UnsupportedAudioFileException ("AudioSystem not implemented");
  }

  public static AudioInputStream getAudioInputStream (File file)
    throws UnsupportedAudioFileException, IOException
  {
    throw new UnsupportedAudioFileException ("AudioSystem not implemented");
  }

  public static AudioFileFormat.Type[] getAudioFileTypes ()
  {
    return new AudioFileFormat.Type[0];
  }

  public static boolean isFileTypeSupported (AudioFileFormat.Type fileType)
  {
    return false;
  }

  public static AudioFileFormat.Type[] getAudioFileTypes(AudioInputStream stream)
  {
    return new AudioFileFormat.Type[0];
  }

  public static boolean isFileTypeSupported(AudioFileFormat.Type fileType,
                                            AudioInputStream stream)
  {
    return false;
  }

  public static int write(AudioInputStream stream,
                          AudioFileFormat.Type fileType,
                          OutputStream out)
    throws IOException
  {
    throw new IOException ("AudioSystem not implemented");
  }

  public static int write(AudioInputStream stream,
                          AudioFileFormat.Type fileType,
                          File out)
    throws IOException
  {
    throw new IOException ("AudioSystem not implemented");
  }
}
