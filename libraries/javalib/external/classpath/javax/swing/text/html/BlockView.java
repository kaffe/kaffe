/* BlockView.java -- 
   Copyright (C) 2005 Free Software Foundation, Inc.

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


package javax.swing.text.html;

import gnu.javax.swing.text.html.css.Length;

import java.awt.Graphics;
import java.awt.Rectangle;
import java.awt.Shape;

import javax.swing.SizeRequirements;
import javax.swing.event.DocumentEvent;
import javax.swing.text.AttributeSet;
import javax.swing.text.BoxView;
import javax.swing.text.Element;
import javax.swing.text.View;
import javax.swing.text.ViewFactory;

/**
 * @author Lillian Angel <langel@redhat.com>
 */
public class BlockView extends BoxView
{

  /**
   * The attributes for this view.
   */
  private AttributeSet attributes;

  /**
   * The box painter for this view.
   */
  private StyleSheet.BoxPainter painter;

  /**
   * The width and height as specified in the stylesheet, null if not
   * specified. The first value is the X_AXIS, the second the Y_AXIS. You
   * can index this directly by the X_AXIS and Y_AXIS constants.
   */
  private Length[] cssSpans;

  /**
   * Creates a new view that represents an html box. 
   * This can be used for a number of elements.
   * 
   * @param elem - the element to create a view for
   * @param axis - either View.X_AXIS or View.Y_AXIS
   */
  public BlockView(Element elem, int axis)
  {
    super(elem, axis);
    cssSpans = new Length[2];
  }

  /**
   * Creates the parent view for this. It is called before
   * any other methods, if the parent view is working properly.
   * Implemented to forward to the superclass and call
   * setPropertiesFromAttributes to set the paragraph 
   * properties.
   * 
   * @param parent - the new parent, or null if the view
   * is being removed from a parent it was added to. 
   */
  public void setParent(View parent)
  {
    super.setParent(parent);
    
    if (parent != null)
      setPropertiesFromAttributes();
  }
  
  /**
   * Calculates the requirements along the major axis.
   * This is implemented to call the superclass and then
   * adjust it if the CSS width or height attribute is specified
   * and applicable.
   * 
   * @param axis - the axis to check the requirements for.
   * @param r - the SizeRequirements. If null, one is created.
   * @return the new SizeRequirements object.
   */
  protected SizeRequirements calculateMajorAxisRequirements(int axis,
                                                            SizeRequirements r)
  {
    if (r == null)
      r = new SizeRequirements();
    
    if (setCSSSpan(r, axis))
      {
        // If we have set the span from CSS, then we need to adjust
        // the margins.
        SizeRequirements parent = super.calculateMajorAxisRequirements(axis,
                                                                       null);
        int margin = axis == X_AXIS ? getLeftInset() + getRightInset()
                                    : getTopInset() + getBottomInset();
        r.minimum -= margin;
        r.preferred -= margin;
        r.maximum -= margin;
        constrainSize(axis, r, parent);
      }
    else
      r = super.calculateMajorAxisRequirements(axis, r);
    return r;
  }

  /**
   * Calculates the requirements along the minor axis.
   * This is implemented to call the superclass and then
   * adjust it if the CSS width or height attribute is specified
   * and applicable.
   * 
   * @param axis - the axis to check the requirements for.
   * @param r - the SizeRequirements. If null, one is created.
   * @return the new SizeRequirements object.
   */
  protected SizeRequirements calculateMinorAxisRequirements(int axis,
                                                            SizeRequirements r)
  {
    if (r == null)
      r = new SizeRequirements();
    
    if (setCSSSpan(r, axis))
      {
        // If we have set the span from CSS, then we need to adjust
        // the margins.
        SizeRequirements parent = super.calculateMinorAxisRequirements(axis,
                                                                       null);
        int margin = axis == X_AXIS ? getLeftInset() + getRightInset()
                                    : getTopInset() + getBottomInset();
        r.minimum -= margin;
        r.preferred -= margin;
        r.maximum -= margin;
        constrainSize(axis, r, parent);
      }
    else
      r = super.calculateMinorAxisRequirements(axis, r);

    // Apply text alignment if appropriate.
    if (axis == X_AXIS)
      {
        Object o = getAttributes().getAttribute(CSS.Attribute.TEXT_ALIGN);
        if (o != null)
          {
            String al = o.toString().trim();
            if (al.equals("center"))
              r.alignment = 0.5f;
            else if (al.equals("right"))
              r.alignment = 1.0f;
            else
              r.alignment = 0.0f;
          }
      }
    return r;
  }

