package java.awt;

import java.awt.AWTError;
import java.awt.Adjustable;
import java.awt.Component;
import java.awt.Container;
import java.awt.Dimension;
import java.awt.Graphics;
import java.awt.LayoutManager;
import java.awt.Point;
import java.awt.Scrollbar;
import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;

/**
 * ScrollPane - widget for scrolling a child component
 *
 * Copyright (c) 1998
 *    Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P.C.Mehlitz
 */
public class ScrollPane
  extends Container
  implements AdjustmentListener
{
	final public static int SCROLLBARS_AS_NEEDED = 0;
	final public static int SCROLLBARS_ALWAYS = 1;
	final public static int SCROLLBARS_NEVER = 2;
	int policy;
	SPAdjustable hScroll;
	SPAdjustable vScroll;
	Point pos;
	Component child;
	ChildWrapper wrapper;

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
	layoutm = null;
}

public void doLayout () {
}

public void paint( Graphics g) {
	g.paintChild( child, false);
}
}

public ScrollPane () {
	this( SCROLLBARS_AS_NEEDED);
}

public ScrollPane ( int policy ) {
	int bw = BORDER_WIDTH;
	layoutm = null;

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
	if ( (child != hScroll) && (child != vScroll) ){
		if ( this.child != null )
			remove( this.child);
		
		this.child = child;
		wrapper = new ChildWrapper();
		wrapper.add( child);
		super.addImpl( wrapper, constraint, idx);
	}
	else {
		super.addImpl( child, constraint, idx);
	}
}

public void adjustmentValueChanged ( AdjustmentEvent e) {
	if ( child == null )
		return;

	Object   src = e.getSource();
	int      val = e.getValue();
	int      w = wrapper.width;
	int      h = wrapper.height;
	int      delta;
	
	if ( src == vScroll){
		delta = val - pos.y;
		pos.y = val;
	}
	else {
		delta = val - pos.x;
		pos.x = val;
	}

	// fake visibility while repositioning child
	child.flags &= ~IS_VISIBLE;
	child.setLocation( -pos.x, -pos.y);
	child.flags |= IS_VISIBLE;

	Graphics g = wrapper.getGraphics();

	if ( g != null ) {
		if ( src == vScroll ) {
			if ( delta > 0 ) {  // scroll downwards
				g.copyArea( 0, delta, w, h - delta, 0, -delta);
				g.setClip( 0, h-delta, w, delta);
			}
			else {              // scroll upwards
				g.copyArea( 0, 0, w, h+delta, 0, -delta);
				g.setClip( 0, 0, w, -delta);
			}
		}
		else {
			if ( delta > 0 ) {  // scroll right
				g.copyArea( delta, 0, w-delta, h, -delta, 0);
				g.setClip( w-delta, 0, delta, h);
			}
			else {              // scroll left
				g.copyArea( 0, 0, w+delta, h, -delta, 0);
				g.setClip( 0, 0, -delta, h);
			}
		}

		wrapper.update( g);

		g.dispose();
	}
}

void checkScrollbarVisibility ( Dimension cd ) {
	int      bw = BORDER_WIDTH;
	int      w = width - 2*bw;
	int      h = height -2*bw;

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

public Adjustable getHAdjustable () {
	return hScroll;
}

public int getHSrollbarHeight () {
	return (hScroll == null) ? 0 : Defaults.ScrollbarWidth;
}

public Point getScrollPosition () {
	return pos;
}

public int getScrollbarDisplayPolicy () {
	return policy;
}

public Adjustable getVAdjustable () {
	return vScroll;
}

public int getVScrollbarWidth () {
	return (vScroll == null) ? 0 : Defaults.ScrollbarWidth;
}

public Dimension getViewportSize () {
	return new Dimension( width - (insets.left + insets.right),
	                      height - (insets.top + insets.bottom));
}

public boolean isFocusTraversable () {
	return false;
}

/**
 * @deprecated
 */
public void layout() {
	if ( child == null ) {
		return;
	}
	int bw = BORDER_WIDTH;
	int sw = Defaults.ScrollbarWidth;
	Dimension cd = child.getPreferredSize();
	int w, h;

	flags |= IS_LAYOUTING;

	if ( policy == SCROLLBARS_AS_NEEDED )
		checkScrollbarVisibility( cd);

	int vw = ((vScroll.flags & IS_VISIBLE) != 0) ? sw : 0;
	int hh = ((hScroll.flags & IS_VISIBLE) != 0) ? sw : 0;

	w = width - (2*bw + vw);
	h = height - (2*bw + hh);
	
	hScroll.setBounds( 0, height - sw, width - vw, sw);
	vScroll.setBounds( width - sw, 0, sw, height - hh);

	pos.x = ( hh == 0 ) ? 0 : hScroll.getValue();
	pos.y = ( vw == 0 ) ? 0 : vScroll.getValue();
	hScroll.setValues( pos.x, w, 0, cd.width);
	vScroll.setValues( pos.y, h, 0, cd.height);
	
	wrapper.setBounds( bw, bw, width - vw -2*bw , height - hh -2*bw);
	child.setBounds( -pos.x, -pos.y, cd.width, cd.height);

	flags &= ~IS_LAYOUTING;
}

public void paint ( Graphics g ) {
	int sw = Defaults.ScrollbarWidth;
	int vw = ((vScroll.flags & IS_VISIBLE) != 0) ? sw : 0;
	int hh = ((hScroll.flags & IS_VISIBLE) != 0) ? sw : 0;

	if ( wrapper != null )
		g.paintChild( wrapper, false);
	if ( hh != 0 )
		g.paintChild( hScroll, false);
	if ( vw != 0 )
		g.paintChild( vScroll, false);
		
	kaffePaintBorder( g, 0, 0, vw, hh);
}

final public void setLayout ( LayoutManager mgr ) {
	throw new AWTError( "ScrollPane can't have LayoutManagers");
}

public void setScrollPosition ( Point p ) {
	setScrollPosition( p.x, p.y);
}

public void setScrollPosition ( int x, int y ) {
	pos.x = x;
	pos.y = y;
	
	hScroll.setValue( x);
	vScroll.setValue( y);
}
}
