/* 
 * Copyright (c) 2001 Transvirtual Technologies, Inc.  All rights reserved.
 * See the file "COPYING" for details.
 *
 * $tvt: LineEvent.java,v 1.1 2001/11/20 01:09:05 samc Exp $ 
 */
package javax.sound.sampled;

import java.util.EventObject;

/**
 * The LineEvent class encapsulates information that a line sends its
 * listeners whenever the line opens, closes, starts, or stops. Each of
 * these four state changes is represented by a corresponding type of
 * event. A listener receives the event as a parameter to its its update
 * method. By querying the event, the listener can learn the type of event,
 * the line responsible for the event, and how much data the line had
 * processed when the event occurred.
 */
public class LineEvent
  extends EventObject
{
  Type type;
  long position;

  /**
   * The LineEvent.Type inner class identifies what kind of event occurred
   * on a line. Static instances are provided for the common types (OPEN,
   * CLOSE, START, and STOP).
   */
  public static class Type
  {
    String name;
    public static final Type OPEN  = new Type ("open" );
    public static final Type CLOSE = new Type ("close");
    public static final Type START = new Type ("start");
    public static final Type STOP  = new Type ("stop" );

    protected Type (String name)
    {
      this.name = name;
    }

    public final boolean equals (Object obj)
    {
      return obj == this;
    }

    public final int hashCode ()
    {
      return super.hashCode ();
    }

    public String toString ()
    {
      return name;
    }
  }

  public LineEvent (Line line, Type type, long position)
  {
    super (line);
    this.type = type;
    this.position = position;
  }

  public final Line getLine ()
  {
    return (Line) source;
  }

  public final Type getType ()
  {
    return type;
  }

  public final long getFramePosition ()
  {
    return position;
  }

  public String toString ()
  {
    return "LineEvent at frame " + position + ": " + type.toString ();
  }
}
