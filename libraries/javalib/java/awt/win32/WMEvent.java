package java.awt;

import java.awt.AWTEvent;
import java.awt.Component;
import java.awt.Toolkit;


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
class WMEvent
  extends AWTEvent
{
	final static int WM_CREATE = 1901;
	final static int WM_DESTROY = 1902;
	final static int WM_DISPATCH_MODAL = 1903;
	final static int WM_SHOW = 1904;
	final static int WM_KILLED = 1905;
	static WMEvent cache;

WMEvent ( Component source, int id ) {
	super( source, id);
}

protected void dispatch () {
	Component c = (Component) source;
	Window w = ( c instanceof Window ) ? (Window) c : null;

	switch ( id ) {
		
	case WM_CREATE:
		c.addNotify();
		synchronized (this) { this.notifyAll(); }
		break;
		
	case WM_DESTROY:
		c.removeNotify();
		synchronized (this) { this.notifyAll(); }
		break;
		
	case WM_DISPATCH_MODAL:
		if ( w != null ) {
			Toolkit.eventThread.run( w);
			synchronized (this) {
			  this.notifyAll();
			}
		}
		break;
		
	case WM_SHOW:
		if ( w != null ) {
			// make the window visible *before* waking up any waiters!
			Toolkit.eventThread.show( w);
			synchronized (this) { this.notifyAll(); }
		}
		break;
	
	case WM_KILLED:
		if ( w != null ) {
			// we got an external kill on one of our windows, clean up so
			// that we don't get danglingwindows
			w.cleanUp();
		}
		break;	
	}
	
	recycle();
}

static synchronized WMEvent getEvent ( Component source, int id ) {
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

static synchronized WMEvent getEvent ( int srcIdx, int id ) {
	WMEvent e;
	Component  source = sources[srcIdx];

	if ( source == null ) return null;

	if ( cache == null ){
		e = new WMEvent( source, id);
	}
	else {
		e = cache;
		cache = (WMEvent) e.next;
		e.next = null;
		
		e.id = id;
		e.source = source;
	}

	if ( (Toolkit.flags & Toolkit.NATIVE_DISPATCHER_LOOP) != 0 ) {
		// this is not used as a direct return value for EventQueue.getNextEvent(), 
		// it has to be Java-queued by the native layer
		Toolkit.eventQueue.postEvent( e);
	}
	
	return e;
}

protected void recycle () {
	synchronized ( WMEvent.class ) {
		source = null;

		next = cache;	
		cache = this;
	}
}
}
