package java.awt;

import java.awt.event.AdjustmentEvent;
import java.awt.event.AdjustmentListener;

/**
 * class Scrollbar - 
 *
 * Copyright (c) 1999
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 */
public class Scrollbar
  extends NativeComponent
  implements Adjustable
{
	final public static int HORIZONTAL = 0;
	final public static int VERTICAL = 1;
	int min;
	int max;
	int vis;
	int val;
	int ori;
	int unitInc;
	int blockInc;
	AdjustmentListener aListener;

public Scrollbar() {
	this( VERTICAL);
}

public Scrollbar( int ori) throws IllegalArgumentException {
	this( ori, 0, 10, 0, 100);
}

public Scrollbar( int ori, int value, int visible, int min, int max) throws IllegalArgumentException{
	setBackground( Defaults.BtnClr);
	setOrientation( ori);
	setValues( value, visible, min, max);
}

public synchronized void addAdjustmentListener( AdjustmentListener al) {
	aListener = AWTEventMulticaster.add( aListener, al);
}

public void addNotify() {
	if ( nativeData == null ) {
		Toolkit.createNative(this);
		super.addNotify();
		Toolkit.scrollSetValues( nativeData, val, min, max, vis);
	}
}

void createNative() {
	nativeData = Toolkit.scrollCreateScrollbar( getParentData(), ori == VERTICAL);
}

public int getBlockIncrement() {
	return (getPageIncrement());
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

protected String paramString() {
	return super.paramString() + ",Value: " + val + ",Visible: " + vis + ",Min: " + min + ",Max: " + max; 
}

/**
 * @deprecated
 */
public Dimension preferredSize() {
	if ( ori == VERTICAL ) {
		return new Dimension(  Defaults.ScrollbarWidth, 100);
	}
	else {
		return new Dimension( 100, Defaults.ScrollbarWidth);
	}
}

void process ( AdjustmentEvent e ) {
	val = e.getValue();

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

	if ( nativeData != null ) {
		Toolkit.scrollSetOrientation( nativeData, ori == VERTICAL);
	}
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

public synchronized void setValues( int value, int visible, int min, int max) {
	int lastVal = this.val;

	if ( (value == val) && (visible == vis) && (min == this.min) && (max == this.max) )
		return;

	// Make size adjustments.
	if (max < min)                  max = min;
	if (visible > max-min)          visible = max-min;
	if ( value > max - visible )	value = max - visible;
	else if ( value < min )         value = min;

	this.min = min;
	this.max = max;
	this.val = value;
	this.vis = visible;
	this.blockInc = visible;

	if ( ((flags & IS_ADD_NOTIFIED) != 0) && (nativeData != null) ) {
		Toolkit.scrollSetValues( nativeData, value, min, max, visible);
	}
}

public synchronized void setVisibleAmount( int vis) {
	setValues( val, vis, min, max);
}
}
