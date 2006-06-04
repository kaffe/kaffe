/* VolatileImageGraphics.java
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


package gnu.java.awt.peer.gtk;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GraphicsConfiguration;
import java.awt.Image;
import java.awt.Point;
import java.awt.Rectangle;
import java.awt.Shape;
import java.awt.font.GlyphVector;
import java.awt.geom.AffineTransform;
import java.awt.geom.Rectangle2D;
import java.awt.image.BufferedImage;
import java.awt.image.DataBuffer;
import java.awt.image.DataBufferInt;
import java.awt.image.ColorModel;
import java.awt.image.DirectColorModel;
import java.awt.image.RenderedImage;
import java.awt.image.ImageObserver;
import java.util.WeakHashMap;

public class VolatileImageGraphics extends CairoSurfaceGraphics
{
  private GtkVolatileImage owner;

  public VolatileImageGraphics(GtkVolatileImage owner)
  {
    super( owner.offScreen );
    this.owner = owner;
  }
  
  VolatileImageGraphics(VolatileImageGraphics copyFrom)
  {
    super( copyFrom.owner.offScreen );
    owner = copyFrom.owner;
  }

  /**
   * Abstract methods.
   */  
  public Graphics create()
  {
    return new VolatileImageGraphics( this );
  }
  
  public void copyAreaImpl(int x, int y, int width, int height, int dx, int dy)
  {
    surface.copyAreaNative(x, y, width, height, dx, dy, surface.width);
    owner.invalidate();
  }

  /**
   * Overloaded methods that do actual drawing need to enter the gdk threads 
   * and also do certain things before and after.
   */
  public void draw(Shape s)
  {
    super.draw(s);
    Rectangle r = s.getBounds();
    owner.invalidate();
  }

  public void fill(Shape s)
  {
    super.fill(s);
    Rectangle r = s.getBounds();
    owner.invalidate();
  }

  public void drawRenderedImage(RenderedImage image, AffineTransform xform)
  {
    super.drawRenderedImage(image, xform);
    owner.invalidate();
  }

  protected boolean drawImage(Image img, AffineTransform xform,
			      Color bgcolor, ImageObserver obs)
  {
    boolean rv = super.drawImage(img, xform, bgcolor, obs);
    owner.invalidate();
    return rv;
  }

  public void drawGlyphVector(GlyphVector gv, float x, float y)
  {
    super.drawGlyphVector(gv, x, y);
    owner.invalidate();
  }
}

