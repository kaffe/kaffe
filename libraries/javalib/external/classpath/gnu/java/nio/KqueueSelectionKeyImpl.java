/* KqueueSelectionKeyImpl.java -- selection key for kqueue/kevent.
   Copyright (C) 2006 Free Software Foundation, Inc.

This file is part of GNU Classpath.

GNU Classpath is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

GNU Classpath is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU Classpath; see the file COPYING.  If not, write to the
Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
02110-1301 USA.

Linking this library statically or dynamically with other modules is
making a combined work based on this library.  Thus, the terms and
conditions of the GNU General Public License cover the whole
combination.

As a special exception, the copyright holders of this library give you
permission to link this library with independent modules to produce an
executable, regardless of the license terms of these independent
modules, and to copy and distribute the resulting executable under
terms of your choice, provided that you also meet, for each linked
independent module, the terms and conditions of the license of that
module.  An independent module is a module which is not derived from
or based on this library.  If you modify this library, you may extend
this exception to your version of the library, but you are not
obligated to do so.  If you do not wish to do so, delete this
exception statement from your version. */


package gnu.java.nio;


import java.io.IOException;
import java.nio.ByteBuffer;
import java.nio.channels.SelectableChannel;
import java.nio.channels.SelectionKey;
import java.nio.channels.Selector;

/**
 * @author Casey Marshall (csm@gnu.org)
 */
public class KqueueSelectionKeyImpl extends SelectionKey
{
  int interestOps;
  int readyOps;
  ByteBuffer nstate;
  boolean valid;
  int key;
  boolean readEverEnabled = false;
  boolean writeEverEnabled = false;

  /** The selector we were created for. */
  private final KqueueSelectorImpl selector;
  
  /** The channel we are attached to. */
  private final SelectableChannel channel;
  
  private final VMChannelOwner natChannel;
  
  public KqueueSelectionKeyImpl(KqueueSelectorImpl selector,
                                SelectableChannel channel)
  {
    this.selector = selector;
    this.channel = channel;
    natChannel = (VMChannelOwner) channel;
    interestOps = 0;
    readyOps = 0;
    valid = true;
  }

  /* (non-Javadoc)
   * @see java.nio.channels.SelectionKey#cancel()
   */
  //@Override
  public void cancel()
  {
    selector.doCancel(this);
    valid = false;
  }

  /* (non-Javadoc)
   * @see java.nio.channels.SelectionKey#channel()
   */
  //@Override
  public SelectableChannel channel()
  {
    return channel;
  }

  /* (non-Javadoc)
   * @see java.nio.channels.SelectionKey#interestOps()
   */
  //@Override
  public int interestOps()
  {
    return interestOps;
  }

  /* (non-Javadoc)
   * @see java.nio.channels.SelectionKey#interestOps(int)
   */
  //@Override
  public SelectionKey interestOps(int ops)
  {
    if (!isValid())
      throw new IllegalStateException();
    if ((ops & ~channel.validOps()) != 0)
      throw new IllegalArgumentException();
    this.interestOps = ops;
    try
      {
        selector.updateOps(this,
                           natChannel.getVMChannel().getState().getNativeFD(),
                           false);
      }
    catch (IOException ioe)
      {
        throw new IllegalStateException("channel is invalid");
      }
    return this;
  }

  /* (non-Javadoc)
   * @see java.nio.channels.SelectionKey#isValid()
   */
  //@Override
  public boolean isValid()
  {
    return valid && selector.isOpen();
  }

  /* (non-Javadoc)
   * @see java.nio.channels.SelectionKey#readyOps()
   */
  //@Override
  public int readyOps()
  {
    return readyOps;
  }

  /* (non-Javadoc)
   * @see java.nio.channels.SelectionKey#selector()
   */
  //@Override
  public Selector selector()
  {
    return selector;
  }
  
  public String toString()
  {
    if (!isValid())
      return super.toString() + " [ <<invalid>> ]";
    return super.toString() + " [ interest ops: {"
      + ((interestOps & OP_ACCEPT) != 0 ? " OP_ACCEPT" : "")
      + ((interestOps & OP_CONNECT) != 0 ? " OP_CONNECT" : "")
      + ((interestOps & OP_READ) != 0 ? " OP_READ" : "")
      + ((interestOps & OP_WRITE) != 0 ? " OP_WRITE" : "")
      + " }; ready ops: {"
      + ((readyOps & OP_ACCEPT) != 0 ? " OP_ACCEPT" : "")
      + ((readyOps & OP_CONNECT) != 0 ? " OP_CONNECT" : "")
      + ((readyOps & OP_READ) != 0 ? " OP_READ" : "")
      + ((readyOps & OP_WRITE) != 0 ? " OP_WRITE" : "")
      + " } ]";
  }
}
