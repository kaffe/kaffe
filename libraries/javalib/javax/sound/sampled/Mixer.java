/* 
 * Copyright (c) 2001 Transvirtual Technologies, Inc.  All rights reserved.
 * See the file "COPYING" for details.
 *
 * $tvt: AudioFileFormat.java,v 1.1 2001/11/19 20:28:44 samc Exp $ 
 */
package javax.sound.sampled;

/**
 * A mixer is an audio device with one or more lines. It need not be
 * designed for mixing audio signals. A mixer that actually mixes audio has
 * multiple input (source) lines and at least one output (target) line. The
 * former are often instances of classes that implement SourceDataLine,
 * and the latter, TargetDataLine. Port  objects, too, are either source
 * lines or target lines. A mixer can accept prerecorded, loopable sound
 * as input, by having some of its source lines be instances of objects
 * that implement the Clip interface.
 */
public interface Mixer
  extends Line
{
  /**
   * The Mixer.Info class represents information about an audio mixer,
   * including the product's name, version, and vendor, along with a
   * textual description. This information may be retrieved through the
   * getMixerInfo method of the Mixer interface.
   */ 
  public static class Info
  {
    String name;
    String vendor;
    String description;
    String version;

    protected Info (String name, String vendor, String description, String version)
    {
      this.name = name;
      this.vendor = vendor;
      this.description = description;
      this.version = version;
    }

    public final boolean equals (Object obj)
    {
      if (!(obj instanceof Info)) 
        {
          return false;
        }
      Info info = (Info) obj;
      return name.equals (info.name) 
          && vendor.equals (info.vendor) 
          && version.equals (info.vendor);

    }

    public final int hashCode ()
    {
      return super.hashCode ();
    }

    public final String getName ()
    {
      return name;
    }

    public final String getVendor ()
    {
      return vendor;
    }

    public final String getDescription ()
    {
      return description;
    }

    public final String getVersion ()
    {
      return version;
    }

    public final String toString ()
    {
      return "Mixer.Info: " + vendor + " " + name + " " + version;
    }
  }

  public Info getMixerInfo ();

  public Line.Info[] getSourceLineInfo ();

  public Line.Info[] getTargetLineInfo ();

  public Line.Info[] getSourceLineInfo (Line.Info info);

  public Line.Info[] getTargetLineInfo (Line.Info info);

  public boolean isLineSupported (Line.Info info);

  public Line getLine (Line.Info info) throws LineUnavailableException;

  public int getMaxLines (Line.Info info);

  public Line[] getSourceLines ();

  public Line[] getTargetLines ();

  public void synchronize (Line[] lines, boolean maintainSync);

  public void unsynchronize (Line[] lines);

  public boolean isSynchronizationSupported (Line[] lines, boolean maintainSync);
}
