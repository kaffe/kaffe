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
	Window src = (Window)source;

	if ( id == WINDOW_ICONIFIED ) {
		src.flags &= ~Component.IS_PARENT_SHOWING;
		src.propagateParentShowing();
	}
	else if ( id == WINDOW_DEICONIFIED ) {
		src.flags |= Component.IS_PARENT_SHOWING;
		src.propagateParentShowing();
	}

	src.processEvent( this);
	if ( (Defaults.RecycleEvents & AWTEvent.WINDOW_EVENT_MASK) != 0 )	recycle();
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

	// this protects us from "normal" window destruction triggered by removeNotify()
	if ( source == null ) return null;

	if ( (id == WINDOW_ICONIFIED) || (id == WINDOW_DEICONIFIED) )
		PopupWindow.checkPopup( source); // close any open popups

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
