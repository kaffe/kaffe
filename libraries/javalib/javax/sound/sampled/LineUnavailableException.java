/* 
 * Copyright (c) 2001 Transvirtual Technologies, Inc.  All rights reserved.
 * See the file "COPYING" for details.
 *
 * $tvt: LineUnavailableException.java,v 1.1 2001/11/20 01:09:05 samc Exp $ 
 */
package javax.sound.sampled;

/**
 * A LineUnavailableException is an exception indicating that a line
 * cannot be opened because it is unavailable. This situation arises most
 * commonly when a requested line is already in use by another application.
 */
public class LineUnavailableException
  extends Exception
{
  public LineUnavailableException (String message)
  {
    super (message);
  }

  public LineUnavailableException ()
  {
    super ();
  }
}
