package java.awt;

import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.awt.event.FocusEvent;
import java.awt.event.FocusListener;
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
	Graphics rgr;

public void addNotify() {
	super.addNotify();
	if ( rgr == null ) {
		rgr = getGraphics();
		updateClip( rgr);
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
	paintBorder();
	
	if ( Defaults.FocusScrolls ) {
		if ( hScroll != null )
			hScroll.setBackground( Defaults.FocusClr);
		if ( vScroll != null )
			vScroll.setBackground( Defaults.FocusClr);	
	}
}

public void focusLost( FocusEvent e) {
	paintBorder();
	
	if ( Defaults.FocusScrolls ) {
		if ( hScroll != null )
			hScroll.setBackground( Defaults.BtnClr);
		if ( vScroll != null )
			vScroll.setBackground( Defaults.BtnClr);	
	}
}

public Graphics getGraphics() {
	Graphics g = super.getGraphics();
	if ( g != null ) {
		g.setTarget( this);
	}
	return g;
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
	int sbd = Scrollbar.SCROLLBAR_WIDTH;

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
	
	updateVScroll();
	
	if ( initVis > -1 ) {
		makeVisible( initVis);
		initVis = -1;
	}
}

void makeVisible ( int row) {
  int mv = getVisibleRows();
	int rs = rows.size();
	int my = rs - mv;

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

public void removeNotify() {
	super.removeNotify();
	if ( rgr != null ) {
		rgr.dispose();
		rgr = null;
	}
}

void repaintRow( int idx) {
}

int repaintRows( int sIdx, int len) {
	int rs = rows.size();
	int d = BORDER_WIDTH;
	int y = d;
	
	for ( int i=first; i<rs; i++) {
		if ( (i >= sIdx) && (i <= sIdx + len) )
			repaintRow( i);
		y += rowHeight;
		if ( y > height)
			break;
	}
	
	if (y < height) {
		rgr.setColor( getBackground()  );
		rgr.fillRect( d, y, width- 2*d , height - y -d);
	}
	
	return y;

}

public void reshape( int x, int y, int w, int h) {
	super.reshape( x, y, w, h);
	updateClip( rgr);
}

void setListeners() {
	if ( hScroll != null )
		hScroll.addAdjustmentListener( this);
	if ( vScroll != null )
		vScroll.addAdjustmentListener( this);
	addFocusListener( this);
}

void shiftVertical( int rows, boolean updScroll) {
	if ( rows == 0 )
		return;

	if ( updScroll && vScroll != null) {
		vScroll.setValue( first);
	}

	int d = BORDER_WIDTH;

	// hjb, 01-28-1999: only move the area that will fall in
	// pane after the copy.
	// otherwise text is displayed above or under the pane
	// obscuring the scrollbar and/or other fields
	
	if (rows < 0)
	    rgr.copyArea( d, d + rows * rowHeight, width- 2*d, height- 2*d,
			  0, rows * rowHeight );
	else
	    rgr.copyArea( d, d, width- 2*d, height- 2*d - rows * rowHeight,
			  0, rows * rowHeight );
	    
	if ( rows < 0 ){
		int sIdx = first + getVisibleRows() + rows;
		int len = -rows + 1;
		repaintRows( sIdx, len ); 
	}
	else {
		repaintRows( first, rows-1);
	}

}

void updateClip( Graphics g) {
	if ( g != null ) {
		g.setClip( BORDER_WIDTH, BORDER_WIDTH,
							 width-2*BORDER_WIDTH, height-2*BORDER_WIDTH);
	}
}

void updateVScroll() {
	boolean v1 = false, v2 = false;
	
	if ( vScroll != null ) {
		v1 = vScroll.isSliderShowing();
		int vr = getVisibleRows();
		vScroll.setValues( first, vr, 0, rows.size() - vr);
		v2 = vScroll.isSliderShowing();
	}
	
	if (v1 != v2 ){
		Graphics g = parent.getGraphics();
		if ( g != null ) {
			g.setColor( parent.getBackground() );
			g.fillRect( 0, 0, parent.width, parent.height);
			g.dispose();
		}
		innerLayout();
	}
}

void vPosChange( int steps) {
}
}
