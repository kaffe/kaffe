/* 
 * Copyright (c) 2001 Transvirtual Technologies, Inc.  All rights reserved.
 * See the file "COPYING" for details.
 *
 * $tvt: Line.java,v 1.1 2001/11/20 01:09:05 samc Exp $ 
 */
package javax.sound.sampled;

/**
 * The Line interface represents a mono or multi-channel audio feed. A
 * line is an element of the digital audio "pipeline," such as a mixer,
 * an input or output port, or a data path into or out of a mixer.
 *
 * A line can have controls, such as gain, pan, and reverb. The
 * controls themselves are instances of classes that extend the base
 * Control class. The Line interface provides two accessor methods for
 * obtaining the line's controls: getControls returns the entire set,
 * and getControl returns a single control of specified type.
 */
public interface Line
{
  public static class Info
  {
    Class lineClass;

    /**
     * A Line.Info object contains information about a line. The only
     * information provided by Line.Info itself is the Java class of
     * the line. A subclass of Line.Info adds other kinds of information
     * about the line. This additional information depends on which Line
     * subinterface is implemented by the kind of line that the Line.Info
     * subclass describes.
     */
    public Info (Class lineClass)
    {
      this.lineClass = lineClass;
    }

    public Class getLineClass ()
    {
      return lineClass;
    }

    public boolean matches (Info info)
    {
      return this.lineClass == info.lineClass;
    }

    public String toString ()
    {
      return "Line.Info for class: " + lineClass.toString ();
    }
  }

  public Info getLineInfo ();

  public void open () throws LineUnavailableException;

  public void close ();

  public boolean isOpen ();

  public Control[] getControls ();

  public boolean isControlSupported (Control.Type control);

  public Control getControl (Control.Type control);

  public void addLineListener (LineListener listener);

  public void removeLineListener (LineListener listener);
}
