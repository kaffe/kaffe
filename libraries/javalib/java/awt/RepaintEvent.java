package java.awt;


/**
 * RepaintEvent -
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file.
 *
 * @author P.C.Mehlitz
 */
class RepaintEvent
  extends AWTEvent
{
	long issued;
	long ms;
	int x;
	int y;
	int width;
	int height;
	static RepaintEvent cache;

RepaintEvent ( Component src, long ms, int x, int y, int width, int height ) {
	super( src, 0);

	issued = System.currentTimeMillis();
	this.ms = ms;
	this.x = x;
	this.y = y;
	this.width  = width;
	this.height = height;
}

protected void dispatch () {
	if ( (System.currentTimeMillis() - issued) >= ms ){
		((Component)source).repaint( x, y, width, height);
		recycle();
	}
	else {
		Toolkit.eventQueue.postEvent( this);
	}
}

protected static RepaintEvent getRepaintEvent( Component src, long tm, int x, int y, int w, int h ) {
	synchronized ( evtLock ) {
		if ( cache == null ) {
			return new RepaintEvent( src, tm, x, y, w, h);
		}
		else {
			RepaintEvent e = cache;
			cache = (RepaintEvent) e.next;
			e.next = null;
		
			e.source = src;
			e.id = 0;
	
			e.issued = System.currentTimeMillis();
			e.ms = tm;
			e.x = x;
			e.y = y;
			e.width = w;
			e.height = h;

			return e;
		}
	}
}

protected void recycle () {
	synchronized ( evtLock ) {
		source = null;
		
		next = cache;
		cache = this;
	}
}
}
