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
	final public static int HORIZONTAL = 0;
	final public static int VERTICAL = 1;
	static int SCROLLBAR_WIDTH = 14;

public Scrollbar() {
	this( VERTICAL);
}

public Scrollbar( int ori) throws IllegalArgumentException {
	this( ori, 0, 30, 0, 100);
}

public Scrollbar( int ori, int value, int visible, int min, int max) throws IllegalArgumentException{
	bgClr = Defaults.BtnClr;
	setOrientation( ori);
	setValues( value, visible, min, max);

	addMouseListener( this);
	addMouseMotionListener( this);
}

public synchronized void addAdjustmentListener( AdjustmentListener al) {
	aListener = AWTEventMulticaster.add( aListener, al);
	eventMask |= AWTEvent.ADJUSTMENT_EVENT_MASK;
}

public int getBlockIncrement() {
	return blockInc;
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

public int getMaximum() {
	return max;
}

public int getMinimum() {
	return min;
}

public int getOrientation() {
	return ori;
}

public Dimension getPreferredSize() {
	if ( ori == VERTICAL )
		return new Dimension( SCROLLBAR_WIDTH, 100);
	else
		return new Dimension( 100, SCROLLBAR_WIDTH);
}

public int getUnitIncrement() {
	return unitInc;
}

public int getValue() {
	return val;
}

public int getVisibleAmount() {
	return vis;
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
	if ( currentOp != AdjustmentEvent.TRACK )
		return;

	int dx, dy;
	Color cp = parent.getBackground();

	if ( ori == VERTICAL ) {
		int y0 = e.getY() - dragOffs.y;
		int maxY = height - slRect.height;
		if ( y0 <= 0) {
			if ( slRect.y >= 0 ){
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
			if ( slRect.y <= maxY) {
				slRect.y = maxY;
				dy = slRect.y - dragRect.y;
				if ( dy > 0 ) {
					dragGr.setColor( cp);
					dragGr.fillRect( slRect.x, dragRect.y, slRect.width, dy);
				}
				paint( dragGr);
			}
			if ( val < max) {
				val = max;
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
			if ( slRect.x <= maxX ) {
				slRect.x = maxX;
				dx = slRect.x - dragRect.x;
				if ( dx > 0 ) {
					dragGr.setColor( cp);
					dragGr.fillRect( dragRect.x, slRect.y, dx, slRect.height);
				}
				paint( dragGr);
			}					
			if ( val < max) {
				val = max;
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
		dragGr.setColor( getBackground());
	
	dragRect.x = slRect.x;
	dragRect.y = slRect.y;
	dragRect.width = slRect.width;
	dragRect.height = slRect.height;
	
	switch ( currentOp) {
		case AdjustmentEvent.UNIT_DECREMENT:
		case AdjustmentEvent.UNIT_INCREMENT:
			timerExpired( null);
			Timer.getDefaultTimer().addClient( this, 300, 100);
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
	if ( hasToNotify( AWTEvent.ADJUSTMENT_EVENT_MASK, aListener) ) {
		AdjustmentEvent ae = AWTEvent.getAdjustmentEvent( this, 0);
		ae.setAdjustmentEvent( currentOp, val);
		Toolkit.eventQueue.postEvent( ae);
	}
}

public void paint ( Graphics g ) {
	int x, y, xw, yh;

	if ( slRect.width == 0 ) {
		g.setColor( parent.getBackground() );
		g.fillRect( 0, 0, width, height);
		return;
	}
	
	g.setColor( getBackground());

	if ( ori == HORIZONTAL) {
		y = height/2 -1;
		xw = slRect.x + slRect.width;
	
		if ( slRect.x > 0 )
			g.draw3DRect( 0, y, slRect.x, 1, true);
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

	if ( slRect.width > 0)
		g.fill3DRect( slRect.x, slRect.y, slRect.width, slRect.height, true);
}

protected String paramString() {
	return super.paramString() + ",Value: " + val + ",Visible: " + vis + ",Min: " + min + ",Max: " + max; 
}

protected void processAdjustmentEvent ( AdjustmentEvent e) {
	aListener.adjustmentValueChanged( e);
}

protected void processEvent ( AWTEvent e) {
	if ( e instanceof AdjustmentEvent)
		processAdjustmentEvent( (AdjustmentEvent) e);
	else
		super.processEvent( e);
}

public synchronized void removeAdjustmentListener( AdjustmentListener al) {
	aListener = AWTEventMulticaster.remove( aListener, al);
}

public synchronized void setBlockIncrement( int inc) {
	setValues( val, inc, min, max);
}

public void setBounds( int x, int y, int width, int height) {
	super.setBounds( x, y, width, height);
	updateSliderRect();
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

public synchronized void setUnitIncrement( int inc) {
	unitInc = inc;
}

public synchronized void setValue( int value) {
	if ( val != value)
		setValues( value, vis, min, max);
}

public synchronized void setValues( int value, int visible, int min, int max) {
	int lastVal = this.val;

	// Make size adjustments.
	if (max < min) {
		max = min;
	}
	if (value > max) {
		value = max;
	}
	if (value < min) {
		value = min;
	}
	if (visible > max-min) {
		visible = max-min;
	}
	
	this.min = min;
	this.max = max;
	this.val = value;
	this.vis = visible;
	this.blockInc = visible;
	
	if ( this.val < min ) {
		if ( this.val == min )
			return;
		this.val = min;
	}
	else if ( this.val > this.max ) {
		if ( this.val == this.max )
			return;
		this.val = this.max;
	}
		
	update();
	
	if ( this.val != lastVal)
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
	Graphics g;
	
	if ( dragGr != null )
		g = dragGr;
	else
		g = getGraphics();
	
	if ( g != null ){
		g.setColor( parent.getBackground());
		g.fillRect( slRect.x, slRect.y, slRect.width, slRect.height);

		updateSliderRect();
		
		paint( g);	
		if ( g != dragGr ) g.dispose();
	}
	else
		updateSliderRect();
}

void updateSliderRect () {
	int delta = max - min;

	//entire contents visible
	if ( delta <= 0 || vis <= 0) {
		slRect.setBounds( 0, 0, 0, 0);
		return;
	}

	//slider position from value
	if ( ori == HORIZONTAL ) {
		int dx = Math.max( vis * width / (delta + vis), 10);
		int x0 = (val - min) * ( width - dx ) / delta;
		if ( x0 >= 0 )
			slRect.setBounds( x0, 2, dx, height-4);
		else
			slRect.setBounds( 0, 0, 0, 0);
	}
	else {
		//total amount is max - min + vis, for contents cannot be scrolled out
		int dy = Math.max( vis * height / (delta + vis), 10);
		int y0 = (val - min) * ( height - dy ) / delta;
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
		newVal = slRect.x * ( max - min) / (width - slRect.width ) + min;
	else
		newVal = slRect.y * ( max - min) / (height - slRect.height) + min;
	
	if ( val != newVal ) {
		val = newVal;
		notifyAdjust();
	}
}
}
