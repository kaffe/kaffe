package java.awt;

import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import java.awt.event.MouseMotionListener;

import kaffe.util.Timer;
import kaffe.util.TimerClient;

/**
 * class Scrollbar - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class Scrollbar
  extends Component
  implements Adjustable, MouseListener, MouseMotionListener, TimerClient
{
	final private static long serialVersionUID = 8451667562882310543L;
	final public static int HORIZONTAL = 0;
	final public static int VERTICAL = 1;
	int min;
	int max;
	int vis;
	int val;
	int ori;
	AdjustmentListener aListener;
	int blockInc;
	int unitInc = 1;
	Rectangle slRect = new Rectangle();
	Graphics dragGr;
	Rectangle dragRect = new Rectangle();
	static int currentOp;
	static Point dragOffs = new Point();

public Scrollbar() {
	this( VERTICAL);
}

public Scrollbar( int ori) throws IllegalArgumentException {
	this( ori, 0, 30, 0, 100);
}

public Scrollbar( int ori, int value, int visible, int min, int max) throws IllegalArgumentException{
	setBackground( Defaults.BtnClr);
	setOrientation( ori);
	setValues( value, visible, min, max);

	addMouseListener( this);
	addMouseMotionListener( this);
}

public synchronized void addAdjustmentListener( AdjustmentListener al) {
	aListener = AWTEventMulticaster.add( aListener, al);
}

public int getBlockIncrement() {
	return (getPageIncrement());
}

ClassProperties getClassProperties () {
	return ClassAnalyzer.analyzeAll( getClass(), true);
}

int getCurrentOp ( int x, int y) {
	if ( slRect.contains( x, y) ) {
		dragOffs.x = x - slRect.x;
		dragOffs.y = y - slRect.y;
		return AdjustmentEvent.TRACK;
	}

	switch( ori) {
		case VERTICAL:
			if ( y > height - width )
				return AdjustmentEvent.UNIT_INCREMENT;
			else if ( y < width )
				return AdjustmentEvent.UNIT_DECREMENT;
			else if ( y < slRect.y )
				return AdjustmentEvent.BLOCK_DECREMENT;
			return AdjustmentEvent.BLOCK_INCREMENT;
		case HORIZONTAL:
			if ( x > width - height )
				return AdjustmentEvent.UNIT_INCREMENT;
			else if ( x < height )
				return AdjustmentEvent.UNIT_DECREMENT;
			else if ( x < slRect.x )
				return AdjustmentEvent.BLOCK_DECREMENT;
			return AdjustmentEvent.BLOCK_INCREMENT;
	}

	return 0;
}

/**
 * @deprecated
 */
public int getLineIncrement() {
	return unitInc;
}

public int getMaximum() {
	return max;
}

public int getMinimum() {
	return min;
}

public int getOrientation() {
	return ori;
}

/**
 * @deprecated
 */
public int getPageIncrement() {
	return blockInc;
}

public int getUnitIncrement() {
	return (getLineIncrement());
}

public int getValue() {
	return val;
}

/**
 * @deprecated
 */
public int getVisible() {
	return vis;
}

public int getVisibleAmount() {
	return (getVisible());
}

public boolean isFocusTraversable() {
	return false;
}

boolean isSliderShowing() {
	return slRect.width > 0;
}

public void mouseClicked( MouseEvent e) {
	currentOp = getCurrentOp( e.getX(), e.getY() );
	switch ( currentOp) {
		case AdjustmentEvent.BLOCK_DECREMENT:
			setValue( val - blockInc);
			break;
		case AdjustmentEvent.BLOCK_INCREMENT:
			setValue( val + blockInc);
			break;
	}
}

