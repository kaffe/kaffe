package java.awt;


/**
 * class BorderLayout - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
public class BorderLayout
  implements LayoutManager2, java.io.Serializable
{
	/* XXX implement serial form! */
	private static final long serialVersionUID = -8658291919501921765L;
	Component center;
	Component north;
	Component west;
	Component south;
	Component east;
	int hGap;
	int vGap;
	final public static String CENTER = "Center";
	final public static String NORTH = "North";
	final public static String WEST = "West";
	final public static String SOUTH = "South";
	final public static String EAST = "East";
	final public static String AFTER_LAST_LINE = "Last";
	final public static String AFTER_LINE_ENDS = "After";
	final public static String BEFORE_FIRST_LINE = "First";
	final public static String BEFORE_LINE_BEGINS = "Before";

/**
 * Constructs a new BorderLayout with no gaps between components.
 */
public BorderLayout() {
	this(0, 0);
}

/**
 * Constructs a BorderLayout with the specified gaps.
 * @param hgap the horizontal gap
 * @param vgap the vertical gap
 */
public BorderLayout ( int hgap, int vgap ) {
	hGap = hgap;
	vGap = vgap;
}

/**
 * Adds the specified component to the layout, using the specified
 * constraint object.
 * @param comp the component to be added
 * @param constraints  where/how the component is added to the layout.
 */
public void addLayoutComponent ( Component cmp, Object position ) {
	try {
		addLayoutComponent((String)position, cmp);
	}
	catch (ClassCastException _) {
		throw new IllegalArgumentException( getClass().getName() + ": invalid constraint " + position );
	}
}

/**
 * Replaced by addLayoutComponent(Component, Object).
 * @deprecated
 */
public void addLayoutComponent ( String pos, Component cmp ) {
	if ( pos == null ) {
		pos = CENTER;
	}
	if (CENTER.equalsIgnoreCase(pos)) {
		center = cmp;
	}
	else if (NORTH.equalsIgnoreCase(pos)) {
		north = cmp;
	}
	else if (WEST.equalsIgnoreCase(pos)) {
	 	west = cmp;
	}
	else if (SOUTH.equalsIgnoreCase(pos)) {
		south = cmp;
	}
	else if (EAST.equalsIgnoreCase(pos)) {
		east = cmp;
	}
	else {
		throw new IllegalArgumentException( getClass().getName() + ": invalid position: " + pos);
	}
}

/**
 * Returns the horizontal gap between components.
 */
public int getHgap() {
	return hGap;
}

/**
 * Returns the alignment along the x axis.  This specifies how
 * the component would like to be aligned relative to other 
 * components.  The value should be a number between 0 and 1
 * where 0 represents alignment along the origin, 1 is aligned
 * the furthest away from the origin, 0.5 is centered, etc.
 */
public float getLayoutAlignmentX ( Container cntr ) {
	return (Component.CENTER_ALIGNMENT);
}

/**
 * Returns the alignment along the y axis.  This specifies how
 * the component would like to be aligned relative to other 
 * components.  The value should be a number between 0 and 1
 * where 0 represents alignment along the origin, 1 is aligned
 * the furthest away from the origin, 0.5 is centered, etc.
 */
public float getLayoutAlignmentY ( Container cntr ) {
	return (Component.CENTER_ALIGNMENT);
}

/**
 * Returns the vertical gap between components.
 */
public int getVgap() {
	return vGap;
}

/**
 * Invalidates the layout, indicating that if the layout manager
 * has cached information it should be discarded.
 */
public void invalidateLayout ( Container cntr ) {
}

/**
 * Lays out the specified container. This method will actually reshape the
 * components in the specified target container in order to satisfy the 
 * constraints of the BorderLayout object. 
 * @param target the component being laid out
 * @see Container
 */
public void layoutContainer ( Container cntr ) {
	Insets insets = cntr.getInsets(); // getInsets() might be reimplemented (swing)
	int top = insets.top;
	int bottom = cntr.height - insets.bottom;
	int left = insets.left;
	int right = cntr.width - insets.right;
	Dimension pd;

	// watch out - JDK seems to bypass isVisible() (which might be resolved, e.g. by Swing)

	if ( (north != null) && ((north.flags & Component.IS_VISIBLE) != 0) ) {
		pd = north.getPreferredSize();
		north.setBounds(left, top, right - left, pd.height);
		top += pd.height + vGap;
	}
	if ((south != null) && ((south.flags & Component.IS_VISIBLE) != 0) ) {
		pd = south.getPreferredSize();
		south.setBounds(left, bottom - pd.height, right - left, pd.height);
		bottom -= pd.height + vGap;
	}
	if ((east != null) && ((east.flags & Component.IS_VISIBLE) != 0) ) {
		pd = east.getPreferredSize();
		east.setBounds(right - pd.width, top, pd.width, bottom - top);
		right -= pd.width + hGap;
	}
	if ((west != null) && ((west.flags & Component.IS_VISIBLE) != 0) ) {
		pd = west.getPreferredSize();
		west.setBounds(left, top, pd.width, bottom - top);
		left += pd.width + hGap;
	}
	if ((center != null) && ((center.flags & Component.IS_VISIBLE) != 0) )
		center.setBounds(left, top, right - left, bottom - top);
}

