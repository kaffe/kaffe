/*
 * ScrollPane - widget for scrolling a child component
 *
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *      Kaffe.org contributors, see ChangeLog for details.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */

package java.awt;

import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;

public class ScrollPane
  extends Container
  implements AdjustmentListener
{
	final private static long serialVersionUID = 7956609840827222915L;
	final public static int SCROLLBARS_AS_NEEDED = 0;
	final public static int SCROLLBARS_ALWAYS = 1;
	final public static int SCROLLBARS_NEVER = 2;
	int policy;
	SPAdjustable hScroll;
	SPAdjustable vScroll;
	Point pos;
	Component child;
	ChildWrapper wrapper;
	static int named;

class SPAdjustable
  extends Scrollbar
{
SPAdjustable ( int orientation ) {
	super( orientation);
}

public void setMaximum ( int max ) {
	throw new AWTError( "can't set scroll maximum for ScrollPane");
}

public void setMinimum ( int min ) {
	throw new AWTError( "can't set scroll minimum for ScrollPane");
}

public void setVisibleAmount ( int a ) {
	throw new AWTError( "can't set visible amount for ScrollPane");
}
}
/**
 * This is almost a dummy. We just can't directly add the child since it would
 * require Component.contains() to be routed via the whole parent chain, checking
 * for a parent which has its own notion of child clipping. This class is used to
 * reduce this to the standard case of child-to-parent clipping
 */
class ChildWrapper
  extends Container
{
ChildWrapper () {
	this.layoutm = null;
}

public void doLayout () {
}

public void paint( Graphics g) {
	g.paintChild(child, false);
}
}

public ScrollPane () {
	this( SCROLLBARS_AS_NEEDED);
}

public ScrollPane ( int policy ) {
	if (GraphicsEnvironment.isHeadless ())
		throw new HeadlessException ();

	layoutm = null;
	int bw = BORDER_WIDTH;
	
	if ( (policy >= 0) && (policy <= 2) )
		this.policy = policy;
	else
		throw new IllegalArgumentException( "illegal ScrollPane policy");
	
	pos = new Point();
	hScroll = new SPAdjustable( Scrollbar.HORIZONTAL);
	vScroll = new SPAdjustable( Scrollbar.VERTICAL);
	
	add( hScroll);
	add( vScroll);
	
	hScroll.addAdjustmentListener( this);
	vScroll.addAdjustmentListener( this);
	
	if ( policy != SCROLLBARS_ALWAYS ) {
		hScroll.flags &= ~IS_VISIBLE;
		vScroll.flags &= ~IS_VISIBLE;
	}
}

final protected void addImpl ( Component child, Object constraint, int idx ){
	if (child == hScroll || child == vScroll) {
		super.addImpl( child, constraint, -1);
		
	}
	else {
		if (wrapper == null) {
			wrapper = new ChildWrapper();
			super.addImpl(wrapper, constraint, -1);
		}
		if (this.child != null) {
			wrapper.remove(this.child);
		}
		this.child = child;
		wrapper.add(child);
	}
}

public void adjustmentValueChanged ( AdjustmentEvent e) {
	if ( child == null )
		return;

	Object   src = e.getSource();
	int      val = e.getValue();
	int      w = wrapper.width;
	int      h = wrapper.height;
	int      delta, w_delta, h_delta;
	
	if ( src == vScroll) {
		delta = val - pos.y;
		pos.y = val;
	}
	else {
		delta = val - pos.x;
		pos.x = val;
	}

	// fake visibility while repositioning child
	child.flags &= ~IS_VISIBLE;
	if ( child.height < h ) {
		child.setLocation(-pos.x, (h - child.height) / 2);
	} else {
		child.setLocation(-pos.x, -pos.y);
	}
	
	child.flags |= IS_VISIBLE;

	Graphics g = wrapper.getGraphics();

	//just copy and clip ( background validation done by wrapper.paint() )
	if ( g != null ) {
		if ( src == vScroll ) {
			if ( delta > 0 ) {  // scroll downwards
				if ( delta < h ) {
					h_delta = h - delta;
					g.copyArea(0, delta, w, h_delta, 0, -delta);
					g.clipRect(0, h_delta, w, delta + 1);
				}
			}
			else {              // scroll upwards
				if ( -delta < h ) {
					g.copyArea(0, 0, w, h + delta, 0, -delta);
					g.clipRect(0, 0, w, -delta + 1);
				}
			}
		}
		else {
			if ( delta > 0 ) {  // scroll right
				if ( delta < w ) {
					w_delta = w - delta;
					g.copyArea(delta, 0, w_delta, h, -delta, 0);
					g.clipRect(w_delta, 0, delta, h);
				}
			}
			else {              // scroll left
				if ( -delta < w ) {
					g.copyArea(0, 0, w + delta, h, -delta, 0);
					g.clipRect( 0, 0, -delta, h);
				}
			}
		}

		wrapper.update(g);
		g.dispose();
	}
}

void checkScrollbarVisibility ( Dimension cd ) {
	int w = width - insets.left - insets.right;
	int h = height - insets.top - insets.bottom;

	if ( (hScroll.flags & IS_VISIBLE) != 0 ) {
		if ( cd.width <= w ) {
			hScroll.setVisible( false);
		}
	}
	else {
		if ( cd.width > w ){
			hScroll.setVisible( true);
		}
	}
	
	if ( (vScroll.flags & IS_VISIBLE) != 0 ) {
		if ( cd.height <= h ) {
			vScroll.setVisible( false);
		}
	}
	else {
		if ( cd.height > h ){
			vScroll.setVisible( true);
		}
	}
}

public void doLayout () {
	layout();
}

String getGenericName () {
	/*
	 * Apparently, the JDK increments the running number each time
	 * after there was a genericName request. And yes, the stem is 
	 * of course not the class name
	 */
	return "scrollpane" + (named++);
}

public Adjustable getHAdjustable () {
	return hScroll;
}

public int getHScrollbarHeight () {
	return hScroll.preferredSize().height;
}

public Dimension getPreferredSize() {
	// seems to be a fixed in Sun JDK
	// return new Dimension(100, 100);
	return super.getPreferredSize();
}

/**
  * Returns the current scroll position of the viewport.
  *
  * @return The current scroll position of the viewport.
  */
public Point getScrollPosition () {
	int x = 0;
	int y = 0;

	Adjustable v = getVAdjustable();
	Adjustable h = getHAdjustable();

	if (v != null)
		y = v.getValue();
	if (h != null)
		x = h.getValue();

	return(new Point(x, y));
	// return new Point(pos);
}

public int getScrollbarDisplayPolicy () {
	return policy;
}

public Adjustable getVAdjustable () {
	return vScroll;
}

/**
  * Returns the current viewport size.  The viewport is the region of
  * this object's window where the child is actually displayed.
  *
  * @return The viewport size.
  */
public Dimension getViewportSize ()
{
	Dimension viewsize = getSize();
	//Insets insets = getInsets();

	viewsize.width -= (insets.left + insets.right);
	viewsize.height -= (insets.top + insets.bottom);

	Component[] list = getComponents();
	if ((list == null) || (list.length <= 0))
		return viewsize;

	Dimension dim = list[0].getPreferredSize();

	if (dim.width <= 0 && dim.height <= 0)
		return viewsize;

	int vScrollbarWidth = getVScrollbarWidth ();
	int hScrollbarHeight = getHScrollbarHeight ();

	if (policy == SCROLLBARS_ALWAYS) {
		viewsize.width -= vScrollbarWidth;
		viewsize.height -= hScrollbarHeight;
		return viewsize;
	}

	if (policy == SCROLLBARS_NEVER)
		return viewsize;

	// The scroll policy is SCROLLBARS_AS_NEEDED, so we need to see if
	// either scrollbar is needed.

	// Assume we don't need either scrollbar.
	boolean mayNeedVertical = false;
	boolean mayNeedHorizontal = false;

	boolean needVertical = false;
	boolean needHorizontal = false;

	// Check if we need vertical scrollbars.  If we do, then we need to
	// subtract the width of the vertical scrollbar from the viewport's
	// width.
	if (dim.height > viewsize.height)
		needVertical = true;
	else if (dim.height > (viewsize.height - hScrollbarHeight))
		// This is tricky.  In this case the child is tall enough that its
		// bottom edge would be covered by a horizontal scrollbar, if one
		// were present.  This means that if there's a horizontal
		// scrollbar then we need a vertical scrollbar.
		mayNeedVertical = true;

	if (dim.width > viewsize.width)
		needHorizontal = true;
	else if (dim.width > (viewsize.width - vScrollbarWidth))
		mayNeedHorizontal = true;

	if (needVertical && mayNeedHorizontal)
		needHorizontal = true;

	if (needHorizontal && mayNeedVertical)
		needVertical = true;

	if (needHorizontal)
		viewsize.height -= hScrollbarHeight;

	if (needVertical)
		viewsize.width -= vScrollbarWidth;

	return viewsize;
	/* Kaffe's original implementation.
	public Dimension getViewportSize () {
        	return new Dimension( width - (insets.left + insets.right),
                	              height - (insets.top + insets.bottom));
	}
	 */
}

public int getVScrollbarWidth () {
	return vScroll.preferredSize().width;
}

public boolean isFocusTraversable () {
	return false;
}

/**
  * Lays out this component.  This consists of resizing the sole child
  * component to its perferred size.
  *
  * @deprecated This method is deprecated in favor of
  * <code>doLayout()</code>.
  */
public void layout() {
	if ( child == null ) {
		return;
	}

	Dimension cd = child.getPreferredSize();
	int w, h;

	flags |= IS_LAYOUTING;

	if ( policy == SCROLLBARS_AS_NEEDED )
		checkScrollbarVisibility( cd);
		
	int vw = ((vScroll.flags & IS_VISIBLE) != 0) ? vScroll.preferredSize().width : 0;
	int hh = ((hScroll.flags & IS_VISIBLE) != 0) ? hScroll.preferredSize().height : 0;

	w = width - (insets.left + insets.right + vw);
	h = height - (insets.top + insets.bottom + hh);

	hScroll.setBounds( insets.left, h, w, hh);
	vScroll.setBounds( w, insets.top, vw,  h);

	// check scroll position bounds
	if ( w + pos.x > cd.width ) {
		pos.x = Math.max( cd.width - w, 0);
	}
	if (  h + pos.y > cd.height ) {
		pos.y = Math.max( cd.height - h, 0);
	}

	hScroll.setValues( pos.x, w, 0, cd.width, false);
	vScroll.setValues( pos.y, h, 0, cd.height, false);

	wrapper.setBounds( insets.left, insets.top, w , h);

	if ( cd.width < wrapper.width ) {
		cd.width = wrapper.width;
	}

	if ( cd.height < wrapper.height ) {
		child.setBounds(
			-pos.x, 
			(wrapper.height - cd.height) / 2, 
			cd.width, cd.height);
	}
	else {
		child.setBounds( -pos.x, -pos.y, cd.width, cd.height);
	}

	flags &= ~IS_LAYOUTING;
}

public void layoutX() {
	if ( child == null ) {
		return;
	}
	
	Dimension cd = child.getPreferredSize();
	int w, h;
	pos.x = pos.y = 0;
	
	flags |= IS_LAYOUTING;

	if ( policy == SCROLLBARS_AS_NEEDED )
		checkScrollbarVisibility( cd);

	int vw = ((vScroll.flags & IS_VISIBLE) != 0) ? 
		vScroll.preferredSize().width : 0;
	int hh = ((hScroll.flags & IS_VISIBLE) != 0) ? 
		hScroll.preferredSize().height : 0;

	w = width - (insets.left + insets.right + vw);
	h = height - (insets.top + insets.bottom + hh);

	hScroll.setBounds( insets.left, h, w, hh);
	vScroll.setBounds( w, insets.top, vw,  h);

	hScroll.setValues( 0, w, 0, cd.width, false);
	vScroll.setValues( 0, h, 0, cd.height, false);

	wrapper.setBounds( insets.left, insets.top, w , h);

	if ( cd.width < wrapper.width ) {
		cd.width = wrapper.width;
	}

	if ( cd.height < wrapper.height ) {
		child.setBounds( 0, (wrapper.height-cd.height)/2, cd.width, cd.height);
	}
	else {
		child.setBounds( 0, 0, cd.width, cd.height);
	}

	flags &= ~IS_LAYOUTING;
}

public void paint ( Graphics g ) {
	if ( wrapper != null )
		g.paintChild( wrapper, false);
	if ( ((hScroll.flags & IS_VISIBLE) != 0) )
		g.paintChild( hScroll, false);
	if (((vScroll.flags & IS_VISIBLE) != 0) )
		g.paintChild( vScroll, false);
}

public void remove ( int idx ) {
	if (children[idx] == wrapper) {
		wrapper.remove(child);
	}
 }

final public void setLayout ( LayoutManager mgr ) {
	throw new AWTError( "ScrollPane can't have LayoutManagers");
}

/**
  * Sets the scroll position to the specified value.
  *
  * @param x The new X coordinate of the scroll position.
  * @param y The new Y coordinate of the scroll position.
  *
  * @exception IllegalArgumentException If the specified value is outside
  * the legal scrolling range.
  */
public void setScrollPosition ( Point p ) {
	Adjustable h = getHAdjustable();
	Adjustable v = getVAdjustable();

	if (h != null)
		h.setValue(x);
	if (v != null)
		v.setValue(y);
	setScrollPosition(x, y);
}

public void setScrollPosition ( int x, int y ) {
	// pos updated by adjustmentValueChanged ( incl.range check )
	hScroll.setValue( x);
	vScroll.setValue( y);
}
}

