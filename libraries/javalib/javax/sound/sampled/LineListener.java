/* 
 * Copyright (c) 2001 Transvirtual Technologies, Inc.  All rights reserved.
 * See the file "COPYING" for details.
 *
 * $tvt: AudioFileFormat.java,v 1.1 2001/11/19 20:28:44 samc Exp $ 
 */
package javax.sound.sampled;

import java.util.EventListener;

/**
 * Instances of classes that implement the LineListener interface can
 * register to receive events when a line's status changes.
 */
public interface LineListener
  extends EventListener
{
  public void update (LineEvent event);
}
