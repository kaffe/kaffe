package java.awt;

import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
import java.awt.event.KeyEvent;
import java.awt.event.MouseEvent;
import java.util.Vector;

/**
 * class RowCanvas -
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
class RowCanvas
  extends Component
  implements AdjustmentListener, FocusListener
{
	Scrollbar hScroll;
	Scrollbar vScroll;
	int rowHeight;
	Vector rows = new Vector();
	int first;
	int xOffs;
	int xOffsInit;
	FontMetrics fm;
	int initVis = -1;
	NativeGraphics rgr;

public void addNotify() {
	super.addNotify();
	
	if ( rgr == null ) {
		setResGraphics();
	}
}

public void adjustmentValueChanged( AdjustmentEvent e) {
	Object src = e.getSource();
	
	if ( src == vScroll){
		int val = e.getValue();
		int lf = first;
		if ( val != first){
			first = val;
			shiftVertical( lf - val, false );
			vPosChange( lf - val);
		}
	}
	else if ( src == hScroll) {
		int val = -e.getValue();
		if ( val != xOffs + xOffsInit) {
			xOffs = val + xOffsInit;
			hPosChange();
		}
	}

}

public void focusGained( FocusEvent e) {
	kaffePaintBorder();
	
	if ( Defaults.FocusScrolls ) {
		if ( hScroll != null )
			hScroll.setBackground( Defaults.FocusClr);
		if ( vScroll != null )
			vScroll.setBackground( Defaults.FocusClr);	
	}
	
	redirectFocusEvent( e);
}

public void focusLost( FocusEvent e) {
	kaffePaintBorder();
	
	if ( Defaults.FocusScrolls ) {
		if ( hScroll != null )
			hScroll.setBackground( Defaults.BtnClr);
		if ( vScroll != null )
			vScroll.setBackground( Defaults.BtnClr);	
	}
	
	redirectFocusEvent( e);
}

int getRowIdx( int yPos) {
	int i, y = BORDER_WIDTH;
	
	if ( yPos < BORDER_WIDTH ) {
		for ( i = first-1; i>=0; i--) {
			y -= rowHeight;
			if ( y < yPos )
				return i;
		}
		return 0;
	}
	
	int rs = rows.size();
	for ( i = first; i<rs-1; i++ ){
		y += rowHeight;
		if ( y > yPos )
			break;
	}
	
	return i;
}

int getRowYPos( int row ) {
	return BORDER_WIDTH + ( row - first ) * rowHeight;
}

int getVisibleRows() {
	return (height - 2 * BORDER_WIDTH) / rowHeight;
}

void hPosChange() {
	repaint();
}

void innerLayout() {
	int w = parent.width;
	int h = parent.height;
	int sbd = Defaults.ScrollbarWidth;

	boolean vVis = (vScroll != null ) && vScroll.isSliderShowing();
	boolean hVis = (hScroll != null ) && hScroll.isSliderShowing();

	setBounds( 0, 0, vVis ? w-sbd : w, hVis ? h-sbd : h);

	if ( vScroll != null ) {
		if ( vScroll.isVisible() && ! vVis )
			vScroll.setVisible( false);
		else if ( !vScroll.isVisible() && vVis )
			vScroll.setVisible( true);
		vScroll.setBounds( w-sbd, 0, sbd, hVis ? h-sbd : h);
	}
	if ( hScroll != null) {
		if ( hScroll.isVisible() && ! hVis )
			hScroll.setVisible( false);
		else if ( !hScroll.isVisible() && hVis )
			hScroll.setVisible( true);
		hScroll.setBounds( 0, h-sbd, vVis ? w-sbd : w, sbd);
	}
	
	updateScrolls();
//	updateVScroll();
//	updateHScroll();
	
	if ( initVis > -1 ) {
		makeVisible( initVis);
		initVis = -1;
	}
}

void makeVisible ( int row) {
  int mv = getVisibleRows();
	int rs = rows.size();

	// we need bounds to calculate visible range
	// defer until next layout
	if ( height == 0 ) {
		initVis = row;
		return;
	}

	// row already visible	
	if ( (row >= first) && (row < first + mv) )
		return;
		
	if ( row < 0 )
		row = 0;
	else if ( row >= rs )
		row = rs - 1;
		
	int lf = first;
  if ( row < first ){
		first = row;
		shiftVertical( lf - first, true);
	}
	else if ( row > first + mv - 1){
		first = row - mv + 1;
		shiftVertical( lf - first, true);
	}
}

int maxRowWidth() {
	return 0;
}

void kaffePaintBorder () {
  // not nice to resolve this, but our getGraphics() is more expensive

	Graphics g = super.getGraphics();
	if ( g != null ){
		kaffePaintBorder( g);
		g.dispose();
	}
}

void propagateReshape () {
	if ( rgr != null ){
		updateLinkedGraphics();
		
		// we have our own clipping, which we have to take care of
		rgr.setClip( BORDER_WIDTH, BORDER_WIDTH,
		             width - 2*BORDER_WIDTH, height - 2*BORDER_WIDTH);
	}
}

void rearrange() {
	Graphics g = parent.getGraphics();
	if ( g != null ) {
		g.setColor( parent.bgClr );
		g.fillRect( 0, 0, parent.width, parent.height);
		g.dispose();
	}
	innerLayout();
}

void redirectFocusEvent( FocusEvent e) {
	if ( parent.focusListener != null ){
		AWTEvent.setSource( e, parent);
		parent.process( e);
	}
}

void redirectKeyEvent( KeyEvent e) {
	if ( parent.keyListener != null ){
		AWTEvent.setSource( e, parent);
		parent.process( e);
	}
}

void redirectMotionEvent( MouseEvent e) {
	if ( parent.motionListener != null ){
		e.retarget( parent, x, y);
		parent.process( e);

		// be careful, the event might be used subsequently
		e.retarget( this, -x, -y);
	}
}

void redirectMouseEvent( MouseEvent e) {
	if ( parent.mouseListener != null ){
		e.retarget( parent, x, y);
		parent.process( e);
		
		// be careful, the event might be used subsequently
		e.retarget( this, -x, -y);
	}
}

public void removeNotify() {
	super.removeNotify();

	if ( rgr != null ) {	
		rgr.dispose();
		rgr = null;
	}
}

void repaintRow( Graphics g, int idx) {
}

void repaintRow( int idx) {
	if ( rgr != null )
		repaintRow( rgr, idx);
}

int repaintRows( Graphics g, int sIdx, int len) {
	if ( g == null )
		return 0;
	int rs = rows.size();
	int d = BORDER_WIDTH;
	int y = d;
	
	for ( int i=first; i<rs; i++) {
		if ( (i >= sIdx) && (i <= sIdx + len) )
			repaintRow( g, i);
		y += rowHeight;
		if ( y > height)
			break;
	}
	
	if (y < height) {
		g.setColor( getBackground()  );
		g.fillRect( d, y, width- 2*d , height - y -d);
	}
	
	return y;
}

int repaintRows( int sIdx, int len) {
	if ( rgr != null )
		return repaintRows( rgr, sIdx, len);
	else
		return 0;
}

void setListeners() {
	if ( hScroll != null )
		hScroll.addAdjustmentListener( this);
	if ( vScroll != null )
		vScroll.addAdjustmentListener( this);
	addFocusListener( this);
}

void setResGraphics () {
	rgr = NativeGraphics.getClippedGraphics( null, this, 0, 0,
                                           BORDER_WIDTH, BORDER_WIDTH,
		                                       width - 2*BORDER_WIDTH, height - 2*BORDER_WIDTH, false);
	if ( rgr != null )
		linkGraphics( rgr);
}

void shiftVertical ( int rows, boolean updScroll) {
	if ( (rows == 0) || (rgr == null) )
		return;

	// PM - FIX THIS. In case we are not called from adjustmentNotify (i.e. the
	// EventDispatchThread), where it is obsolete anyway, this might cause reverse scrolling
	// (see Scrollbar.setValue). We currently solve this by fiddling with the inner
	// workings of Scrollbar.
	if ( updScroll && vScroll != null) {
		vScroll.setValues( first, vScroll.vis, vScroll.min, vScroll.max, false);
	}

	int d = BORDER_WIDTH;
	int d2 = d*2;
	int dy = rows * rowHeight;
	int vr = getVisibleRows();

	if ( rows < 0 ){
		if ( -rows < vr )  // otherwise the new top wasn't visible
			rgr.copyArea( d, d-dy, width -d2, height-d2+dy-1, 0, dy);

		repaintRows( first + vr + rows, 1 - rows );
	}
	else {
		if ( rows < vr )   // otherwise the old top will not be visible anymore
			rgr.copyArea( d, d, width -d2, height-d2-dy-1, 0, dy);

		repaintRows( first, rows-1);
	}

}

public void update ( Graphics g ){
	// we don't need background blanking
	paint( g);
}

boolean updateHScroll() {
	if ( hScroll == null )
		return false;

	boolean	v1 = hScroll.isSliderShowing();
	int mw = maxRowWidth() + 2*(BORDER_WIDTH+1);
	int cv = hScroll.getValue();
	
	// entire contents going to be visible ( reset to left )
	if ( v1 && (mw <= width) )
		cv = 0;
		
	// notify only on left reset
	hScroll.setValues( cv, width, 0, mw, (v1 && (cv==0)) );
	
	return ( v1 != hScroll.isSliderShowing() );
}

void updateScrolls() {
	boolean dv1 = updateVScroll();
	boolean dv2 = updateHScroll();
	
	if ( dv1 || dv2 ) {
		rearrange();
	}
}

boolean updateVScroll() {
	if ( vScroll == null )
		return false;
		
	boolean	v1 = vScroll.isSliderShowing();
	int rs = rows.size();
	int vr = getVisibleRows();

	//entire contents going to be visible ( reset to top )
	if ( v1 && (vr >= rs) )
		first = 0;
		
	//no need to notify ( first already adjusted )
	vScroll.setValues( first, vr, 0, rs, false );
	
	return ( v1 != vScroll.isSliderShowing() );
}

void vPosChange( int steps) {
}
}
