package java.awt;

import java.awt.event.FocusEvent;
import java.awt.event.WindowEvent;

class FocusEvt
  extends FocusEvent
{
	static FocusEvt cache;
	static Component keyTgtRequest;
	static Window newActiveWindow;

FocusEvt ( Component src, int evtId ) {
	super( src, evtId);
}

static void checkActiveWindow( Component c) {
	Component top = c.getToplevel();

	if ( (top != activeWindow) && (top != null) ) {
		FocusEvt e = getEvent( top, FocusEvent.FOCUS_GAINED);
		e.dispatch();
	}
}

protected void dispatch () {
	WindowEvt  we;
	Component  src = (Component) source;
	Component  lastTgt;

	if ( id == FOCUS_GAINED ) {
		if ( keyTgtRequest != null ) {
			src = keyTgtRequest;
			keyTgtRequest = null;
		}
		
		if ( keyTgt == src ) return; // nothing to do
		
		if ( (keyTgt != null) && (keyTgt != src) ) {
			id = FOCUS_LOST;
			source = lastTgt = keyTgt;
			keyTgt = src;
			lastTgt.processEvent( this);
		  id = FOCUS_GAINED;
		}

		// we should make the next active window known (within java.awt)
		// prior to sending the DEACTIVATED messages because RootWindow
		// instances might have to react on this
		newActiveWindow = (Window) getToplevel( src);
		if ( (activeWindow != null) && (newActiveWindow != activeWindow)
		       && ((newActiveWindow instanceof Frame) || (newActiveWindow == root)) ) {
			we = WindowEvt.getEvent( activeWindow, WindowEvent.WINDOW_DEACTIVATED);
			activeWindow.processEvent( we);
		}

		keyTgt = src;
		source = src;
		src.processEvent( this);
		
		if ( (newActiveWindow != activeWindow) && (newActiveWindow instanceof Frame) ) {
			we = WindowEvt.getEvent( newActiveWindow, WindowEvent.WINDOW_ACTIVATED);
			newActiveWindow.processEvent( we);
			activeWindow = (Window) newActiveWindow;
		}
		else if ( newActiveWindow == root )
			activeWindow = null;
	}
	else if ( id == FOCUS_LOST ) {
		if ( src == activeWindow ) { // native generated focus lost
			we = WindowEvt.getEvent( activeWindow, WindowEvent.WINDOW_DEACTIVATED);
			activeWindow.processEvent( we);
			activeWindow = null;
		}

		if ( keyTgt != null ) {
			src    = keyTgt;
			source = src;
			keyTgt = null;
			inputModifier = 0; // just a matter of safety (a reset point)

			src.processEvent( this);			
		}
		
		// sledge hammer for (buggy?) WMs sending focus losts without prior gains
		keyTgtRequest = null;
	}
	
	recycle();
}

static synchronized FocusEvt getEvent ( Component source, int id ) {
	if ( cache == null ){
		return new FocusEvt( source, id);
	}
	else {
		FocusEvt e = cache;
		cache = (FocusEvt) e.next;
		e.next = null;
		
		e.id = id;
		e.source = source;
		
		return e;
	}
}

static synchronized FocusEvt getEvent ( int srcIdx, int id ) {
	Component source = sources[srcIdx];

	if ( cache == null ){
		return new FocusEvt( source, id);
	}
	else {
		FocusEvt e = cache;
		cache = (FocusEvt) e.next;
		e.next = null;
		
		e.id = id;
		e.source = source;
		
		return e;
	}
}

protected void recycle () {
	synchronized ( FocusEvt.class ) {
		source = null;

		next = cache;	
		cache = this;
	}
}

static void sendFocusEvent ( Component src, boolean focusGained, boolean sync ) {
	int id = focusGained ? FOCUS_GAINED : FOCUS_LOST;
	FocusEvt e = getEvent( src, id);

	if ( sync )
		e.dispatch();
	else
		Toolkit.eventQueue.postEvent( e);
}
}
