/* 
 * Copyright (c) 2001 Transvirtual Technologies, Inc.  All rights reserved.
 * See the file "COPYING" for details.
 *
 * $tvt: AudioFileFormat.java,v 1.1 2001/11/19 20:28:44 samc Exp $ 
 */
package javax.sound.sampled;

/**
 * Ports are simple lines for input or output of audio to or from audio
 * devices. Common examples of ports that act as source lines (mixer inputs)
 * include the microphone, line input, and CD-ROM drive. Ports that act
 * as target lines (mixer outputs) include the speaker, headphone, and
 * line output. You can access port using a Port.Info  object.
 */
public interface Port
{
  /**
   * The Port.Info class extends Line.Info with additional information
   * specific to ports, including the port's name and whether it is
   * a source or a target for its mixer. By definition, a port acts as
   * either a source or a target to its mixer, but not both. (Audio input
   * ports are sources; audio output ports are targets.)
   */
  public static class Info
    extends Line.Info
  {
    String name;
    boolean isSource;

    public static final Info MICROPHONE   = new Info (Port.class, "microphone"  , true);
    public static final Info LINE_IN      = new Info (Port.class, "line in"     , true);
    public static final Info COMPACT_DISC = new Info (Port.class, "compact disc", true);
    public static final Info SPEAKER      = new Info (Port.class, "speaker"     , false);
    public static final Info HEADPHONE    = new Info (Port.class, "headphone"   , false);
    public static final Info LINE_OUT     = new Info (Port.class, "line out"    , false);

    public Info (Class lineClass, String name, boolean isSource)
    {
      super (lineClass);
      this.isSource = isSource;
      this.name = name;
    }

    public String getName ()
    {
      return name;
    }

    public boolean isSource ()
    {
      return isSource ();
    }

    public boolean matches (Line.Info info)
    {
      return super.matches (info);
    }

    public final boolean equals (Object obj)
    {
      return super.equals (obj);
    }

    public final int hashCode ()
    {
      return super.hashCode ();
    }

    public final String toString ()
    {
      return super.toString ();
    }
  }
}
