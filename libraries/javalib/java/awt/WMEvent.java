/**
 * WMEvent - 
 *
 * Copyright (c) 1998
 *      Transvirtual Technologies, Inc.  All rights reserved.
 *
 * See the file "license.terms" for information on usage and redistribution 
 * of this file. 
 *
 * @author P. Mehlitz
 */

package java.awt;


class WMEvent
  extends AWTEvent
{
	final static int WM_CREATE = 1901;
	final static int WM_DESTROY = 1902;
	final static int WM_DISPATCH_MODAL = 1903;
	final static int WM_SHOW = 1904;
	static WMEvent cache;

WMEvent ( Window source, int id ) {
	super( source, id);
}

protected void dispatch () {

	Window w = (Window) source;

	switch ( id ) {
		
	case WM_CREATE:
		w.addNotify();
		synchronized (this) { this.notifyAll(); }
		break;
		
	case WM_DESTROY:
		w.removeNotify();
		synchronized (this) { this.notifyAll(); }
		break;
		
	case WM_DISPATCH_MODAL:
		Toolkit.eventThread.run( w);
		synchronized (this) { this.notifyAll(); }
		break;
		
	case WM_SHOW:
		// make the window visible *before* waking up any waiters!
		Toolkit.eventThread.show( w);
		synchronized (this) { this.notifyAll(); }
		break;
	}
	
	recycle();
}

static synchronized WMEvent getEvent ( Window source, int id ) {
	if ( cache == null ){
		return new WMEvent( source, id);
	}
	else {
		WMEvent e = cache;
		cache = (WMEvent) e.next;
		e.next = null;
		
		e.id = id;
		e.source = source;
		
		return e;
	}
}

protected void recycle () {
	synchronized ( WMEvent.class ) {
		source = null;

		next = cache;	
		cache = this;
	}
}
}