public void mouseDragged( MouseEvent e) {
	if ( (currentOp != AdjustmentEvent.TRACK) || (dragGr == null) )
		return;

	int dx, dy;
	Color cp = parent.bgClr;

	if ( ori == VERTICAL ) {
		int y0 = e.getY() - dragOffs.y;
		int maxY = height - slRect.height;
		if ( y0 <= 0) {
			if ( slRect.y > 0 ){
				slRect.y = 0;
				dy = dragRect.y;
				if ( dy > 0 ) {
					dragGr.setColor( cp);
					dragGr.fillRect( slRect.x, slRect.height, slRect.width, dy);
				}
				paint( dragGr);
			}
			if ( val > min) {
				val = min;
				notifyAdjust();
			}
		}
		else if ( y0 >= maxY) {
			if ( slRect.y < maxY) {
				slRect.y = maxY;
				dy = slRect.y - dragRect.y;
				if ( dy > 0 ) {
					dragGr.setColor( cp);
					dragGr.fillRect( slRect.x, dragRect.y, slRect.width, dy);
				}
				paint( dragGr);
			}
			if ( val < max - vis) {
				val = max - vis;
				notifyAdjust();
			}
		}
		else {
			slRect.y = y0;
			dy = slRect.y - dragRect.y;
			dragGr.setColor( cp);
			if ( dy > 0 )
				dragGr.fillRect( slRect.x, dragRect.y, slRect.width, dy);
			else if ( dy < 0 )
				dragGr.fillRect( slRect.x, slRect.y + slRect.height, slRect.width, -dy);
			paint( dragGr);
			updateValue();
		}
		
		dragRect.y = slRect.y;
	}
	else {
		int x0 = e.getX() - dragOffs.x;
		int maxX = width - slRect.width;
		if ( x0 <= 0) {
			if ( slRect.x >= 0 ) {
				slRect.x = 0;
				dx = dragRect.x;
				if ( dx > 0 ) {
					dragGr.setColor( cp);
					dragGr.fillRect( slRect.width, slRect.y, dx, slRect.height);
				}
				paint( dragGr);
			}
			if ( val > min) {
				val = min;
				notifyAdjust();
			}
		}
		else if ( x0 >= maxX) {
			if ( slRect.x < maxX ) {
				slRect.x = maxX;
				dx = slRect.x - dragRect.x;
				if ( dx > 0 ) {
					dragGr.setColor( cp);
					dragGr.fillRect( dragRect.x, slRect.y, dx, slRect.height);
				}
				paint( dragGr);
			}					
			if ( val < max - vis) {
				val = max - vis;
				notifyAdjust();
			}
		}
		else {
			slRect.x = x0;
			dx = slRect.x - dragRect.x;
			dragGr.setColor( cp);
			if ( dx > 0 )
				dragGr.fillRect( dragRect.x, slRect.y, dx, slRect.height);
			else if ( dx < 0 )
				dragGr.fillRect( slRect.x + slRect.width, slRect.y, -dx, slRect.height);
			paint( dragGr);
			updateValue();
		}
		
		dragRect.x = slRect.x;
	}
}

public void mouseEntered( MouseEvent e) {
}

public void mouseExited( MouseEvent e) {
}

public void mouseMoved( MouseEvent e) {
}

public void mousePressed( MouseEvent e) {
	currentOp = getCurrentOp( e.getX(), e.getY() );
	
	if ( (dragGr = getGraphics()) != null )
		dragGr.setColor( bgClr);
	
	dragRect.x = slRect.x;
	dragRect.y = slRect.y;
	dragRect.width = slRect.width;
	dragRect.height = slRect.height;
	
	switch ( currentOp) {
		case AdjustmentEvent.UNIT_DECREMENT:
		case AdjustmentEvent.UNIT_INCREMENT:
			timerExpired( null);
			Timer.getDefaultTimer().addClient( this, 300, 50);
			break;
		default:
	}
					
}

public void mouseReleased( MouseEvent e) {
	Timer.getDefaultTimer().removeClient( this);
	dragOffs.x = 0;
	dragOffs.y = 0;

	if ( dragGr != null ){
		dragGr.dispose();
		dragGr = null;
	}

	switch( currentOp) {
		case AdjustmentEvent.UNIT_INCREMENT:
		case AdjustmentEvent.UNIT_DECREMENT:
			currentOp = 0;
	}
}

void notifyAdjust() {
	if ( (aListener != null) ||
	     (eventMask & AWTEvent.ADJUSTMENT_EVENT_MASK) != 0 ||
	     ((flags & IS_OLD_EVENT) != 0) ) {
		Toolkit.eventQueue.postEvent( AdjustmentEvt.getEvent( this,
		                                                      AdjustmentEvent.ADJUSTMENT_VALUE_CHANGED,
		                                                      currentOp, val));
	}
}

public void paint ( Graphics g ) {
	int x, y, xw, yh;

	if ( slRect.width == 0 ) {
		g.setColor( parent.bgClr );
		g.fillRect( 0, 0, width, height);
		return;
	}
	
	g.setColor( bgClr);

	if ( ori == HORIZONTAL) {
		y = height/2 -1;
		xw = slRect.x + slRect.width;
	
		if ( slRect.x > 0 ) {
			g.draw3DRect( 0, y, slRect.x, 1, true);
		}
		if ( xw < width ) {
			x = xw -1;
			g.draw3DRect( x, y, width-x-1, 1, true);
		}
	}
	else {
		x = width/2 -1;
		yh = slRect.y + slRect.height;

		if ( slRect.y > 0 )
			g.draw3DRect( x, 0, 1, slRect.y, true);
		if ( yh < height ) {
			y = yh -1;
			g.draw3DRect( x, y, 1, height-y-1, true);
		}
	}

	if ( slRect.width > 0) {
		g.fill3DRect( slRect.x, slRect.y, slRect.width, slRect.height, true);
	}
}

protected String paramString() {
	return super.paramString() + ",Value: " + val + ",Visible: " + vis + ",Min: " + min + ",Max: " + max; 
}

/**
 * @deprecated
 */
public Dimension preferredSize() {
	if ( ori == VERTICAL ) {
		return new Dimension( Defaults.ScrollbarWidth, 100);
	}
	else {
		return new Dimension( 100, Defaults.ScrollbarWidth);
	}
}

