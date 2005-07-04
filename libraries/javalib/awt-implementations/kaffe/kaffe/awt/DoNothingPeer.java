/*DoNothingPeer - PseudoPeer used by peerless AWTs

Copyright (c) 2004
Kaffe.org contributors, see ChangeLog for details.  All rights reserved.

This file is part of Kaffe.

Kaffe is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2, or (at your option)
any later version.

Kaffe is distributed in the hope that it will be useful, but
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

package kaffe.awt;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.*;
import java.awt.peer.*;

/**
 * PseudoPeer used by peerless AWTs
 *
 * @author Dalibor Topic (robilad@kaffe.org)
 */

public class DoNothingPeer implements LightweightPeer, WindowPeer {

  public int checkImage(Image img, 
			int width,
			int height, 
			ImageObserver ob) {
    return 0;
  }

  public Image createImage(ImageProducer prod) {
    return null;
  }

  public Image createImage(int width, int height) {
    return null;
  }

  public void disable() {}
  public void dispose() {}
  public void enable() {}

  public ColorModel getColorModel() {
    return null;
  }

  public FontMetrics getFontMetrics(Font f) {
    return null;
  }

  public Graphics getGraphics() {
    return null;
  }

  public Point getLocationOnScreen() {
    return null;
  }

  public Dimension getMinimumSize() {
    return null;
  }

  public Dimension getPreferredSize() {
    return null;
  }

  public Toolkit getToolkit(){
    return null;
  }

  public void handleEvent(AWTEvent e) {}
  public void hide() {}

  public boolean isFocusTraversable() {
    return false;
  }

  public boolean isFocusable() {
    return false;
  }

  public Dimension minimumSize() {
    return null;
  }

  public Dimension preferredSize() {
    return null;
  }

  public void paint(Graphics graphics) {
  }

  public boolean prepareImage(Image img,
			      int width,
			      int height,
			      ImageObserver ob) {
    return false;
  }

  public void print(Graphics graphics) {}
  public void repaint(long tm, int x, int y, int width, int height) {}
  public void requestFocus() {}

  public boolean requestFocus (Component source, boolean bool1, boolean bool2, long x){
    return false;
  }

  public void reshape(int x, int y, int width, int height) {}
  public void setBackground(Color color){}
  public void setBounds(int x, int y, int width, int height){}
  public void setCursor(Cursor cursor){}
  public void setEnabled(boolean enabled){}
  public void setFont(Font font){}
  public void setForeground(Color color){}
  public void setVisible(boolean visible){}
  public void show(){}

  public GraphicsConfiguration getGraphicsConfiguration(){
    return null;
  }

  public void setEventMask (long mask){}

  public boolean isObscured() {
    return false;
  }

  public boolean canDetermineObscurity(){
    return false;
  }

  public void coalescePaintEvent(PaintEvent e) {}
  public void updateCursorImmediately(){}
  public VolatileImage createVolatileImage(int width, int height){
    return null;
  }

  public boolean handlesWheelScrolling() {
    return false;
  }

  public void createBuffers(int x, BufferCapabilities capabilities) throws AWTException {}

  public Image getBackBuffer(){
    return null;
  }

  public void flip(BufferCapabilities.FlipContents contents){}
  public void destroyBuffers(){}

  public Insets insets() {
    return null;
  }

  public Insets getInsets() {
    return null;
  }

  public void beginValidate() {}

  public void endValidate() {}

  public void beginLayout() {}

  public void endLayout() {}

  public boolean isPaintPending() {
    return false;
  }

  public void toBack() {}
  public void toFront() {}
}