  /**
   * Sets the span on the SizeRequirements object according to the
   * according CSS span value, when it is set.
   * 
   * @param r the size requirements
   * @param axis the axis
   *
   * @return <code>true</code> when the CSS span has been set,
   *         <code>false</code> otherwise
   */
  private boolean setCSSSpan(SizeRequirements r, int axis)
  {
    boolean ret = false;
    Length span = cssSpans[axis];
    // We can't set relative CSS spans here because we don't know
    // yet about the allocated span. Instead we use the view's
    // normal requirements.
    if (span != null && ! span.isPercentage())
      {
        r.minimum = (int) span.getValue();
        r.preferred = (int) span.getValue();
        r.maximum = (int) span.getValue();
        ret = true;
      }
    return ret;
  }

  /**
   * Constrains the <code>r</code> requirements according to
   * <code>min</code>.
   *
   * @param axis the axis
   * @param r the requirements to constrain
   * @param min the constraining requirements
   */
  private void constrainSize(int axis, SizeRequirements r,
                             SizeRequirements min)
  {
    if (min.minimum > r.minimum)
      {
        r.minimum = min.minimum;
        r.preferred = min.minimum;
        r.maximum = Math.max(r.maximum, min.maximum);
      }
  }

  /**
   * Lays out the box along the minor axis (the axis that is
   * perpendicular to the axis that it represents). The results
   * of the layout are placed in the given arrays which are
   * the allocations to the children along the minor axis.
   * 
   * @param targetSpan - the total span given to the view, also 
   * used to layout the children.
   * @param axis - the minor axis
   * @param offsets - the offsets from the origin of the view for
   * all the child views. This is a return value and is filled in by this
   * function.
   * @param spans - the span of each child view. This is a return value and is 
   * filled in by this function.
   */
  protected void layoutMinorAxis(int targetSpan, int axis,
                                 int[] offsets, int[] spans)
  {
    int viewCount = getViewCount();
    CSS.Attribute spanAtt = axis == X_AXIS ? CSS.Attribute.WIDTH
                                           : CSS.Attribute.HEIGHT;
    for (int i = 0; i < viewCount; i++)
      {
        View view = getView(i);
        int min = (int) view.getMinimumSpan(axis);
        int max;
        // Handle CSS span value of child.
        AttributeSet atts = view.getAttributes();
        Length length = (Length) atts.getAttribute(spanAtt);
        if (length != null)
          {
            min = Math.max((int) length.getValue(targetSpan), min);
            max = min;
          }
        else
          max = (int) view.getMaximumSpan(axis);

        if (max < targetSpan)
          {
            // Align child.
            float align = view.getAlignment(axis);
            offsets[i] = (int) ((targetSpan - max) * align);
            spans[i] = max;
          }
        else
          {
            offsets[i] = 0;
            spans[i] = Math.max(min, targetSpan);
          }
      }
  }

  /**
   * Paints using the given graphics configuration and shape.
   * This delegates to the css box painter to paint the
   * border and background prior to the interior.
   * 
   * @param g - Graphics configuration
   * @param a - the Shape to render into.
   */
  public void paint(Graphics g, Shape a)
  {
    Rectangle rect = a instanceof Rectangle ? (Rectangle) a : a.getBounds();
    painter.paint(g, rect.x, rect.y, rect.width, rect.height, this);
    super.paint(g, a);
  }
  
  /**
   * Fetches the attributes to use when painting.
   * 
   * @return the attributes of this model.
   */
  public AttributeSet getAttributes()
  {
    if (attributes == null)
      attributes = getStyleSheet().getViewAttributes(this);
    return attributes;
  }
  
  /**
   * Gets the resize weight.
   * 
   * @param axis - the axis to get the resize weight for.
   * @return the resize weight.
   * @throws IllegalArgumentException - for an invalid axis
   */
  public int getResizeWeight(int axis) throws IllegalArgumentException
  {
    // Can't resize the Y_AXIS
    if (axis == Y_AXIS)
      return 0;
    if (axis == X_AXIS)
      return 1;
    throw new IllegalArgumentException("Invalid Axis");
  }
  
