/* TableView.java -- A table view for HTML tables
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


package javax.swing.text.html;

import java.awt.Shape;

import gnu.javax.swing.text.html.css.Length;

import javax.swing.SizeRequirements;
import javax.swing.event.DocumentEvent;
import javax.swing.text.AttributeSet;
import javax.swing.text.BoxView;
import javax.swing.text.Element;
import javax.swing.text.StyleConstants;
import javax.swing.text.View;
import javax.swing.text.ViewFactory;

/**
 * A view implementation that renders HTML tables.
 *
 * This is basically a vertical BoxView that contains the rows of the table
 * and the rows are horizontal BoxViews that contain the actual columns.
 */
class TableView
  extends BoxView
  implements ViewFactory
{

  /**
   * Represents a single table row.
   */
  class RowView
    extends BlockView
  {
    /**
     * Creates a new RowView.
     *
     * @param el the element for the row view
     */
    RowView(Element el)
    {
      super(el, X_AXIS);
    }

    public void replace(int offset, int len, View[] views)
    {
      super.replace(offset, len, views);
      gridValid = false;
    }

    /**
     * Overridden to make rows not resizable along the Y axis.
     */
    public float getMaximumSpan(int axis)
    {
      float span;
      if (axis == Y_AXIS)
        span = super.getPreferredSpan(axis);
      else
        span = Integer.MAX_VALUE;
      return span;
    }

    public float getMinimumSpan(int axis)
    {
      float span;
      if (axis == X_AXIS)
        span = totalColumnRequirements.minimum;
      else
        span = super.getMinimumSpan(axis);
      return span;
    }

    public float getPreferredSpan(int axis)
    {
      float span;
      if (axis == X_AXIS)
        span = totalColumnRequirements.preferred;
      else
        span = super.getPreferredSpan(axis);
      return span;
    }

    /**
     * Calculates the overall size requirements for the row along the
     * major axis. This will be the sum of the column requirements.
     */
    protected SizeRequirements calculateMajorAxisRequirements(int axis,
                                                            SizeRequirements r)
    {
      if (r == null)
        r = new SizeRequirements();
      int adjust = (columnRequirements.length + 1) * cellSpacing;
      r.minimum = totalColumnRequirements.minimum + adjust;
      r.preferred = totalColumnRequirements.preferred + adjust;
      r.maximum = totalColumnRequirements.maximum + adjust;
      r.alignment = 0.0F;
      return r;
    }

    /**
     * Lays out the columns in this row.
     */
    protected void layoutMajorAxis(int targetSpan, int axis, int[] offsets,
                                   int spans[])
    {
      int numCols = offsets.length;
      int realColumn = 0;
      for (int i = 0; i < numCols; i++)
        {
          View v = getView(i);
          if (v instanceof CellView)
            {
              CellView cv = (CellView) v;
              offsets[i] = columnOffsets[realColumn];
              spans[i] = 0;
              for (int j = 0; j < cv.colSpan; j++, realColumn++)
                {
                  spans[i] += columnSpans[realColumn];
                  if (j < cv.colSpan - 1)
                    spans[i] += cellSpacing;
                }
            }
        }
    }
  }

  /**
   * A view that renders HTML table cells (TD and TH tags).
   */
  class CellView
    extends BlockView
  {

    /**
     * The number of columns that this view spans.
     */
    int colSpan;

    /**
     * Creates a new CellView for the specified element.
     *
     * @param el the element for which to create the colspan
     */
    CellView(Element el)
    {
      super(el, Y_AXIS);
    }

    protected SizeRequirements calculateMajorAxisRequirements(int axis,
                                                            SizeRequirements r)
    {
      r = super.calculateMajorAxisRequirements(axis, r);
      r.maximum = Integer.MAX_VALUE;
      return r;
    }

    /**
     * Overridden to fetch the columnSpan attibute.
     */
    protected void setPropertiesFromAttributes()
    {
      super.setPropertiesFromAttributes();
      colSpan = 1;
      AttributeSet atts = getAttributes();
      Object o = atts.getAttribute(HTML.Attribute.COLSPAN);
      if (o != null)
        {
          try
            {
              colSpan = Integer.parseInt(o.toString());
            }
          catch (NumberFormatException ex)
            {
              // Couldn't parse the colspan, assume 1.
              colSpan = 1;
            }
        }
    }
  }


  /**
   * The attributes of this view.
   */
  private AttributeSet attributes;

  /**
   * The column requirements.
   *
   * Package private to avoid accessor methods.
   */
  SizeRequirements[] columnRequirements;

  /**
   * The overall requirements across all columns.
   *
   * Package private to avoid accessor methods.
   */
  SizeRequirements totalColumnRequirements;

  /**
   * The column layout, offsets.
   *
   * Package private to avoid accessor methods.
   */
  int[] columnOffsets;

  /**
   * The column layout, spans.
   *
   * Package private to avoid accessor methods.
   */
  int[] columnSpans;

  /**
   * The widths of the columns that have been explicitly specified.
   */
  Length[] columnWidths;

  /**
   * Indicates if the grid setup is ok.
   */
  boolean gridValid;

  /**
   * Additional space that is added _between_ table cells.
   *
   * This is package private to avoid accessor methods.
   */
  int cellSpacing;

  /**
   * Creates a new HTML table view for the specified element.
   *
   * @param el the element for the table view
   */
  public TableView(Element el)
  {
    super(el, Y_AXIS);
    totalColumnRequirements = new SizeRequirements();
  }

  /**
   * Implementation of the ViewFactory interface for creating the
   * child views correctly.
   */
  public View create(Element elem)
  {
    View view = null;
    AttributeSet atts = elem.getAttributes();
    Object name = atts.getAttribute(StyleConstants.NameAttribute);
    if (name instanceof HTML.Tag)
      {
        HTML.Tag tag = (HTML.Tag) name;
        if (tag == HTML.Tag.TR)
          view = new RowView(elem);
        else if (tag == HTML.Tag.TD || tag == HTML.Tag.TH)
          view = new CellView(elem);
        else if (tag == HTML.Tag.CAPTION)
          view = new ParagraphView(elem);
      }

    // If we haven't mapped the element, then fall back to the standard
    // view factory.
    if (view == null)
      {
        View parent = getParent();
        if (parent != null)
          {
            ViewFactory vf = parent.getViewFactory();
            if (vf != null)
              view = vf.create(elem);
          }
      }
    return view;
  }

  /**
   * Returns this object as view factory so that we get our TR, TD, TH
   * and CAPTION subelements created correctly.
   */
  public ViewFactory getViewFactory()
  {
    return this;
  }

  /**
   * Returns the attributes of this view. This is overridden to provide
   * the attributes merged with the CSS stuff.
   */
  public AttributeSet getAttributes()
  {
    if (attributes == null)
      attributes = getStyleSheet().getViewAttributes(this);
    return attributes;
  }

  /**
   * Returns the stylesheet associated with this view.
   *
   * @return the stylesheet associated with this view
   */
  private StyleSheet getStyleSheet()
  {
    HTMLDocument doc = (HTMLDocument) getDocument();
    return doc.getStyleSheet();
  }

  /**
   * Overridden to calculate the size requirements according to the
   * columns distribution.
   */
  protected SizeRequirements calculateMinorAxisRequirements(int axis,
                                                            SizeRequirements r)
  {
    updateGrid();
    calculateColumnRequirements();

    // Calculate the horizontal requirements according to the superclass.
    // This will return the maximum of the row's widths.
    r = super.calculateMinorAxisRequirements(axis, r);

    // Try to set the CSS width if it fits.
    AttributeSet atts = getAttributes();
    Length l = (Length) atts.getAttribute(CSS.Attribute.WIDTH);
    if (l != null)
      {
        int width = (int) l.getValue();
        if (r.minimum < width)
          r.minimum = width;
      }

    // Adjust requirements when we have cell spacing.
    int adjust = (columnRequirements.length + 1) * cellSpacing;
    r.minimum += adjust;
    r.preferred += adjust;

    // Apply the alignment.
    Object o = atts.getAttribute(CSS.Attribute.TEXT_ALIGN);
    r.alignment = 0.0F;
    if (o != null)
      {
        String al = o.toString();
        if (al.equals("left"))
          r.alignment = 0.0F;
        else if (al.equals("center"))
          r.alignment = 0.5F;
        else if (al.equals("right"))
          r.alignment = 1.0F;
      }

    // Make it not resize in the horizontal direction.
    r.maximum = r.preferred;
    return r;
  }

  /**
   * Overridden to perform the table layout before calling the super
   * implementation.
   */
  protected void layoutMinorAxis(int targetSpan, int axis, int[] offsets, 
                                 int[] spans)
  {
    updateGrid();

    // Mark all rows as invalid.
    int n = getViewCount();
    for (int i = 0; i < n; i++)
      {
        View row = getView(i);
        if (row instanceof RowView)
          ((RowView) row).layoutChanged(axis);
      }

    layoutColumns(targetSpan);
    super.layoutMinorAxis(targetSpan, axis, offsets, spans);
  }

  /**
   * Calculates the size requirements for the columns.
   */
  private void calculateColumnRequirements()
  {
    int numRows = getViewCount();
    totalColumnRequirements.minimum = 0;
    totalColumnRequirements.preferred = 0;
    totalColumnRequirements.maximum = 0;

    // In this first pass we find out a suitable total width to fit in
    // all columns of all rows.
    for (int r = 0; r < numRows; r++)
      {
        View rowView = getView(r);
        int numCols;
        if (rowView instanceof RowView)
          numCols = ((RowView) rowView).getViewCount();
        else
          numCols = 0;

        // We collect the normal (non-relative) column requirements in the
        // total variable and the relative requirements in the relTotal
        // variable. In the end we create the maximum of both to get the
        // real requirements.
        SizeRequirements total = new SizeRequirements();
        SizeRequirements relTotal = new SizeRequirements();
        float totalPercent = 0.F;
        for (int c = 0; c < numCols; )
          {
            View v = rowView.getView(c);
            if (v instanceof CellView)
              {
                CellView cellView = (CellView) v;
                int colSpan = cellView.colSpan;
                if (colSpan > 1)
                  {
                    int cellMin = (int) cellView.getMinimumSpan(X_AXIS);
                    int cellPref = (int) cellView.getPreferredSpan(X_AXIS);
                    int cellMax = (int) cellView.getMaximumSpan(X_AXIS);
                    int currentMin = 0;
                    int currentPref = 0;
                    long currentMax = 0;
                    for (int i = 0; i < colSpan; i++)
                      {
                        SizeRequirements req = columnRequirements[c + i];
                        currentMin += req.minimum;
                        currentPref += req.preferred;
                        currentMax += req.maximum;
                      }
                    int deltaMin = cellMin - currentMin;
                    int deltaPref = cellPref - currentPref;
                    int deltaMax = (int) (cellMax - currentMax);
                    // Distribute delta.
                    for (int i = 0; i < colSpan; i++)
                      {
                        SizeRequirements req = columnRequirements[c + i];
                        if (deltaMin > 0)
                          req.minimum += deltaMin / colSpan;
                        if (deltaPref > 0)
                          req.preferred += deltaPref / colSpan;
                        if (deltaMax > 0)
                          req.maximum += deltaMax / colSpan;
                        if (columnWidths[c + i] == null
                            || ! columnWidths[c + i].isPercentage())
                          {
                            total.minimum += req.minimum;
                            total.preferred += req.preferred;
                            total.maximum += req.maximum;
                          }
                        else
                          {
                            relTotal.minimum =
                              Math.max(relTotal.minimum,
                                     (int) (req.minimum
                                            * columnWidths[c + i].getValue()));
                            relTotal.preferred =
                              Math.max(relTotal.preferred,
                                     (int) (req.preferred
                                            * columnWidths[c + i].getValue()));
                            relTotal.maximum =
                              Math.max(relTotal.maximum,
                                     (int) (req.maximum
                                            * columnWidths[c + i].getValue()));
                            totalPercent += columnWidths[c + i].getValue();
                          }
                      }
                  }
                else
                  {
                    // Shortcut for colSpan == 1.
                    SizeRequirements req = columnRequirements[c];
                    req.minimum = Math.max(req.minimum,
                                           (int) cellView.getMinimumSpan(X_AXIS));
                    req.preferred = Math.max(req.preferred,
                                             (int) cellView.getPreferredSpan(X_AXIS));
                    req.maximum = Math.max(req.maximum,
                                           (int) cellView.getMaximumSpan(X_AXIS));
                    if (columnWidths[c] == null
                        || ! columnWidths[c].isPercentage())
                      {
                        total.minimum += columnRequirements[c].minimum;
                        total.preferred += columnRequirements[c].preferred;
                        total.maximum += columnRequirements[c].maximum;
                      }
                    else
                      {
                        relTotal.minimum =
                          Math.max(relTotal.minimum,
                                 (int) (req.minimum
                                        / columnWidths[c].getValue()));
                        relTotal.preferred =
                          Math.max(relTotal.preferred,
                                 (int) (req.preferred
                                        / columnWidths[c].getValue()));
                        relTotal.maximum =
                          Math.max(relTotal.maximum,
                                 (int) (req.maximum
                                        / columnWidths[c].getValue()));
                        totalPercent += columnWidths[c].getValue();
                      }
                  }
                c += colSpan;
              }
            else
              c++;
          }

        // Update the total requirements as follows:
        // 1. Multiply the absolute requirements with 1 - totalPercent. This
        //    gives the total requirements based on the wishes of the absolute
        //    cells.
        // 2. Take the maximum of this value and the total relative
        //    requirements. Now we should have enough space for whatever cell
        //    in this column.
        // 3. Take the maximum of this value and the previous maximum value.
        total.minimum *= 1.F / (1.F - totalPercent);
        total.preferred *= 1.F / (1.F - totalPercent);
        total.maximum *= 1.F / (1.F - totalPercent);

        int rowTotalMin = Math.max(total.minimum, relTotal.minimum);
        int rowTotalPref = Math.max(total.preferred, relTotal.preferred);
        int rowTotalMax = Math.max(total.maximum, relTotal.maximum);
        totalColumnRequirements.minimum =
          Math.max(totalColumnRequirements.minimum, rowTotalMin);
        totalColumnRequirements.preferred =
          Math.max(totalColumnRequirements.preferred, rowTotalPref);
        totalColumnRequirements.maximum =
          Math.max(totalColumnRequirements.maximum, rowTotalMax);
      }

    // Now we know what we want and can fix up the actual relative
    // column requirements.
    int numCols = columnRequirements.length;
    for (int i = 0; i < numCols; i++)
      {
        if (columnWidths[i] != null)
          {
            columnRequirements[i].minimum = (int)
              columnWidths[i].getValue(totalColumnRequirements.minimum);
            columnRequirements[i].preferred = (int)
              columnWidths[i].getValue(totalColumnRequirements.preferred);
            columnRequirements[i].maximum = (int)
              columnWidths[i].getValue(totalColumnRequirements.maximum);
          }
      }
  }

  /**
   * Lays out the columns.
   *
   * @param targetSpan the target span into which the table is laid out
   */
  private void layoutColumns(int targetSpan)
  {
    // Set the spans to the preferred sizes. Determine the space
    // that we have to adjust the sizes afterwards.
    long sumPref = 0;
    int n = columnRequirements.length;
    for (int i = 0; i < n; i++)
      {
        SizeRequirements col = columnRequirements[i];
        if (columnWidths[i] != null)
          columnSpans[i] = (int) columnWidths[i].getValue(targetSpan);
        else
          columnSpans[i] = col.preferred;
        sumPref += columnSpans[i];
      }

    // Try to adjust the spans so that we fill the targetSpan.
    // For adjustments we have to use the targetSpan minus the cumulated
    // cell spacings.
    long diff = targetSpan - (n + 1) * cellSpacing - sumPref;
    float factor = 0.0F;
    int[] diffs = null;
    if (diff != 0)
      {
        long total = 0;
        diffs = new int[n];
        for (int i = 0; i < n; i++)
          {
            // Only adjust the width if we haven't set a column width here.
            if (columnWidths[i] == null)
              {
                SizeRequirements col = columnRequirements[i];
                int span;
                if (diff < 0)
                  {
                    span = col.minimum;
                    diffs[i] = columnSpans[i] - span;
                  }
                else
                  {
                    span = col.maximum;
                    diffs[i] = span - columnSpans[i];
                  }
                total += span;
              }
            else
              total += columnSpans[i];
          }

        float maxAdjust = Math.abs(total - sumPref);
        factor = diff / maxAdjust;
        factor = Math.min(factor, 1.0F);
        factor = Math.max(factor, -1.0F);
      }

    // Actually perform adjustments.
    int totalOffs = cellSpacing;
    for (int i = 0; i < n; i++)
      {
        columnOffsets[i] = totalOffs;
        if (diff != 0)
          {
            float adjust = factor * diffs[i];
            columnSpans[i] += Math.round(adjust);
          }
        // Avoid overflow here.
        totalOffs = (int) Math.min((long) totalOffs + (long) columnSpans[i]
                                   + (long) cellSpacing, Integer.MAX_VALUE);
      }
  }

  /**
   * Updates the arrays that contain the row and column data in response
   * to a change to the table structure.
   */
  private void updateGrid()
  {
    if (! gridValid)
      {
        int maxColumns = 0;
        int numRows = getViewCount();
        for (int r = 0; r < numRows; r++)
          {
            View rowView = getView(r);
            int numCols;
            if (rowView instanceof RowView)
              numCols = ((RowView) rowView).getViewCount();
            else
              numCols = 0;
            maxColumns = Math.max(numCols, maxColumns);
          }
        columnWidths = new Length[maxColumns];
        for (int r = 0; r < numRows; r++)
          {
            View rowView = getView(r);
            int numCols;
            if (rowView instanceof RowView)
              numCols = ((RowView) rowView).getViewCount();
            else
              numCols = 0;
            int colIndex = 0;
            for (int c = 0; c < numCols; c++)
              {
                View v = rowView.getView(c);
                if (v instanceof CellView)
                  {
                    CellView cv = (CellView) v;
                    Object o =
                      cv.getAttributes().getAttribute(CSS.Attribute.WIDTH);
                    if (o != null && columnWidths[colIndex] == null
                        && o instanceof Length)
                      columnWidths[colIndex]= (Length) o;
                    colIndex += cv.colSpan;
                  }
              }
          }
        columnRequirements = new SizeRequirements[maxColumns];
        for (int i = 0; i < maxColumns; i++)
          columnRequirements[i] = new SizeRequirements();
        columnOffsets = new int[maxColumns];
        columnSpans = new int[maxColumns];

        gridValid = true;
      }
  }

  /**
   * Overridden to restrict the table width to the preferred size.
   */
  public float getMaximumSpan(int axis)
  {
    float span;
    if (axis == X_AXIS)
      span = super.getPreferredSpan(axis);
    else
      span = super.getMaximumSpan(axis);
    return span;
  }

  /**
   * Overridden to fetch the CSS attributes when view gets connected.
   */
  public void setParent(View parent)
  {
    super.setParent(parent);
    if (parent != null)
      setPropertiesFromAttributes();
  }

  /**
   * Fetches CSS and HTML layout attributes.
   */
  private void setPropertiesFromAttributes()
  {
    // Fetch and parse cell spacing.
    Object o = getAttributes().getAttribute(CSS.Attribute.BORDER_SPACING);
    if (o != null && o instanceof Length)
      {
        Length l = (Length) o;
        cellSpacing = (int) l.getValue();
      }
  }

  /**
   * Overridden to adjust for cellSpacing.
   */
  protected SizeRequirements calculateMajorAxisRequirements(int axis,
                                                            SizeRequirements r)
  {
    r = super.calculateMajorAxisRequirements(axis, r);
    int adjust = (getViewCount() + 1) * cellSpacing;
    r.minimum += adjust;
    r.preferred += adjust;
    r.maximum += adjust;
    return r;
  }

  /**
   * Overridden to adjust for cellSpacing.
   */
  protected void layoutMajorAxis(int targetSpan, int axis, int[] offsets,
                                 int spans[])
  {
    int adjust = (getViewCount() + 1) * cellSpacing;
    super.layoutMajorAxis(targetSpan - adjust, axis, offsets, spans);
    for (int i = 0; i < offsets.length; i++)
      {
        offsets[i] += (i + 1) * cellSpacing;
      }
  }

  /**
   * Overridden to replace view factory with this one.
   */
  public void insertUpdate(DocumentEvent e, Shape a, ViewFactory f)
  {
    super.insertUpdate(e, a, this);
  }

  /**
   * Overridden to replace view factory with this one.
   */
  public void removeUpdate(DocumentEvent e, Shape a, ViewFactory f)
  {
    super.removeUpdate(e, a, this);
  }

  /**
   * Overridden to replace view factory with this one.
   */
  public void changedUpdate(DocumentEvent e, Shape a, ViewFactory f)
  {
    super.changedUpdate(e, a, this);
  }

  public void replace(int offset, int len, View[] views)
  {
    super.replace(offset, len, views);
    gridValid = false;
  }
}
