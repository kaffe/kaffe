package java.awt;

import java.awt.event.FocusEvent;
import java.awt.event.WindowEvent;

class FocusEvt
  extends FocusEvent
{
	static FocusEvt cache;
	static Component keyTgtRequest;
	static Window newActiveWindow;

FocusEvt ( Component src, int evtId, boolean isTemporary ) {
	super( src, evtId, isTemporary);
}

static void checkActiveWindow( Component c) {
	Component top = c.getToplevel();

	if ( (top != AWTEvent.activeWindow) && (top != null) ) {
		FocusEvt e = getEvent( top, FocusEvent.FOCUS_GAINED, false);
		e.dispatch();
	}
}

protected void dispatch () {
	WindowEvt  we;
	Component  src = (Component) source;

	if ( id == FOCUS_GAINED ) {
		if ( (keyTgtRequest != null) ) {
			// There is an obscure WM behavior (e.g. in KWM), first sending
			// a FOCUS_LOST to a freshly opened window. If we just reset keyTgtRequest
			// on FOCUS_LOST, we loose the forwarding. If we don't do that, we better
			// check here that the keyTgtRequest is for us
			if ( getToplevel( keyTgtRequest) == src )
				src = keyTgtRequest;
			keyTgtRequest = null;
		}		

		// we should make the next active window known (within java.awt)
		// prior to sending the DEACTIVATED messages because RootWindow
		// instances might have to react on this
		newActiveWindow = (Window) getToplevel( src);

		if ( (AWTEvent.keyTgt != null) && (AWTEvent.keyTgt != src) ) {
			Component lastTgt = AWTEvent.keyTgt;
			source = lastTgt;
			id = FOCUS_LOST;
			AWTEvent.keyTgt = src;
			lastTgt.process( this);
		  id = FOCUS_GAINED;
		}

		if ( (newActiveWindow != AWTEvent.activeWindow) /* && (newActiveWindow instanceof Frame) */) {
		  if ( (AWTEvent.activeWindow != null) ) {
				we = WindowEvt.getEvent( AWTEvent.activeWindow, WindowEvent.WINDOW_DEACTIVATED);
				AWTEvent.activeWindow.process( we);
			}

			we = WindowEvt.getEvent( newActiveWindow, WindowEvent.WINDOW_ACTIVATED);
			newActiveWindow.process( we);
			AWTEvent.activeWindow = newActiveWindow;
		}

		AWTEvent.keyTgt = src;
		source = src;
		src.process( this);
		
		ShortcutHandler.buildCodeTable( src);
	}
	else if ( id == FOCUS_LOST ) {
		if ( AWTEvent.keyTgt != null ) {
			source = AWTEvent.keyTgt;
			inputModifier = 0; // just a matter of safety (a reset point)

			AWTEvent.keyTgt.process( this);
			AWTEvent.keyTgt = null;
		}

		if ( src == AWTEvent.activeWindow ) { // native generated focus lost
			we = WindowEvt.getEvent( AWTEvent.activeWindow, WindowEvent.WINDOW_DEACTIVATED);
			AWTEvent.activeWindow.process( we);
			AWTEvent.activeWindow = null;
		}
	}
	
	if ( (Defaults.RecycleEvents & AWTEvent.FOCUS_EVENT_MASK) != 0 )	recycle();
}

static synchronized FocusEvt getEvent ( Component source, int id, boolean isTemporary ) {
	if ( cache == null ){
		return new FocusEvt( source, id, isTemporary);
	}
	else {
		FocusEvt e = cache;
		cache = (FocusEvt) e.next;
		e.next = null;
		
		e.id = id;
		e.source = source;
		e.isTemporary = isTemporary;

		return e;
	}
}

static synchronized FocusEvt getEvent ( int srcIdx, int id, boolean isTemporary ) {
	FocusEvt  e;
	Component source = sources[srcIdx];

	if ( source == null ) return null;

	if ( cache == null ){
		e = new FocusEvt( source, id, isTemporary);
	}
	else {
		e = cache;
		cache = (FocusEvt) e.next;
		e.next = null;
		
		e.id = id;
		e.source = source;
		e.isTemporary = isTemporary;
	}

	if ( (Toolkit.flags & Toolkit.NATIVE_DISPATCHER_LOOP) != 0 ) {
		// This is not used as a direct return value for EventQueue.getNextEvent(), 
		// it has to be Java-queued by the native layer.
		// Note that we use the specific postFocusEvent, because we have to take care of
		// focus solicitation for the Java queue. Otherwise, we might get several pending
		// focus events (this is called from the native side), which might lead to out-of-order
		// focus events in case the application forwards the focus (e.g. toplevel -> 1st child)
		Toolkit.eventQueue.postFocusEvent( e);
	}
	
	return e;
}

protected boolean isLiveEventFor ( Object src ) {
	return (source == src);
}

protected void recycle () {
	synchronized ( FocusEvt.class ) {
		source = null;

		next = cache;	
		cache = this;
	}
}
}
