/**
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 */

package java.awt;


public class FlowLayout
  implements LayoutManager, java.io.Serializable
{
	/* XXX implement serial form! */
	private static final long serialVersionUID = -7262534875583282631L;
	final public static int LEFT = 0;
	final public static int CENTER = 1;
	final public static int RIGHT = 2;
	final public static int LEADING = 3;
	final public static int TRAILING = 4;
	int align;
	int hgap;
	int vgap;

public FlowLayout() {
	this(CENTER, 5, 5);
}

public FlowLayout( int align) {
	this( align, 5, 5);
}

public FlowLayout( int align, int hgap, int vgap) {
	this.align = align;
	this.hgap  = hgap;
	this.vgap  = vgap;
}

public void addLayoutComponent( String name, Component comp) {
}

private int alignComps( Container target, int start, int end, int w, int y) {
	Insets in = target.getInsets(); // getInsets() might be reimplemented (swing)
	int wm = target.width - in.left - in.right;
	int startx;
	int hMax = 0;

	if ( align == CENTER) {
		startx = in.left + (wm - w)/2;
	}
	else if ( align == RIGHT) {
		startx = wm - w;
	}
	else {
		startx = in.left + hgap;
	}
 
	for ( int i = start; i <= end; i++) {
		Component c = target.getComponent(i);
		if ( c.height > hMax ) {
			hMax = c.height;
		}
		c.setLocation(startx, y);
		startx += c.width + hgap;
	}

	return (hMax + vgap);
}

public int getAlignment() {
	return align;
}

public int getHgap() {
	return hgap;
}

public int getVgap() {
	return vgap;
}

public void layoutContainer( Container target) {
	Insets in = target.getInsets(); // getInsets() might be reimplemented (swing)
	int mw = target.width - in.left - in.right;
	int width = hgap;
	int ypos = vgap + in.top;
	int rowfirst = 0;
	int cc = target.getComponentCount();

	for ( int i = 0; i < cc; i++) {
		Component c = target.getComponent(i);
		if ( (c.flags & Component.IS_VISIBLE) == 0 ) {
			continue;
		}
		c.setSize( c.getPreferredSize() );
		if (width + c.width + hgap <= mw) {
			width += c.width + hgap;
		}
		else {
			if (i == rowfirst) {	 //single component too wide
				c.setBounds( in.left + hgap, ypos, mw - 2*hgap, c.height);
				rowfirst++;
				ypos += c.height + vgap;
			}
			else {
				ypos += alignComps( target, rowfirst, i-1, width, ypos);
				rowfirst = i;
				i--;
			}
			width = hgap;
		}
	}
	alignComps( target, rowfirst, cc-1, width, ypos);
}

public Dimension minimumLayoutSize( Container parent) {
	return preferredSize( parent, true);
}

public Dimension preferredLayoutSize( Container parent) {
	return preferredSize( parent, false);
}

private Dimension preferredSize( Container parent, boolean min) {
	int w = 0;
	int h = 0;
	int cc = parent.getComponentCount();

	for ( int i=0; i<cc; i++) {
		Component c = parent.getComponent(i);
		if ( (c.flags & Component.IS_VISIBLE) != 0 ) {
			Dimension dc = min ? c.getMinimumSize() : c.getPreferredSize();
			w += dc.width + hgap;
			if ( h < dc.height) {
				h = dc.height;
			}
		}
	}

	Insets in = parent.getInsets(); // getInsets() might be reimplemented (swing)
	w += in.left + in.right + hgap;
	h += in.top + in.bottom + 2*vgap;

	return new Dimension( w, h);
}

public void removeLayoutComponent( Component comp) {
}

public void setAlignment( int align) {
	this.align = align;
}

public void setHgap( int hgap) {
	this.hgap = hgap;
}

public void setVgap( int vgap) {
	this.vgap = vgap;
}

public String toString() {
	String s = getClass().getName() + ",vgap=" + vgap + ",hgap=" + hgap + ",align=";
	switch( align) {
	case LEFT:   s += "left";   break;
	case CENTER: s += "center"; break;
	case RIGHT:  s += "right";  break;
	}
	return s;
}
}
