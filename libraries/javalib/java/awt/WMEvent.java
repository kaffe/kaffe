package java.awt;


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
	final static int WM_KILLED = 1904;
	static WMEvent cache;

WMEvent ( Component source, int id ) {
	super( source, id);
}

protected void dispatch () {

	Component w = (Component) source;

	switch ( id ) {
		
	case WM_CREATE:
		w.createNative();
		synchronized (this) {
			this.notifyAll();
		}
		break;
		
	case WM_DESTROY:
		w.destroyNative();
		synchronized (this) {
			this.notifyAll();
		}
		break;
		
	case WM_DISPATCH_MODAL:
		Toolkit.eventThread.run( w);
		synchronized (this) {
			// we need to flag if our client (EventDispatchThread.run) needs to wait
			consumed = true;
		  this.notifyAll();
		}
		break;
			
	case WM_KILLED:
		// we got an external kill on one of our windows, clean up so
		// that we don't get dangling windows
		w.cleanUpNative();
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
	Window  source = (Window) sources[srcIdx];

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
