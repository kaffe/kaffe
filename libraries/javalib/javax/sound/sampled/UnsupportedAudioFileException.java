/* 
 * Copyright (c) 2001 Transvirtual Technologies, Inc.  All rights reserved.
 * See the file "COPYING" for details.
 *
 * $tvt: UnsupportedAudioFileException.java,v 1.1 2001/11/19 20:28:44 samc Exp $ 
 */
package javax.sound.sampled;

/**
 * An UnsupportedAudioFileException is an exception indicating that
 * an operation failed because a file did not contain valid data of a
 * recognized file type and format.
 */
public class UnsupportedAudioFileException 
  extends Exception
{
  public UnsupportedAudioFileException (String message)
  {
    super (message);
  }

  public UnsupportedAudioFileException ()
  {
    super ();
  }
}
