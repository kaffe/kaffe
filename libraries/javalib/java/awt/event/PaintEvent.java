package java.awt.event;

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
public class PaintEvent
  extends ComponentEvent
{
	protected Rectangle updateRect;
	final public static int PAINT_FIRST = 800;
	final public static int PAINT = 800;
	final public static int UPDATE = 801;
	final public static int PAINT_LAST = 801;

public PaintEvent ( Component src, int evtId, Rectangle rect ) {
	super( src, evtId);

	updateRect = rect;
}

protected void dispatch () {
	processPaintEvent( this);
	recycle();
}

static PaintEvent getPaintEvent ( int srcIdx, int id, int x, int y, int width, int height ) {
	Component source = sources[srcIdx];
	Rectangle r;

	synchronized ( evtLock ) {
		if ( paintEvtCache == null ) {
			return new PaintEvent( source, id, new Rectangle( x, y, width, height));
		}
		else {
			PaintEvent e = paintEvtCache;
			paintEvtCache = (PaintEvent) e.next;
			e.next = null;
		
			e.source = source;
			e.id = id;

			r = e.updateRect;
			r.x = x;
			r.y = y;
			r.width = width;
			r.height = height;

			return e;
		}
	}
}

public Rectangle getUpdateRect() {
	return updateRect;
}

public String paramString() {
	String ps;
	
	if ( id == PAINT )       ps = "PAINT";
	else if ( id == UPDATE ) ps = "UPDATE";
	else                     ps = "Paint: " + id;

	ps += " [" + updateRect.x + ',' + updateRect.y +
	          ',' + updateRect.width + ',' + updateRect.height + ']';

	return ps;
}

protected void recycle () {
	synchronized ( evtLock ) {
		source = null;

		next = paintEvtCache;	
		paintEvtCache = this;
	}
}

public void setUpdateRect ( Rectangle rect ) {
	updateRect = rect;
}
}
