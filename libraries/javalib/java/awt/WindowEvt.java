package java.awt;

import java.awt.event.WindowEvent;

class WindowEvt
  extends WindowEvent
{
	static WindowEvt cache;

WindowEvt ( Window src, int evtId ){
	super( src, evtId);
}

protected void dispatch () {
	Component src = (Component)source;

	if ( id == WINDOW_CLOSED )
		src.removeNotify();

	src.processEvent( this);
	recycle();
}

static synchronized WindowEvt getEvent ( Window source, int id ) {
	if ( cache == null ){
		return new WindowEvt( source, id);
	}
	else {
		WindowEvt e = cache;
		cache = (WindowEvt) e.next;
		e.next = null;
		
		e.id = id;
		e.source = source;
		
		return e;
	}
}

static synchronized WindowEvt getEvent ( int srcIdx, int id ) {
	Component source = sources[srcIdx];

	if ( cache == null ){
		return new WindowEvt( (Window)source, id);
	}
	else {
		WindowEvt e = cache;
		cache = (WindowEvt) e.next;
		e.next = null;
		
		e.id = id;
		e.source = source;
		
		return e;
	}
}

protected void recycle () {
	synchronized ( WindowEvt.class ) {
		source = null;

		next = cache;	
		cache = this;
	}
}

static void sendWindowEvent ( Window src, int id, boolean sync ) {
	WindowEvt e = getEvent( src, id);
	
	if ( sync )
		e.dispatch();
	else
		Toolkit.eventQueue.postEvent( e);
}
}
