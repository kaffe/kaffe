package java.awt.event;

import java.awt.AWTEvent;
import java.awt.Adjustable;

/**
 * class AdjustmentEvent - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
public class AdjustmentEvent
  extends AWTEvent
{
	int adjType;
	int adjVal;
	final public static int UNIT_INCREMENT = 1;
	final public static int UNIT_DECREMENT = 2;
	final public static int BLOCK_DECREMENT = 3;
	final public static int BLOCK_INCREMENT = 4;
	final public static int TRACK = 5;
	final public static int ADJUSTMENT_FIRST = 601;
	final public static int ADJUSTMENT_LAST = 601;
	final public static int ADJUSTMENT_VALUE_CHANGED = ADJUSTMENT_FIRST;

public AdjustmentEvent ( Adjustable src, int evtId, int type, int value ) {
	super( src, evtId);
	
	adjType = type;
	adjVal = value;
}

protected void dispatch() {
	processAdjustmentEvent( this);
	recycle();
}

public Adjustable getAdjustable () {
	return (Adjustable) source;
}

public int getAdjustmentType () {
	return adjType;
}

public int getValue () {
	return adjVal;
}

public String paramString () {
	String s;
	
	if ( id == ADJUSTMENT_VALUE_CHANGED )
		s = "ADJUSTMENT_VALUE_CHANGED";
	else
		s = "ADJUSTMENT:" + id;

	s += ", type:";

	switch ( adjType ) {
	case UNIT_INCREMENT:  s += "UNIT_INCREMENT"; break;
	case UNIT_DECREMENT:  s += "UNIT_DECREMENT"; break;
	case BLOCK_INCREMENT: s += "BLOCK_INCREMENT"; break;
	case BLOCK_DECREMENT: s += "BLOCK_DECREMENT"; break;
	case TRACK:           s += "TRACK"; break;
	}

	return s + ", val:" + adjVal;
}

protected void recycle() {
	synchronized ( evtLock ) {
		source = null;

		next = adjEvtCache;	
		adjEvtCache = this;
	}
}

protected void setAdjustmentEvent( int adjType, int adjVal) {
	this.adjType = adjType;
	this.adjVal = adjVal;
}
}