void process ( AdjustmentEvent e ) {
	if ( (aListener != null) || ((eventMask & AWTEvent.ADJUSTMENT_EVENT_MASK) != 0) ){
		processEvent( e);
	}
	
	if ( (flags & IS_OLD_EVENT) != 0 ) {
		postEvent( Event.getEvent( e));
	}
}

protected void processAdjustmentEvent ( AdjustmentEvent e) {
	if (  aListener != null ) {
		aListener.adjustmentValueChanged( e);
	}
}

public synchronized void removeAdjustmentListener( AdjustmentListener al) {
	aListener = AWTEventMulticaster.remove( aListener, al);
}

public void reshape( int x, int y, int width, int height) {
	super.reshape( x, y, width, height);
	updateSliderRect();
}

public synchronized void setBlockIncrement( int inc) {
	setPageIncremental(inc);
}

/**
 * @deprecated
 */
public void setLineIncrement(int inc) {
	unitInc = inc;
}

public synchronized void setMaximum( int max) {
	setValues( val, vis, min, max);
}

public synchronized void setMinimum( int min) {
	setValues( val, vis, min, max);
}

public synchronized void setOrientation( int ori) throws IllegalArgumentException {
	if ( (ori != HORIZONTAL) && (ori != VERTICAL) )
		throw new IllegalArgumentException();
	this.ori = ori;
}

/**
 * @deprecated
 */
public void setPageIncremental(int inc) {
	setValues( val, inc, min, max);
}

public void setUnitIncrement( int inc) {
	setLineIncrement(inc);
}

public synchronized void setValue( int value) {
	if ( val != value) {
		setValues( value, vis, min, max);
	}
}

public void setValues( int value, int visible, int min, int max) {
	setValues( value, visible, min, max, true);
}

synchronized void setValues( int value, int visible, int min, int max, boolean notify ) {
	// PM: not ideal - this is a workaround for clients with coexisting sync (own) and
	// async (Scrollbar initiated) updates. In case they rely on sync updates, they are
	// just interested in Scrollbar updates, but not notifications. However, this is a design
	// flaw of these clients (this is not a public API of Scrollbar !!)

	int lastVal = this.val;

	if ( (value == val) && (visible == vis) && (min == this.min) && (max == this.max) )
		return;

	// Make size adjustments.
	if (max < min)								max = min;
	if (visible > max-min)				visible = max-min;
	if ( value > max - visible )	value = max - visible;
	else if ( value < min )				value = min;
			
	this.min = min;
	this.max = max;
	this.val = value;
	this.vis = visible;
	this.blockInc = visible;
	
	update();

	if ( notify && (val != lastVal) )
		notifyAdjust();
}

public synchronized void setVisibleAmount( int vis) {
	setValues( val, vis, min, max);
}

public void timerExpired( Timer t) {
	switch ( currentOp) {
	case AdjustmentEvent.UNIT_DECREMENT:
		if ( val == min)
			Timer.getDefaultTimer().removeClient( this);
		else
			setValue( val-unitInc);
		break;
	case AdjustmentEvent.UNIT_INCREMENT:
		if ( val == max)
			Timer.getDefaultTimer().removeClient( this);
		else
			setValue( val+unitInc);
		break;
	}	
}

void update() {
	Graphics g = null;
	
	if ( dragGr != null )
		g = dragGr;
	else if ( (flags & IS_VISIBLE) != 0 )
		g = getGraphics();
	
	if ( g != null ){
		g.setColor( parent.bgClr);
		g.fillRect( slRect.x, slRect.y, slRect.width, slRect.height);

		updateSliderRect();
		
		paint( g);	
		if ( g != dragGr ) {
			g.dispose();
		}
	}
	else {
		updateSliderRect();
	}
}

public void update ( Graphics g ) {
	g.setColor( parent.bgClr );
	g.fillRect( 0, 0, width, height);
	paint( g);
}

void updateSliderRect () {
	int delta = max - min;

	//entire contents visible
	if ( delta <= 0 || vis <= 0 || max == vis) {
		slRect.setBounds( 0, 0, 0, 0);
		return;
	}

	//slider position from value
	if ( ori == HORIZONTAL ) {
		int dx = Math.max( vis * width / delta, 10);
		int x0 = (val - min) * ( width - dx ) / (max - vis);
		if ( x0 >= 0 )
			slRect.setBounds( x0, 2, dx, height-4);
		else
			slRect.setBounds( 0, 0, 0, 0);
	}
	else {
		int dy = Math.max (vis * height / delta, 10);
		int y0 = (val - min) * (height - dy) / (max - vis);
		if ( y0 >= 0 )
			slRect.setBounds( 2, y0, width-4, dy);
		else
			slRect.setBounds( 0, 0, 0, 0);
	}
}

void updateValue () {
	//value from slider position
	int newVal;
	
	if ( ori == HORIZONTAL )
		newVal = min + ( (max - vis) * slRect.x ) / (width - slRect.width);
	else {
		newVal = min + ( (max - vis) * slRect.y ) / (height - slRect.height);
	}
	if ( val != newVal ) {
		val = newVal;
		notifyAdjust();
	}
}
}
