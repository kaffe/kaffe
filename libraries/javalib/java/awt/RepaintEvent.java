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

package java.awt;

class RepaintEvent
  extends AWTEvent
{
	long issued;
	long ms;
	int x;
	int y;
	int width;
	int height;

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
	}
	else {
		Toolkit.eventQueue.postEvent( this);
	}
}
}
