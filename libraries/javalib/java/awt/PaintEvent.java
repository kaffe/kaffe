package java.awt;

import java.awt.Component;
import java.awt.Graphics;
import java.awt.Rectangle;

/**
 *
 * Copyright (c) 1998
 *   Transvirtual Technologies Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 * @author P.C.Mehlitz
 */
class PaintEvent
  extends AWTEvent
{
	long issued;
	long ms;
	int x;
	int y;
	int width;
	int height;
	static PaintEvent cache;
	final public static int PAINT_FIRST = 800;
	final public static int PAINT = 800;
	final public static int UPDATE = 801;
	final public static int REPAINT = 802;
	final public static int PAINT_LAST = 802;

PaintEvent ( Component src, int evtId, long ms, int x, int y, int width, int height ) {
	super( src, evtId);

	this.x = x;
	this.y = y;
	this.width = width;
	this.height = height;
	
	// we don't support "lazyness" control (ms is an *upper* bound)
	this.ms = ms;
	this.issued = -1;
}

protected void dispatch () {
	Component src = (Component)source;
	NativeGraphics g = NativeGraphics.getClippedGraphics( null, src, 0,0,
	                                                      x, y, width, height, true);	
	if ( g != null ){
		src.update( g);
		g.dispose();
	}
	
	recycle();
}

static synchronized PaintEvent getEvent ( Component source, int id,
                      long ms, int x, int y, int width, int height ) {
	if ( cache == null ) {
		return new PaintEvent( source, id, ms, x, y, width, height);
	}
	else {
		PaintEvent e = cache;
		cache = (PaintEvent) e.next;
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

static synchronized PaintEvent getEvent ( int srcIdx, int id, int x, int y, int width, int height ) {
	Component source = sources[srcIdx];
	Rectangle r;

	x += source.deco.x;
	y += source.deco.y;

	if ( cache == null ) {
		return new PaintEvent( source, id, 0, x, y, width, height);
	}
	else {
		PaintEvent e = cache;
		cache = (PaintEvent) e.next;
		e.next = null;
	
		e.source = source;
		e.id = id;
		e.ms = 0;
		e.issued = -1;
		e.x = x;
		e.y = y;
		e.width = width;
		e.height = height;

		return e;
	}
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
	synchronized ( PaintEvent.class ) {
		source = null;

		next = cache;
		cache = this;
	}
}

boolean solicitRepaint ( Component c, int cx, int cy, int cw, int ch ) {
	// our policy is not to propagate repaints upwards in the parent chain, since
	// this can cause rather big repaint areas, resulting in flicker
	if ( c != source ) return false;
	
	int xw = x + width;
	int yh = x + height;
	int cxw = cx + cw;
	int cyh = cy + ch;
	
	if ( cx < x ) x = cx;
	if ( cy < y ) y = cy;
	
	width = ((cxw > xw) ? cxw : xw) - x;
	height = ((cyh > yh) ? cyh : yh) - y;

	return true;
}
}
