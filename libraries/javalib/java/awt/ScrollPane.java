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

package java.awt;

import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;

public class ScrollPane
  extends Container
  implements AdjustmentListener, FocusListener
{
	final static int SCROLLBARS_AS_NEEDED = 0;
	final static int SCROLLBARS_ALWAYS = 1;
	final static int SCROLLBARS_NEVER = 2;
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
	
	insets = new Insets( bw, bw, bw, bw);
	
	if ( policy == SCROLLBARS_ALWAYS ) {
		insets.right  += Scrollbar.SCROLLBAR_WIDTH;
		insets.bottom += Scrollbar.SCROLLBAR_WIDTH;
	}
	else {
		hScroll.isVisible = false;
		vScroll.isVisible = false;
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
		
		child.addFocusListener( this);
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
	int      w = width - (insets.left + insets.right);
	int      h = height - (insets.top + insets.bottom);
	int      delta;
	NativeGraphics g = NativeGraphics.getClippedGraphics( null, this,
	                                                      0, 0,
	                                                      insets.left, insets.top,
														                            w, h, false);
	if ( src == vScroll){
		delta = val - pos.y;
		pos.y = val;
	}
	else {
		delta = val - pos.x;
		pos.x = val;
	}

	child.isVisible = false;
	child.setLocation( insets.left -pos.x, insets.top -pos.y);
	child.isVisible = true;

	if ( g != null ) {
		if ( src == vScroll ) {
			if ( delta > 0 ) {  // scroll downwards
				g.copyArea( insets.left, insets.top + delta, w, h - delta, 0, -delta);
				g.setClip( insets.left, insets.top + h-delta, w, delta);
			}
			else {              // scroll upwards
				g.copyArea( insets.left, insets.top, w, h+delta, 0, -delta);
				g.setClip( insets.left, insets.top, w, -delta);
			}
		}
		else {
			if ( delta > 0 ) {  // scroll right
				g.copyArea( insets.left +delta, insets.top, w-delta, h, -delta, 0);
				g.setClip( insets.left + w-delta, insets.top, delta, h);
			}
			else {              // scroll left
				g.copyArea( insets.left, insets.top, w+delta, h, -delta, 0);
				g.setClip( insets.left, insets.top, -delta, h);
			}
		}

		g.translate( child.x, child.y);
		child.paint( g);			
		g.dispose();
	}
}

void checkScrollbarVisibility ( Dimension cd ) {
	int      bw = BORDER_WIDTH;
	int      w = width - 2*bw;
	int      h = height -2*bw;

	if ( hScroll.isVisible ) {
		if ( cd.width <= w ) {
			hScroll.setVisible( false);
			insets.bottom -= Scrollbar.SCROLLBAR_WIDTH;
		}
	}
	else {
		if ( cd.width > w ){
			hScroll.setVisible( true);
			insets.bottom += Scrollbar.SCROLLBAR_WIDTH;
		}
	}
	
	if ( vScroll.isVisible ) {
		if ( cd.height <= h ) {
			vScroll.setVisible( false);
			insets.right -= Scrollbar.SCROLLBAR_WIDTH;
		}
	}
	else {
		if ( cd.height > h ){
			vScroll.setVisible( true);
			insets.right += Scrollbar.SCROLLBAR_WIDTH;
		}
	}
}

public void doLayout () {
	layout();
}

public void focusGained ( FocusEvent e ) {
	if ( hScroll.isVisible )
		hScroll.setBackground( Defaults.FocusClr);
	if ( vScroll.isVisible )
		vScroll.setBackground( Defaults.FocusClr);
}

public void focusLost ( FocusEvent e ) {
	if ( hScroll.isVisible )
		hScroll.setBackground( Defaults.BtnClr);
	if ( vScroll.isVisible )
		vScroll.setBackground( Defaults.BtnClr);
}

public Adjustable getHAdjustable () {
	return hScroll;
}

public int getHSrollbarHeight () {
	return (hScroll == null) ? 0 : Scrollbar.SCROLLBAR_WIDTH;
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
	return (vScroll == null) ? 0 : Scrollbar.SCROLLBAR_WIDTH;
}

public Dimension getViewportSize () {
	return new Dimension( width - (insets.left + insets.right),
	                      height - (insets.top + insets.bottom));
}

/**
 * @deprecated
 */
public void layout() {
	if ( child == null ) {
		return;
	}

	int sw = Scrollbar.SCROLLBAR_WIDTH;
	Dimension cd = child.getPreferredSize();
	int w, h;
	
	isLayouting = true;	

	if ( policy == SCROLLBARS_AS_NEEDED )
		checkScrollbarVisibility( cd);
		
	w = width - (insets.left + insets.right);
	h = height - (insets.top + insets.bottom);
	
	hScroll.setBounds( 0, height - sw,
	                   (vScroll.isVisible) ? width - sw : width, sw);
	
	vScroll.setBounds( width - sw, 0,
	                   sw, (hScroll.isVisible) ? height - sw : height);

	hScroll.setValues( 0, w, 0, cd.width - w);
	vScroll.setValues( 0, h, 0, cd.height -h);

	wrapper.setBounds( insets.left, insets.top, w, h);
	child.setBounds( 0, 0, cd.width, cd.height);

	isLayouting = false;
}

public void paint ( Graphics g ) {
	Rectangle clip = g.getClipBounds();
	int       xClip = clip.x;
	int       yClip = clip.y;
	int       xwClip = xClip + clip.width;
	int       yhClip = yClip + clip.height;
	boolean   paintHS = (xwClip > width - insets.right);
	boolean   paintVS = (yhClip > height - insets.bottom);

	if ( (xClip < insets.left) || (yClip < insets.top) || paintHS || paintVS ) {
		paintBorder( g, 0, 0,
		            (vScroll.isVisible) ? Scrollbar.SCROLLBAR_WIDTH : 0,
		            (hScroll.isVisible) ? Scrollbar.SCROLLBAR_WIDTH : 0);
		
		if ( hScroll.isVisible && paintVS ) {
			g.translate( hScroll.x, hScroll.y);
			hScroll.paint( g);
			g.translate( -hScroll.x, -hScroll.y);
		}
		if ( vScroll.isVisible && paintVS ) {
			g.translate( vScroll.x, vScroll.y);
			vScroll.paint( g);
			g.translate( -vScroll.x, -vScroll.y);
		}
	}
	
	if ( child != null ) {
		g.translate( insets.left, insets.top);
		g.setClip( 0, 0,
		           width - (insets.left+insets.right),
		           height - (insets.top + insets.bottom));

		child.paint( g);
	}
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