/**
 * Returns the maximum dimensions for this layout given the components
 * in the specified target container.
 * @param target the component which needs to be laid out
 * @see Container
 * @see #minimumLayoutSize
 * @see #preferredLayoutSize
 */
public Dimension maximumLayoutSize ( Container cntr ) {
	return new Dimension( Integer.MAX_VALUE, Integer.MAX_VALUE);
}

/**
 * Returns the minimum dimensions needed to layout the components
 * contained in the specified target container. 
 * @param target the Container on which to do the layout
 * @see Container
 * @see #preferredLayoutSize
 */
public Dimension minimumLayoutSize ( Container cntr ) {
	Dimension md, d = new Dimension();

	// watch out - JDK seems to ignore isVisible() (might be resolved, e.g. by swing)

	if ((east != null) && ((east.flags & Component.IS_VISIBLE) != 0) ) {
		md = east.getMinimumSize();
		d.width += md.width + hGap;
		d.height = Math.max( md.height, d.height);
	}
	if ((west != null) && ((west.flags & Component.IS_VISIBLE) != 0) ){
		md = west.getMinimumSize();
		d.width += md.width + hGap;
		d.height = Math.max( md.height, d.height);
	}
	if ((center != null) && ((center.flags & Component.IS_VISIBLE) != 0) ){
		md = center.getMinimumSize();
		d.width += md.width;
		d.height = Math.max( md.height, d.height);
	}
	if ((north != null) && ((north.flags & Component.IS_VISIBLE) != 0) ){
		md = north.getMinimumSize();
		d.width = Math.max( md.width, d.width);
		d.height += md.height + vGap;
	}
	if ((south != null) && ((south.flags & Component.IS_VISIBLE) != 0) ) {
		md = south.getMinimumSize();
		d.width = Math.max( md.width, d.width);
		d.height += md.height + vGap;
	}

	Insets insets = cntr.getInsets(); // getInsets() might be reimplemented (swing)
	d.width += insets.left + insets.right;
	d.height += insets.top + insets.bottom;

	return d;
}

/**
 * Returns the preferred dimensions for this layout given the components
 * in the specified target container.
 * @param target the component which needs to be laid out
 * @see Container
 * @see #minimumLayoutSize
 */
public Dimension preferredLayoutSize ( Container cntr ) {
	Dimension pd, d = new Dimension();

	// watch out - JDK seems to ignore setVisible() (which might be resolved, e.g. by swing)

	if ((east != null) && ((east.flags & Component.IS_VISIBLE) != 0) ) {
		pd = east.getPreferredSize();
		d.width += pd.width + hGap;
		d.height = Math.max( pd.height, d.height);
	}
	if ((west != null) && ((west.flags & Component.IS_VISIBLE) != 0) ) {
		pd = west.getPreferredSize();
		d.width += pd.width + hGap;
		d.height = Math.max( pd.height, d.height);
	}
	if ((center != null) && ((center.flags & Component.IS_VISIBLE) != 0) ) {
		pd = center.getPreferredSize();
		d.width += pd.width;
		d.height = Math.max( pd.height, d.height);
	}
	if ((north != null) && ((north.flags & Component.IS_VISIBLE) != 0) ) {
		pd = north.getPreferredSize();
		d.width = Math.max( pd.width, d.width);
		d.height += pd.height + vGap;
	}
	if ((south != null) && ((south.flags & Component.IS_VISIBLE) != 0) ){
		pd = south.getPreferredSize();
		d.width = Math.max( pd.width, d.width);
		d.height += pd.height + vGap;
	}

	Insets insets = cntr.getInsets(); // getInsets() might be reimplemented (swing)
	d.width += insets.left + insets.right;
	d.height += insets.top + insets.bottom;

	return d;
}

/**
 * Removes the specified component from the layout.
 * @param comp the component to be removed
 */
public void removeLayoutComponent ( Component cmp ) {
	if (cmp == center) {
		center = null;
	}
	else if (cmp == north) {
		north = null;
	}
	else if (cmp == west) {
		west = null;
	}
	else if (cmp == south) {
		south = null;
	}
	else if (cmp == east) {
		east = null;
	}
}

/**
 * Sets the horizontal gap between components.
 * @param hgap the horizontal gap between components
 */
public void setHgap ( int hgap ) {
	hGap = hgap;
}

/**
 * Sets the vertical gap between components.
 * @param vgap the vertical gap between components
 */
public void setVgap ( int vgap ) {
	vGap = vgap;
}

/**
 * Returns the String representation of this BorderLayout's values.
 */
public String toString() {
	return getClass().getName() + "[hgap=" + hGap + ",vgap=" + vGap + ']';
}
}