  /**
   * Gets the alignment.
   * 
   * @param axis - the axis to get the alignment for.
   * @return the alignment.
   */
  public float getAlignment(int axis)
  {
    if (axis == X_AXIS)
      return 0.0F;
    if (axis == Y_AXIS)
      {
        if (getViewCount() == 0)
          return 0.0F;
        float prefHeight = getPreferredSpan(Y_AXIS);
        View first = getView(0);
        float firstRowHeight = first.getPreferredSpan(Y_AXIS);
        return prefHeight != 0 ? (firstRowHeight * first.getAlignment(Y_AXIS))
                                 / prefHeight
                               : 0;
      }
    throw new IllegalArgumentException("Invalid Axis");
  }
  
  /**
   * Gives notification from the document that attributes were
   * changed in a location that this view is responsible for.
   * 
   * @param ev - the change information
   * @param a - the current shape of the view
   * @param f - the factory to use to rebuild if the view has children.
   */
  public void changedUpdate(DocumentEvent ev,
                            Shape a, ViewFactory f)
  {
    super.changedUpdate(ev, a, f);
    
    // If more elements were added, then need to set the properties for them
    int currPos = ev.getOffset();
    if (currPos <= getStartOffset()
        && (currPos + ev.getLength()) >= getEndOffset())
        setPropertiesFromAttributes();
  }

  /**
   * Determines the preferred span along the axis.
   * 
   * @param axis - the view to get the preferred span for.
   * @return the span the view would like to be painted into >=0/
   * The view is usually told to paint into the span that is returned, 
   * although the parent may choose to resize or break the view.
   * @throws IllegalArgumentException - for an invalid axis
   */
  public float getPreferredSpan(int axis) throws IllegalArgumentException
  {
    if (axis == X_AXIS || axis == Y_AXIS)
      return super.getPreferredSpan(axis);
    throw new IllegalArgumentException("Invalid Axis");
  }
  
  /**
   * Determines the minimum span along the axis.
   * 
   * @param axis - the axis to get the minimum span for.
   * @return the span the view would like to be painted into >=0/
   * The view is usually told to paint into the span that is returned, 
   * although the parent may choose to resize or break the view.
   * @throws IllegalArgumentException - for an invalid axis
   */
  public float getMinimumSpan(int axis) throws IllegalArgumentException
  {
    if (axis == X_AXIS || axis == Y_AXIS)
      return super.getMinimumSpan(axis);
    throw new IllegalArgumentException("Invalid Axis");
  }
  
  /**
   * Determines the maximum span along the axis.
   * 
   * @param axis - the axis to get the maximum span for.
   * @return the span the view would like to be painted into >=0/
   * The view is usually told to paint into the span that is returned, 
   * although the parent may choose to resize or break the view.
   * @throws IllegalArgumentException - for an invalid axis
   */
  public float getMaximumSpan(int axis) throws IllegalArgumentException
  {
    if (axis == X_AXIS || axis == Y_AXIS)
      return super.getMaximumSpan(axis);
    throw new IllegalArgumentException("Invalid Axis");
  }
  
  /**
   * Updates any cached values that come from attributes.
   */
  protected void setPropertiesFromAttributes()
  {
    // Fetch attributes.
    StyleSheet ss = getStyleSheet();
    attributes = ss.getViewAttributes(this);

    // Fetch painter.
    painter = ss.getBoxPainter(attributes);

    // Update insets.
    if (attributes != null)
      {
        setInsets((short) painter.getInset(TOP, this),
                  (short) painter.getInset(LEFT, this),
                  (short) painter.getInset(BOTTOM, this),
                  (short) painter.getInset(RIGHT, this));
      }

    // Fetch width and height.
    cssSpans[X_AXIS] = (Length) attributes.getAttribute(CSS.Attribute.WIDTH);
    cssSpans[Y_AXIS] = (Length) attributes.getAttribute(CSS.Attribute.HEIGHT);
  }

  /**
   * Gets the default style sheet.
   * 
   * @return the style sheet
   */
  protected StyleSheet getStyleSheet()
  {
    HTMLDocument doc = (HTMLDocument) getDocument();
    return doc.getStyleSheet();
  }
}
