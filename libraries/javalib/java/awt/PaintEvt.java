/*
 * PaintEvt
 *
 * Copyright (c) 1998
 *	Transvirtual Technologies Inc.  All rights reserved.
 *
 * Copyright (c) 2004
 *	Kaffe.org contributors. See ChangeLog for details.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author P.C.Mehlitz
 */

package java.awt;

import java.awt.event.PaintEvent;

class PaintEvt
  extends PaintEvent
{
	long issued;
	long ms;
	static PaintEvt cache;
	final static int REPAINT = 802;

PaintEvt ( Component src, int evtId, long ms, int x, int y, int width, int height ) {
	super( src, evtId, x, y, width, height);
	
	// we don't support "lazyness" control (ms is an *upper* bound)
	this.ms = ms;
	this.issued = -1;
}

protected void dispatch () {
	if ( (width > 0) && (height > 0) ) {
		Component c = (Component) source;
		if ( (c.flags & Component.IS_SHOWING) == Component.IS_SHOWING ) {
			c.processPaintEvent( id, x, y, width, height);
		}
	}
	recycle();
}

static synchronized PaintEvt getEvent ( Component source, int id,
                      long ms, int x, int y, int width, int height ) {
	if ( cache == null ) {
		return new PaintEvt( source, id, ms, x, y, width, height);
	}
	else {
		PaintEvt e = cache;
		cache = (PaintEvt) e.next;
		e.next = null;
		
		e.source = source;
		e.id = id;
		e.x = x;
		e.y = y;
		e.width = width;
		e.height = height;

		// we don't support "lazyness" control (ms is an *upper* bound)
		e.ms = ms;
		e.issued = -1;

		return e;
	}
}

static synchronized PaintEvt getEvent ( int srcIdx, int id, int x, int y, int width, int height ) {
	PaintEvt e;
	Component source = sources[srcIdx];

	if ( (Toolkit.flags & Toolkit.EXTERNAL_DECO) != 0 ) {
		x += source.deco.x;
		y += source.deco.y;
	}

	if ( cache == null ) {
		e = new PaintEvt( source, id, 0, x, y, width, height);
	}
	else {
		e = cache;
		cache = (PaintEvt) e.next;
		e.next = null;
	
		e.source = source;
		e.id = id;
		e.ms = 0;
		e.issued = -1;
		e.x = x;
		e.y = y;
		e.width = width;
		e.height = height;
	}

	if ( (Toolkit.flags & Toolkit.NATIVE_DISPATCHER_LOOP) != 0 ) {
		// this is not used as a direct return value for EventQueue.getNextEvent(), 
		// it has to be Java-queued by the native layer
		Toolkit.eventQueue.postEvent( e);
	}

	return e;
}

public Rectangle getUpdateRect () {
	return new Rectangle( x, y, width, height);
}

protected boolean isObsoletePaint( Object src, int x, int y, int w, int h ){
	return ((src == source) &&
	        (x <= this.x) && (y <= this.y) &&
	        ((x+w) >= (this.x + this.width)) &&
	        ((y+h) >= (this.y + this.height)));
}

public String paramString() {
	String ps;
	
	switch ( id ) {
	case PAINT:   ps = "PAINT"; break;
	case UPDATE:  ps = "UPDATE"; break;
	case REPAINT: ps = "REPAINT"; break;
	default:      ps = ("unknown paint " + id);
	}

	ps += " [" + x + ',' + y + ',' + width + ',' + height + ']';

	return ps;
}

protected void recycle () {
	synchronized ( PaintEvt.class ) {
		source = null;

		next = cache;
		cache = this;
	}
}

public void setUpdateRect ( Rectangle r ) {
	x = r.x;
	y = r.y;
	width = r.width;
	height = r.height;
}

boolean solicitRepaint ( Component c, int cx, int cy, int cw, int ch ) {
	// our policy is not to propagate repaints upwards in the parent chain, since
	// this can cause rather big repaint areas, resulting in flicker
	if ( c != source ) return false;

	int xw = x + width;
	int yh = y + height;
	int cxw = cx + cw;
	int cyh = cy + ch;
	
	if ( cx < x ) x = cx;
	if ( cy < y ) y = cy;
	
	width = ((cxw > xw) ? cxw : xw) - x;
	height = ((cyh > yh) ? cyh : yh) - y;

	return true;
}
}
