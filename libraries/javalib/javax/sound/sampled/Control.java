/* 
 * Copyright (c) 2001 Transvirtual Technologies, Inc.  All rights reserved.
 * See the file "COPYING" for details.
 *
 * $tvt: AudioFileFormat.java,v 1.1 2001/11/19 20:28:44 samc Exp $ 
 */
package javax.sound.sampled;

/**
 * Lines often have a set of controls, such as gain and pan, that affect
 * the audio signal passing through the line. Java Sound's Line objects
 * let you obtain a particular control object by passing its class as the
 * argument to a getControl method.
 *
 * Because the various types of controls have different purposes and
 * features, all of their functionality is accessed from the subclasses
 * that define each kind of control.
 */
public abstract class Control
{
  /**
   * An instance of the Type class represents the type of the
   * control. Static instances are provided for the common types.
   */
  public static class Type
  {
    String name;

    protected Type (String name)
    {
      this.name = name;
    }

    public final boolean equals (Object obj)
    {
      return (obj instanceof Type) && name.equals (((Type) obj).name);
    }

    public final int hashCode ()
    {
      return super.hashCode ();
    }

    public final String toString()
    {
      return "Control.Type: " + name;
    }
  }

  Type type;

  protected Control (Type type)
  {
    this.type = type;
  }

  public String toString ()
  {
    return "Control (" + type.toString () + ")";
  }
}
