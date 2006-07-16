/* XGraphics.java -- The Graphics implementation for X
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


package gnu.java.awt.peer.x;

import gnu.x11.Drawable;
import gnu.x11.GC;
import gnu.x11.Pixmap;
import gnu.x11.Point;

import java.awt.AWTError;
import java.awt.Color;
import java.awt.Composite;
import java.awt.Font;
import java.awt.FontMetrics;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.GraphicsConfiguration;
import java.awt.Image;
import java.awt.Paint;
import java.awt.Rectangle;
import java.awt.RenderingHints;
import java.awt.Shape;
import java.awt.Stroke;
import java.awt.RenderingHints.Key;
import java.awt.font.FontRenderContext;
import java.awt.font.GlyphVector;
import java.awt.geom.AffineTransform;
import java.awt.image.BufferedImage;
import java.awt.image.BufferedImageOp;
import java.awt.image.ImageObserver;
import java.awt.image.ImageProducer;
import java.awt.image.RenderedImage;
import java.awt.image.renderable.RenderableImage;
import java.text.AttributedCharacterIterator;
import java.util.Map;

public class XGraphics
  extends Graphics2D
  implements Cloneable
{

  /**
   * The X Drawable to draw on.
   */
  private Drawable xdrawable;

  /**
   * The X graphics context (GC).
   */
  private GC xgc;

  /**
   * The current translation.
   */
  private int translateX;
  private int translateY;

  /**
   * The current clip. Possibly null.
   */
  private Rectangle clip;

  /**
   * The current font, possibly null.
   */
  private Font font;

  /**
   * The current foreground color, possibly null.
   */
  private Color foreground;

  /**
   * Indicates if this object has been disposed.
   */
  private boolean disposed = false;

  /**
   * Creates a new XGraphics on the specified X Drawable.
   *
   * @param d the X Drawable for which we create the Graphics
   */
  XGraphics(Drawable d)
  {
    xdrawable = d;
    xgc = new GC(d);
    translateX = 0;
    translateY = 0;
    clip = new Rectangle(0, 0, d.width, d.height);
  }

  /**
   * Creates an exact copy of this graphics context.
   *
   * @return an exact copy of this graphics context
   */
  public Graphics create()
  {
    XGraphics copy = (XGraphics) clone();
    return copy;
  }

  /**
   * Translates the origin by (x, y).
   */
  public void translate(int x, int y)
  {
    translateX += x;
    translateY += y;
    if (clip != null)
      {
        clip.x -= x;
        clip.y -= y;
        setClip(clip);
      }
  }

  /**
   * Returns the current foreground color, possibly <code>null</code>.
   *
   * @return the current foreground color, possibly <code>null</code>
   */
  public Color getColor()
  {
    return foreground;
  }

  /**
   * Sets the current foreground color. A <code>null</code> value doesn't
   * change the current setting.
   *
   * @param c the foreground color to set
   */
  public void setColor(Color c)
  {
    if (c != null)
      {
        xgc.set_foreground(c.getRGB());
        foreground = c;
      }
  }

  public void setPaintMode()
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void setXORMode(Color color)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  /**
   * Returns the current font, possibly <code>null</code>.
   *
   * @return the current font, possibly <code>null</code>
   */
  public Font getFont()
  {
    return font;
  }

  /**
   * Sets the font on the graphics context. A <code>null</code> value doesn't
   * change the current setting.
   *
   * @param f the font to set
   */
  public void setFont(Font f)
  {
    if (f != null)
      {
        XFontPeer xFontPeer = (XFontPeer) f.getPeer();
        xgc.set_font(xFontPeer.getXFont());
        font = f;
      }
  }

  /**
   * Returns the font metrics for the specified font.
   *
   * @param font the font for which we want the font metrics
   *
   * @return the font metrics for the specified font
   */
  public FontMetrics getFontMetrics(Font font)
  {
    if (font == null)
      {
        if (this.font == null)
          setFont(new Font("Dialog", Font.PLAIN, 12));
        font = this.font;
      }
    XFontPeer xFontPeer = (XFontPeer) font.getPeer();
    return xFontPeer.getFontMetrics(font);
  }

  /**
   * Returns the bounds of the current clip.
   *
   * @return the bounds of the current clip
   */
  public Rectangle getClipBounds()
  {
    return clip != null ? clip.getBounds() : null;
  }

  /**
   * Clips the current clip with the specified clip.
   */
  public void clipRect(int x, int y, int width, int height)
  {
    if (clip == null)
      {
        clip = new Rectangle(x, y, width, height);
      }
    else
      {
        computeIntersection(x, y, width, height, clip);
      }
    // Update the X clip setting.
    setClip(clip.x, clip.y, clip.width, clip.height);
  }

  /**
   * Returns <code>true</code> when the specified rectangle intersects with
   * the current clip, <code>false</code> otherwise. This is overridden to
   * avoid unnecessary creation of Rectangles via getBounds().
   *
   * @param x the x coordinate of the rectangle
   * @param y the y coordinate of the rectangle
   * @param w the width of the rectangle
   * @param h the height of the rectangle
   *
   * @return <code>true</code> when the specified rectangle intersects with
   *         the current clip, <code>false</code> otherwise
   */
  public boolean hitClip(int x, int y, int w, int h)
  {
    boolean hit;
    if (clip == null)
      {
        hit = true;
      }
    else
      {
        hit = clip.intersects(x, y, w, h);
      }
    //System.err.println("hitClip:  " + hit);
    return hit;
  }

  public void setClip(int x, int y, int width, int height)
  {
    if (clip != null)
      clip.setBounds(x, y, width, height);
    else
      clip = new Rectangle(x, y, width, height);

    gnu.x11.Rectangle[] clipRects = new gnu.x11.Rectangle[] {
                                  new gnu.x11.Rectangle(x, y, width, height) };
    xgc.set_clip_rectangles(translateX, translateY, clipRects, GC.UN_SORTED);
  }

  public Shape getClip()
  {
    // Return a copy here, so nobody can trash our clip.
    return clip == null ? null : clip.getBounds();
  }

  /**
   * Sets the current clip.
   *
   * @param clip the clip to set
   */
  public void setClip(Shape clip)
  {
    if (clip != null)
      {
        Rectangle b;
        if (clip instanceof Rectangle)
          {
            b = (Rectangle) clip;
          }
        else
          {
            b = clip.getBounds();
          }
        setClip(b.x, b.y, b.width, b.height);
      }
    else
      {
        setClip(0, 0, xdrawable.width, xdrawable.height);
      }
  }

  public void copyArea(int x, int y, int width, int height, int dx, int dy)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  /**
   * Draws a line from point (x1, y1) to point (x2, y2).
   */
  public void drawLine(int x1, int y1, int x2, int y2)
  {
    //System.err.println("drawLine: " + (x1 + translateX) + ", " + ( y1 + translateY) + ", " + (x2 + translateX) + ", " + (y2 + translateY) + " on: " + xdrawable);
    xdrawable.line(xgc, x1 + translateX, y1 + translateY,
                   x2 + translateX, y2 + translateY);
  }

  /**
   * Fills the specified rectangle.
   */
  public void fillRect(int x, int y, int width, int height)
  {
    xdrawable.rectangle(xgc, x + translateX, y + translateY,
                        width, height, true);
  }

  public void clearRect(int x, int y, int width, int height)
  {
    xgc.set_foreground(Color.WHITE.getRGB());
    xdrawable.rectangle(xgc, x, y, width, height, true);
    if (foreground != null)
      xgc.set_foreground(foreground.getRGB());
  }

  public void drawRoundRect(int x, int y, int width, int height, int arcWidth,
                            int arcHeight)
  {
    // Draw 4 lines.
    int arcRadiusX = arcWidth / 2;
    int arcRadiusY = arcHeight / 2;
    drawLine(x + arcRadiusX, y, x + width - arcRadiusX, y);
    drawLine(x, y + arcRadiusY, x, y + height - arcRadiusY);
    drawLine(x + arcRadiusX, y + height, x + width - arcRadiusX, y + height);
    drawLine(x + width, y + arcRadiusY, x + width, y + height - arcRadiusY);

    // Draw the 4 arcs at the corners.
    // Upper left.
    drawArc(x, y, arcWidth, arcHeight, 90, 90);
    // Lower left.
    drawArc(x, y + height - arcHeight, arcWidth, arcHeight, 180, 90);
    // Upper right.
    drawArc(x + width - arcWidth, y, arcWidth, arcHeight, 0, 90);
    // Lower right.
    drawArc(x + width - arcWidth, y + height - arcHeight, arcWidth, arcHeight,
            270, 90);
  }

  public void fillRoundRect(int x, int y, int width, int height, int arcWidth,
                            int arcHeight)
  {
    // Fill the 3 rectangles that make up the inner area.
    int arcRadiusX = arcWidth / 2;
    int arcRadiusY = arcHeight / 2;
    // Left.
    fillRect(x, y + arcRadiusY, arcRadiusX, height - arcHeight);
    // Middle.
    fillRect(x + arcRadiusX, y, width - arcWidth, height);
    // Right.
    fillRect(x + width - arcRadiusX, y + arcRadiusY, arcRadiusX,
             height - arcHeight);

    // Fill the 4 arcs in the corners.
    // Upper left.
    fillArc(x, y, arcWidth, arcHeight, 90, 90);
    // Lower left.
    fillArc(x, y + height - arcHeight, arcWidth, arcHeight, 180, 90);
    // Upper right.
    fillArc(x + width - arcWidth, y, arcWidth, arcHeight, 0, 90);
    // Lower right.
    fillArc(x + width - arcWidth, y + height - arcHeight, arcWidth, arcHeight,
            270, 90);
  }

  public void drawOval(int x, int y, int width, int height)
  {
    xdrawable.arc(xgc, x, y, width, height, 0, 360 * 64, false);
  }

  public void fillOval(int x, int y, int width, int height)
  {
    xdrawable.arc(xgc, x, y, width, height, 0, 360 * 64, true);
  }

  public void drawArc(int x, int y, int width, int height, int arcStart,
                      int arcAngle)
  {
    xdrawable.arc(xgc, x, y, width, height, arcStart * 64, arcAngle * 64, false);
  }

  public void fillArc(int x, int y, int width, int height, int arcStart,
                      int arcAngle)
  {
    xdrawable.arc(xgc, x, y, width, height, arcStart * 64, arcAngle * 64, true);
  }

  public void drawPolyline(int[] xPoints, int[] yPoints, int npoints)
  {
    int numPoints = Math.min(xPoints.length, yPoints.length);
    Point[] points = new Point[numPoints];
    // FIXME: Improve Escher API to accept arrays to avoid creation
    // of many Point objects.
    for (int i = 0; i < numPoints; i++)
      points[i] = new Point(xPoints[i], yPoints[i]);
    xdrawable.poly_line(xgc, points, Drawable.ORIGIN);
  }

  public void drawPolygon(int[] xPoints, int[] yPoints, int npoints)
  {
    int numPoints = Math.min(xPoints.length, yPoints.length);
    Point[] points = new Point[numPoints];
    // FIXME: Improve Escher API to accept arrays to avoid creation
    // of many Point objects.
    for (int i = 0; i < numPoints; i++)
      points[i] = new Point(xPoints[i], yPoints[i]);
    xdrawable.poly_line(xgc, points, Drawable.ORIGIN);
  }

  public void fillPolygon(int[] xPoints, int[] yPoints, int npoints)
  {
    int numPoints = Math.min(xPoints.length, yPoints.length);
    Point[] points = new Point[numPoints];
    // FIXME: Improve Escher API to accept arrays to avoid creation
    // of many Point objects.
    for (int i = 0; i < numPoints; i++)
      points[i] = new Point(xPoints[i], yPoints[i]);
    xdrawable.fill_poly(xgc, points, Drawable.COMPLEX, Drawable.ORIGIN);
  }

  /**
   * Draws the specified string at (x, y).
   */
  public void drawString(String string, int x, int y)
  {
    if (disposed)
      throw new AWTError("XGraphics already disposed");

    xdrawable.text(xgc, x + translateX, y + translateY, string);
  }

  public void drawString(AttributedCharacterIterator ci, int x, int y)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  /**
   * Draws the specified image on the drawable at position (x,y).
   */
  public boolean drawImage(Image image, int x, int y, ImageObserver observer)
  {
    if (image instanceof XImage)
      {
        XImage xim = (XImage) image;
        Pixmap pm = xim.pixmap;
        xdrawable.copy_area(pm, xgc, 0, 0, pm.width, pm.height,
                            x + translateX, y + translateY);
      }
//    else if (image instanceof BufferedImage)
//      {
//        BufferedImage bufferedImage = (BufferedImage) image;
//        Raster raster = bufferedImage.getData();
//        int w = bufferedImage.getWidth();
//        int h = bufferedImage.getHeight();
//        // Push data to X server.
//        ZPixmap zPixmap = new ZPixmap(xdrawable.display, w, h,
//                                      xdrawable.display.default_pixmap_format);
//        System.err.println("data buffer length: " + zPixmap.data.length);
//        int[] pixel = new int[4];
//        for (int tx = 0; tx < w; tx++)
//          {
//            for (int ty = 0; ty < h; ty++)
//              {
//                pixel = raster.getPixel(tx, ty, pixel);
////                System.err.print("r: " + pixel[0]);
////                System.err.print(", g: " + pixel[1]);
////                System.err.println(", b: " + pixel[2]);
//                zPixmap.set_red(tx, ty, pixel[0]);
//                zPixmap.set_green(tx, ty, pixel[1]);
//                zPixmap.set_blue(tx, ty, pixel[2]);
//              }
//          }
//        xdrawable.put_image(xgc, zPixmap, x, y);
//      }
    else
      {
        // Pre-render the image into an XImage.
        ImageProducer source = image.getSource();
        ImageConverter conv = new ImageConverter();
        source.startProduction(conv);
        XImage xim = conv.getXImage();
        Pixmap pm = xim.pixmap;
        xdrawable.copy_area(pm, xgc, 0, 0, pm.width, pm.height,
                            x + translateX, y + translateY);
      }
    return true;
  }

  public boolean drawImage(Image image, int x, int y, int width, int height,
                           ImageObserver observer)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public boolean drawImage(Image image, int x, int y, Color bgcolor,
                           ImageObserver observer)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public boolean drawImage(Image image, int x, int y, int width, int height,
                           Color bgcolor, ImageObserver observer)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public boolean drawImage(Image image, int dx1, int dy1, int dx2, int dy2,
                           int sx1, int sy1, int sx2, int sy2,
                           ImageObserver observer)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public boolean drawImage(Image image, int dx1, int dy1, int dx2, int dy2,
                           int sx1, int sy1, int sx2, int sy2, Color bgcolor,
                           ImageObserver observer)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  /**
   * Frees any resources associated with this object.
   */
  public void dispose()
  {
    xdrawable.display.flush();
    if (! disposed)
      {
        xgc.free();
        disposed = true;
      }
  }

  // Additional Graphics2D methods.
  
  public void draw(Shape shape)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public boolean drawImage(Image image, AffineTransform xform, ImageObserver obs)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void drawImage(BufferedImage image, BufferedImageOp op, int x, int y)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void drawRenderedImage(RenderedImage image, AffineTransform xform)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void drawRenderableImage(RenderableImage image, AffineTransform xform)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void drawString(String text, float x, float y)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void drawString(AttributedCharacterIterator iterator, float x, float y)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void fill(Shape shape)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public boolean hit(Rectangle rect, Shape text, boolean onStroke)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public GraphicsConfiguration getDeviceConfiguration()
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void setComposite(Composite comp)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void setPaint(Paint paint)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void setStroke(Stroke stroke)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void setRenderingHint(Key hintKey, Object hintValue)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public Object getRenderingHint(Key hintKey)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void setRenderingHints(Map hints)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void addRenderingHints(Map hints)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public RenderingHints getRenderingHints()
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void translate(double tx, double ty)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void rotate(double theta)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void rotate(double theta, double x, double y)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void scale(double scaleX, double scaleY)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void shear(double shearX, double shearY)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void transform(AffineTransform Tx)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void setTransform(AffineTransform Tx)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public AffineTransform getTransform()
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public Paint getPaint()
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public Composite getComposite()
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void setBackground(Color color)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public Color getBackground()
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public Stroke getStroke()
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void clip(Shape s)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public FontRenderContext getFontRenderContext()
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }

  public void drawGlyphVector(GlyphVector g, float x, float y)
  {
    // FIXME: Implement this.
    throw new UnsupportedOperationException("Not yet implemented");
  }


  // Additional helper methods.

  /**
   * Creates and returns an exact copy of this XGraphics.
   */
  protected Object clone()
  {
    try
      {
        XGraphics copy = (XGraphics) super.clone();
        copy.xgc = xgc.copy();

        // Save the original clip.
        if (clip != null)
          copy.clip = new Rectangle(clip);
        return copy;
      }
    catch (CloneNotSupportedException ex)
      {
        AWTError err = new AWTError("Error while cloning XGraphics");
        err.initCause(ex);
        throw err;
      }
  }
  
  /**
   * Computes the intersection between two rectangles and stores the result
   * int the second rectangle.
   *
   * This method has been copied from {@link javax.swing.SwingUtilities}.
   *
   * @param x the x coordinate of the rectangle #1
   * @param y the y coordinate of the rectangle #1
   * @param w the width of the rectangle #1
   * @param h the height of the rectangle #1
   * @param rect the rectangle #2 and output rectangle
   */
  private static void computeIntersection(int x, int y, int w, int h,
                                          Rectangle rect)
  {
    int x2 = (int) rect.x;
    int y2 = (int) rect.y;
    int w2 = (int) rect.width;
    int h2 = (int) rect.height;

    int dx = (x > x2) ? x : x2;
    int dy = (y > y2) ? y : y2;
    int dw = (x + w < x2 + w2) ? (x + w - dx) : (x2 + w2 - dx);
    int dh = (y + h < y2 + h2) ? (y + h - dy) : (y2 + h2 - dy);

    if (dw >= 0 && dh >= 0)
      rect.setBounds(dx, dy, dw, dh);
    else
      rect.setBounds(0, 0, 0, 0);
  }


}
